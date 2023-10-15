import os
from uuid import UUID
from glob import glob
from struct import unpack
from unittest import TestCase

from emessgee import Publisher
from emessgee.exceptions import (
    DataTooLargeError, DataNotBytesOrStringError, PublisherAlreadyExistsError
)
from emessgee.constants import (
    TMP_FOLDER, DEFAULT_BUFFER_SIZE, RESERVED_BYTES, HEADER_START, HEADER_END,
    STRUCT_FORMAT, ID_BYTES_ENDIEN, WRITING_FLAG_INDEX
)

class TestPublisher(TestCase):
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
            self.assertEqual(len(written_data), DEFAULT_BUFFER_SIZE + RESERVED_BYTES)

        publisher.close()
    
    def test_constructor_customBuffersize_bufferSetToCorrectSize(self):
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
            self.assertEqual(len(written_data), buffer_size + RESERVED_BYTES)

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
            header = written_data[HEADER_START:HEADER_END]
            index, message_size, id_bytes = unpack(STRUCT_FORMAT, header)
            message_id = UUID(int=int.from_bytes(id_bytes, ID_BYTES_ENDIEN))

            self.assertEqual(index, RESERVED_BYTES)
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
            self.assertTrue(written_data[WRITING_FLAG_INDEX])

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
            header = written_data[HEADER_START:HEADER_END]
            index, _, _ = unpack(STRUCT_FORMAT, header)
            self.assertEqual(index, RESERVED_BYTES)
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
        num_sends = 5
        buffer_size = 100
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic, buffer_size)
        data = 55.5

        #Act/Assert
        with self.assertRaises(DataNotBytesOrStringError):
            publisher.send(data)

        publisher.close()

    def test_close_calledTwice_nothingHappens(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)

        #Act
        publisher.close()
        publisher.close()