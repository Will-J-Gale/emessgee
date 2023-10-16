import os
from uuid import UUID
from glob import glob
from unittest import TestCase
from unittest.mock import MagicMock, patch

from emessgee import Publisher, Subscriber
from emessgee.constants import TMP_FOLDER, MAX_SANITY_LOOPS, ReservedIndexes
from emessgee.exceptions import MMapFileExistsButNotYetTruncatedError

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
        self.assertIsNone(subscriber._memory_block)
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
        self.assertIsNotNone(subscriber._memory_block)
        publisher.close()
        subscriber.close()
    
     
    def test_constructor_correctlyReadsQueueSize(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        queue_size = 7
        publisher = Publisher(topic, queue_size=queue_size)

        #Act
        subscriber = Subscriber(topic)

        #Assert
        self.assertEqual(subscriber._queue_size, queue_size)
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
        before_memory_block = subscriber._memory_block
        publisher = Publisher(topic)

        #Act
        result = subscriber.recv()

        #Assert
        self.assertIsNone(before_memory_block)
        self.assertIsNotNone(subscriber._memory_block)
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
    
    def test_recv_queueSizeIs1_publisherSendsMultiple_subscriberOnlyReceivesLatestData(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        data1 = b"kwjhbcvirehbv"
        data2 = b"irvbeirjhvb"
        data3 = b"8042fu934"
        publisher = Publisher(topic, queue_size=1)
        publisher.send(data1)
        publisher.send(data2)
        publisher.send(data3)

        #Act
        result1 = subscriber.recv()
        result2 = subscriber.recv()
        result3 = subscriber.recv()

        #Assert
        self.assertEqual(result1, data3)
        self.assertEqual(result2, None)
        self.assertEqual(result3, None)
        publisher.close()
        subscriber.close()
    
    def test_recv_queueSizeIs5_publisherSendsOneMessage_subscriberReadsMultipleTimes_queueIndexOnlyIncrementedOnce(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        data1 = b"kwjhbcvirehbv"
        publisher = Publisher(topic, queue_size=5)
        publisher.send(data1)

        #Act
        result1 = subscriber.recv()
        result2 = subscriber.recv()
        result3 = subscriber.recv()
        result4 = subscriber.recv()

        #Assert
        self.assertEqual(result1, data1)
        self.assertEqual(result2, None)
        self.assertEqual(result3, None)
        self.assertEqual(result4, None)
        self.assertEqual(subscriber._queue_index, 1)
        publisher.close()
        subscriber.close()


    def test_recv_publisherSendsMultiple_subscriberReadsFromQueue(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        data1 = b"kwjhbcvirehbv"
        data2 = b"irvbeirjhvb"
        data3 = b"8042fu934"
        publisher = Publisher(topic, queue_size=5)
        publisher.send(data1)
        publisher.send(data2)
        publisher.send(data3)

        #Act
        result1 = subscriber.recv()
        result2 = subscriber.recv()
        result3 = subscriber.recv()

        #Assert
        self.assertEqual(result1, data1)
        self.assertEqual(result2, data2)
        self.assertEqual(result3, data3)
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
        index, data_size, message_id = subscriber._read_header(0)

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
        result = subscriber._read_header(0)

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
        subscriber._memory_block = MagicMock()
        subscriber._memory_block.read.side_effect = [1, 1, 1, 0]

        #Act
        subscriber._wait_for_writing()

        #Assert
        self.assertEqual(subscriber._memory_block.read.call_count, 4)
        subscriber.close()

    def test_waitForWriting_writerFlagNever0_maxSanityLoopsReached(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)
        subscriber._memory_block = MagicMock()

        #Act
        subscriber._wait_for_writing()

        #Assert
        self.assertEqual(subscriber._memory_block.read.call_count, MAX_SANITY_LOOPS)
        subscriber.close()
    
    def test_createBuffer_setsBufferProperly(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        subscriber = Subscriber(topic)
        publisher = Publisher(topic)

        #Act
        created = subscriber._create_memory_block()

        #Assert
        self.assertTrue(created)
        self.assertIsNotNone(subscriber._memory_block)
        publisher.close()
        subscriber.close()
    
    def test_createBuffer_sharedMemoryFileDoesNotExist(self):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)

        #Act
        created = subscriber._create_memory_block()

        #Assert
        self.assertFalse(created)
        self.assertIsNone(subscriber._memory_block)
        subscriber.close()
    
    @patch("emessgee.subscriber.MemoryBlock")
    def test_createMemoryBlock_fileExistsButBufferNotTruncated_loopsUntilFileIsReady(self, mock_memory_block):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        subscriber = Subscriber(topic)
        mock_buffer = MagicMock()
        mock_buffer.read.return_value = b"\x07"
        mock_memory_block.side_effect = [
            MMapFileExistsButNotYetTruncatedError, 
            MMapFileExistsButNotYetTruncatedError, 
            MMapFileExistsButNotYetTruncatedError, 
            mock_buffer
        ]
        open(topic_filepath, "wb").close()

        #Act
        created = subscriber._create_memory_block()

        #Assert
        self.assertTrue(created)
        self.assertIsNotNone(subscriber._memory_block)
        self.assertEqual(mock_memory_block.call_count, 4)
        subscriber.close()
    
    @patch("emessgee.subscriber.MemoryBlock")
    def test_createMemoryBlock_memoryBlockCreatedButBlockNotReady_waitsUntilBlockIsReady(self, mock_memory_block_class):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        subscriber = Subscriber(topic)
        mock_block = MagicMock()

        try_index = -1
        return_values = [0, 0, 0, 1]
        def mock_read(index):
            nonlocal try_index
            if(index == ReservedIndexes.BLOCK_READY.value):
                try_index += 1
                return chr(return_values[try_index])
            else:
                return chr(7)

        mock_block.read.side_effect = mock_read
        mock_memory_block_class.return_value = mock_block
        open(topic_filepath, "wb").close()

        #Act
        created = subscriber._create_memory_block()

        #Assert
        self.assertTrue(created)
        self.assertIsNotNone(subscriber._memory_block)
        self.assertEqual(mock_memory_block_class.call_count, 4)
        self.assertEqual(mock_block.read.call_count, 5)
        subscriber.close()

    def test_close_calledTwice_nothingHappens(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        subscriber = Subscriber(topic)

        #Act
        subscriber.close() 
        subscriber.close() 
    
    def test_createSubscriberBeforePublisher_nothingHappens(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)

        #Act
        subscriber = Subscriber(topic)
        publisher = Publisher(topic)

        #Assert
        publisher.close()
        subscriber.close()