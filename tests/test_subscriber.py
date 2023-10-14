import os
import shutil
from uuid import UUID
from glob import glob
from unittest import TestCase
from unittest.mock import MagicMock, patch

from emessgee import Publisher, Subscriber
from emessgee.constants import TMP_FOLDER, MAX_SANITY_LOOPS

class TestSubscriber(TestCase):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{TMP_FOLDER}/*")]

    def test_constructor_sharedMemoryFileDoesNotExist_bufferIsNone(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)

        #Act
        subscriber = Subscriber(topic)

        #Assert
        self.assertFalse(os.path.exists(topic_filepath))
        self.assertIsNone(subscriber._buffer)
        subscriber.close()
    
    def test_constructor_sharedMemoryExists_bufferIsCreated(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        publisher = Publisher(topic)

        #Act
        subscriber = Subscriber(topic)

        #Assert
        self.assertTrue(os.path.exists(topic_filepath))
        self.assertIsNotNone(subscriber._buffer)
        publisher.close()
        subscriber.close()

    def test_recv_sharedMemoryFileDoesNotExist_returnsNone(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)

        #Act
        result = subscriber.recv()

        #Assert
        self.assertIsNone(result)
        subscriber.close()
    
    def test_recv_sharedMemoryFileCreatedAfterConstructor_createsBufferOnRecv(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        before_buffer = subscriber._buffer
        publisher = Publisher(topic)

        #Act
        result = subscriber.recv()

        #Assert
        self.assertIsNone(before_buffer)
        self.assertIsNotNone(subscriber._buffer)
        self.assertIsNone(result)
        publisher.close()
        subscriber.close()
    
    def test_recv_dataIsPublished_correctlyParsesAndReturnsData(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        data = b"test bytes ijrgbiejrvbierbv"
        publisher = Publisher(topic)
        publisher.send(data)

        #Act
        result = subscriber.recv()

        #Assert
        self.assertEqual(result, data)
        publisher.close()
        subscriber.close()
    
    def test_recv_dataIsPublished_readTwice_dataOnlyReturnedOnce(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        data = b"test bytes ijrgbiejrvbierbv"
        publisher = Publisher(topic)
        publisher.send(data)

        #Act
        result1 = subscriber.recv()
        result2 = subscriber.recv()

        #Assert
        self.assertEqual(result1, data)
        self.assertIsNone(result2)
        publisher.close()
        subscriber.close()
    
    def test_readHeader_headerCorrectlyParsed(self):
        #Assemble
        topic = "test_topic"
        data = b"test bytes ijrgbiejrvbierbv"
        publisher = Publisher(topic)
        publisher.send(data)
        subscriber = Subscriber(topic)

        #Act
        index, data_size, message_id = subscriber._read_header()

        #Assert
        self.assertIsNotNone(index)
        self.assertEqual(data_size, len(data))
        self.assertIsInstance(message_id, UUID)
        publisher.close()
        subscriber.close()
    
    def test_readHeader_bufferIsNone_noneIsReturned(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)

        #Act
        result = subscriber._read_header()

        #Assert
        self.assertIsNone(result)
        subscriber.close()
    
    def test_waitForWriting_bufferIsNone_imediatlyReturns(self):
        '''
            Looks like this test is doing nothing, but there is no nice was
            of testing the internals of _wait_for_writing when _buffer is None
        '''
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)

        #Act/Assert
        subscriber._wait_for_writing()
    
    def test_waitForWriting_returnsWhenWritingFlagIsFalse(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        mock_generater = MagicMock()
        mock_generater.side_effect = [1, 1, 1, 0]
        subscriber._buffer = MagicMock()
        subscriber._buffer.__getitem__ = mock_generater

        #Act
        subscriber._wait_for_writing()

        #Assert
        self.assertEqual(mock_generater.call_count, 4)
        subscriber.close()

    def test_waitForWriting_writerFlagNever0_maxSanityLoopsReached(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        mock_generater = MagicMock()
        subscriber._buffer = MagicMock()
        subscriber._buffer.__getitem__ = mock_generater

        #Act
        subscriber._wait_for_writing()

        #Assert
        self.assertEqual(mock_generater.call_count, MAX_SANITY_LOOPS)
        subscriber.close()
    
    def test_createBuffer_setsBufferProperly(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        subscriber = Subscriber(topic)
        publisher = Publisher(topic)

        #Act
        created = subscriber._create_buffer()

        #Assert
        self.assertTrue(created)
        self.assertIsNotNone(subscriber._buffer)
        publisher.close()
        subscriber.close()
    
    def test_createBuffer_sharedMemoryFileDoesNotExist(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)

        #Act
        created = subscriber._create_buffer()

        #Assert
        self.assertFalse(created)
        self.assertIsNone(subscriber._buffer)
        subscriber.close()
    
    @patch("emessgee.subscriber.mmap.mmap")
    def test_createBuffer_fileExistsButBufferNotTruncated_loopsUntilFileIsReady(self, mock_mmap):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        subscriber = Subscriber(topic)
        mock_buffer = MagicMock()
        mock_mmap.side_effect = [ValueError, ValueError, ValueError, mock_buffer]
        open(topic_filepath, "wb").close()

        #Act
        created = subscriber._create_buffer()

        #Assert
        self.assertTrue(created)
        self.assertIsNotNone(subscriber._buffer)
        self.assertEqual(mock_mmap.call_count, 4)
        subscriber.close()
    