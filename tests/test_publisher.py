import os
from uuid import UUID
from glob import glob
from struct import unpack
from unittest import TestCase

from emessgee import Publisher
from emessgee.header import header_from_bytes
from emessgee.exceptions import (
    DataTooLargeError, DataNotBytesOrStringError, PublisherAlreadyExistsError
)
from emessgee.constants import (
    TMP_FOLDER, DEFAULT_BUFFER_SIZE, HEADER_START, 
    HEADER_FORMAT, ID_BYTES_ENDIEN, 
    HEADER_LENGTH, INVALID_ID, INVALID_INDEX, INVALID_SIZE, ReservedIndexes
)

class TestPublisher(TestCase):
    def _read_header(self, queue_index, data_bytes):
        header_index = (queue_index * HEADER_LENGTH) + HEADER_START
        header_end = header_index + HEADER_LENGTH
        header_bytes = data_bytes[header_index:header_end]
        return header_from_bytes(header_bytes)

    def tearDown(self):
        [os.remove(file) for file in glob(f"{TMP_FOLDER}/*")]

    def test_constructor_createsFileForMemoryMapping(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)

        #Act
        publisher = Publisher(topic)

        #Assert
        self.assertTrue(os.path.exists(topic_filepath))
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            self.assertEqual(len(written_data), DEFAULT_BUFFER_SIZE + len(ReservedIndexes) + publisher._header_length)

        publisher.close()
    
    def test_constructor_customBufferSize_bufferSetToCorrectSize(self):
        #Assemble
        buffer_size = 111
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)

        #Act
        publisher = Publisher(topic, buffer_size)

        #Assert
        self.assertTrue(os.path.exists(topic_filepath))
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            self.assertEqual(len(written_data), buffer_size + len(ReservedIndexes) + publisher._header_length)

        publisher.close()
    
    def test_constructor_queueSize10_emptyHeadersCreatedInFileAndQueueSizeSet(self):
        #Assemble
        queue_size = 10
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)

        #Act
        publisher = Publisher(topic, queue_size=10)

        #Assert
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            written_queue_size = written_data[ReservedIndexes.QUEUE_SIZE.value]
            self.assertEqual(written_queue_size, queue_size)
            for i in range(queue_size):
                data_index, message_size, message_id = self._read_header(i, written_data)                

                self.assertEqual(data_index, INVALID_INDEX)
                self.assertEqual(message_size, INVALID_SIZE)
                self.assertEqual(message_id, INVALID_ID)

        publisher.close()
    
    def test_constructor_publishedAlreadyExists_errorRaised(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)

        #Act/Assert
        with self.assertRaises(PublisherAlreadyExistsError):
            Publisher(topic)

        publisher.close()
    
    def test_constructor_publishedAlreadyExistsButClosed_newPublishedCreated(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)
        publisher.close()

        #Act
        publisher2 = Publisher(topic)

        self.assertIsNotNone(publisher2)
        publisher2.close()
    
    def test_close_deletedSharedFile(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)

        #Act
        publisher.close()

        #Assert
        self.assertFalse(os.path.exists(topic_filepath))
    
    def test_send_writesDataToSharedFile(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)
        data = b"test data"

        #Act
        publisher.send(data)

        #Assert
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            self.assertIn(data, written_data)

        publisher.close()
    
    def test_send_calledTwice_headerQueueWrittenCorrectly(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic, queue_size=10)
        data = b"test data"

        #Act
        publisher.send(data)
        publisher.send(data)

        #Assert
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            data_index_1, message_size_1, message_id_1 = self._read_header(0, written_data)
            data_index_2, message_size_2, message_id_2 = self._read_header(1, written_data)

            self.assertNotEqual(data_index_1, INVALID_INDEX)
            self.assertEqual(message_size_1, len(data))
            self.assertNotEqual(message_id_1, INVALID_ID)

            self.assertNotEqual(data_index_2, INVALID_INDEX)
            self.assertEqual(message_size_2, len(data))
            self.assertNotEqual(message_id_2, INVALID_ID)

        publisher.close()
    
    def test_send_dataIsString_writesDataToSharedFile(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)
        data = "string data"

        #Act
        publisher.send(data)

        #Assert
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            self.assertIn(data.encode(), written_data)

        publisher.close()
    
    def test_send_headerCreatedCorrectly(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)
        data = b"test data"

        #Act
        publisher.send(data)

        #Assert
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            index, message_size, message_id = self._read_header(0, written_data)

            self.assertEqual(index, publisher._data_start)
            self.assertEqual(message_size, len(data))
            self.assertIsNotNone(message_id)

            message_data = written_data[index:index+message_size]
            self.assertEqual(message_data, data)

        publisher.close()

    def test_send_setsWritingFlag(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)
        publisher._end_write = lambda: None
        data = b"test data"

        #Act
        publisher.send(data)

        #Assert
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            self.assertTrue(written_data[ReservedIndexes.WRITING.value])

        publisher.close()
    
    def test_send_dataTooLarge_exceptionRaised(self):
        #Assemble
        topic = "test_topic"
        publisher = Publisher(topic, buffer_size=100)
        data = b"t" * 101

        #Act/Assert
        with self.assertRaises(DataTooLargeError):
            publisher.send(data)

        publisher.close()

    def test_send_writeIndexCorrectlyWrapsAround(self):
        #Assemble
        buffer_size = 100
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic, buffer_size)
        publisher._write_index = 99
        data = b"test data"

        #Act
        publisher.send(data)

        #Assert
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            index, _, _ = self._read_header(0, written_data) 
            self.assertEqual(index, publisher._data_start)
            self.assertLess(index, buffer_size)

        publisher.close()
    
    def test_send_multipleWrites_successfullyWritesAllData(self):
        #Assemble
        num_sends = 5
        buffer_size = 100
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic, buffer_size)
        data = b"test data"

        #Act
        for i in range(num_sends):
            publisher.send(data)

        #Assert
        with open(topic_filepath, "rb") as file:
            written_data = file.read()
            self.assertEqual(written_data.count(data), num_sends)


        publisher.close()
    
    def test_send_dataNotBytesOrString_errorRaised(self):
        #Assemble
        buffer_size = 100
        topic = "test_topic"
        publisher = Publisher(topic, buffer_size)
        data = 55.5

        #Act/Assert
        with self.assertRaises(DataNotBytesOrStringError):
            publisher.send(data)

        publisher.close()

    def test_close_calledTwice_nothingHappens(self):
        #Assemble
        topic = "test_topic"
        publisher = Publisher(topic)

        #Act
        publisher.close()
        publisher.close()