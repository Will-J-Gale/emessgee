import os
from glob import glob
from uuid import UUID
from unittest import TestCase
from unittest.mock import patch, MagicMock

from emessgee.memory_queue import ReadMemoryQueue, WriteMemoryQueue
from emessgee.constants import TMP_FOLDER, ReservedIndexes

class TestReadMemoryQueue(TestCase):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{TMP_FOLDER}/*")]

    def test_constructor_memoryFiepathDoesNotExist_objectCreatedAndReadBlockIsNone(self):
        #Assemble
        topic = "random_topic_name"

        #Act
        read_queue = ReadMemoryQueue(topic)

        #Assert
        self.assertIsNotNone(read_queue)
        self.assertIsNone(read_queue._memory_block)
    
    def test_constructor_memoryFiepathExistsButIsEmpty_objectCreatedAndReadBlockIsNone(self):
        #Assemble
        topic = "random_topic_name"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        open(topic_filepath, "wb").close()

        #Act
        read_queue = ReadMemoryQueue(topic)

        #Assert
        self.assertIsNotNone(read_queue)
        self.assertIsNone(read_queue._memory_block)
    
    @patch("emessgee.memory_queue.ReadMemoryBlock")
    def test_constructor_memoryBlockSuccessfullyCreated(self, mock_read_block):
        #Assemble
        topic = "random_topic_name"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        open(topic_filepath, "wb").close()
        mock_read_block().read.return_value = chr(1)

        #Act
        read_queue = ReadMemoryQueue(topic)

        #Assert
        self.assertIsNotNone(read_queue)
        self.assertIsNotNone(read_queue._memory_block)
        self.assertEqual(mock_read_block().read.call_count, 2)

    @patch("emessgee.memory_queue.ReadMemoryBlock")
    def test_constructor_writeBlockNotYetRead_loopsUntilReady(self, mock_read_block):
        #Assemble
        topic = "random_topic_name"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        open(topic_filepath, "wb").close()
        
        try_index = -1
        return_values = [0, 0, 0, 1]
        def mock_read(index):
            nonlocal try_index
            if(index == ReservedIndexes.BLOCK_READY.value):
                try_index += 1
                return chr(return_values[try_index])
            else:
                return chr(7)

        mock_memory_instance = MagicMock()
        mock_memory_instance.read.side_effect = mock_read
        mock_read_block.return_value = mock_memory_instance

        #Act
        read_queue = ReadMemoryQueue(topic)

        #Assert
        self.assertIsNotNone(read_queue)
        self.assertIsNotNone(read_queue._memory_block)
        self.assertEqual(mock_read_block.call_count, 4)
    
    @patch("emessgee.memory_queue.ReadMemoryBlock")
    def test_waitForWriting_waitsForWritingFlagToBeTrue(self, mock_read_block):
        #Assemble
        topic = "random_topic_name"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        open(topic_filepath, "wb").close()
        mock_memory_instance = MagicMock()
        
        try_index = -1
        return_values = [1, 1, 1, 0]
        def mock_read(index):
            nonlocal try_index
            if(index == ReservedIndexes.WRITING.value):
                try_index += 1
                print(try_index)
                return chr(return_values[try_index])
            else:
                return chr(7)
            
        mock_memory_instance.read.side_effect = mock_read
        mock_read_block.return_value = mock_memory_instance 
        read_queue = ReadMemoryQueue(topic)

        #Act
        read_queue._wait_for_writing()

        #Assert
        self.assertGreater(mock_memory_instance.read.call_count, len(return_values))
    
    def test_read_sharedMemoryFileDoesNotExist_returnsNone(self):
        #Assemble
        topic = "test_topic"
        read_memory_queue = ReadMemoryQueue(topic)

        #Act
        result = read_memory_queue.read()

        #Assert
        self.assertIsNone(result)
        read_memory_queue.close()
    
    def test_read_sharedMemoryFileCreatedAfterConstructor_createsBufferOnRecv(self):
        #Assemble
        topic = "test_topic"
        read_memory_queue = ReadMemoryQueue(topic)
        before_memory_block = read_memory_queue._memory_block
        write_memory_queue = WriteMemoryQueue(topic)

        #Act
        result = read_memory_queue.read()

        #Assert
        self.assertIsNone(before_memory_block)
        self.assertIsNotNone(read_memory_queue._memory_block)
        self.assertIsNone(result)
        write_memory_queue.close()
        read_memory_queue.close()
    
    def test_read_dataIsPublished_correctlyParsesAndReturnsData(self):
        #Assemble
        topic = "test_topic"
        subscriber = ReadMemoryQueue(topic)
        data = b"test bytes ijrgbiejrvbierbv"
        write_memory_queue = WriteMemoryQueue(topic)
        write_memory_queue.write(data)

        #Act
        result = subscriber.read()

        #Assert
        self.assertEqual(result, data)
        write_memory_queue.close()
        subscriber.close()
    
    def test_read_dataIsPublished_readTwice_dataOnlyReturnedOnce(self):
        #Assemble
        topic = "test_topic"
        read_memory_queue = ReadMemoryQueue(topic)
        data = b"test bytes ijrgbiejrvbierbv"
        write_memory_queue = WriteMemoryQueue(topic)
        write_memory_queue.write(data)

        #Act
        result1 = read_memory_queue.read()
        result2 = read_memory_queue.read()

        #Assert
        self.assertEqual(result1, data)
        self.assertIsNone(result2)
        write_memory_queue.close()
        read_memory_queue.close()
    
    def test_read_queueSizeIs1_publisherSendsMultiple_subscriberOnlyReceivesLatestData(self):
        #Assemble
        topic = "test_topic"
        read_memory_queue = ReadMemoryQueue(topic)
        data1 = b"kwjhbcvirehbv"
        data2 = b"irvbeirjhvb"
        data3 = b"8042fu934"
        write_memory_queue = WriteMemoryQueue(topic, queue_size=1)
        write_memory_queue.write(data1)
        write_memory_queue.write(data2)
        write_memory_queue.write(data3)

        #Act
        result1 = read_memory_queue.read()
        result2 = read_memory_queue.read()
        result3 = read_memory_queue.read()

        #Assert
        self.assertEqual(result1, data3)
        self.assertEqual(result2, None)
        self.assertEqual(result3, None)
        write_memory_queue.close()
        read_memory_queue.close()
    
    def test_read_queueSizeIs5_publisherSendsOneMessage_subscriberReadsMultipleTimes_queueIndexOnlyIncrementedOnce(self):
        #Assemble
        topic = "test_topic"
        read_memory_queue = ReadMemoryQueue(topic)
        data1 = b"kwjhbcvirehbv"
        write_memory_queue = WriteMemoryQueue(topic, queue_size=5)
        write_memory_queue.write(data1)

        #Act
        result1 = read_memory_queue.read()
        result2 = read_memory_queue.read()
        result3 = read_memory_queue.read()
        result4 = read_memory_queue.read()

        #Assert
        self.assertEqual(result1, data1)
        self.assertEqual(result2, None)
        self.assertEqual(result3, None)
        self.assertEqual(result4, None)
        self.assertEqual(read_memory_queue._queue_index, 1)
        write_memory_queue.close()
        read_memory_queue.close()


    def test_read_publisherSendsMultiple_subscriberReadsFromQueue(self):
        #Assemble
        topic = "test_topic"
        read_memory_queue = ReadMemoryQueue(topic)
        data1 = b"kwjhbcvirehbv"
        data2 = b"irvbeirjhvb"
        data3 = b"8042fu934"
        write_memory_queue = WriteMemoryQueue(topic, queue_size=5)
        write_memory_queue.write(data1)
        write_memory_queue.write(data2)
        write_memory_queue.write(data3)

        #Act
        result1 = read_memory_queue.read()
        result2 = read_memory_queue.read()
        result3 = read_memory_queue.read()

        #Assert
        self.assertEqual(result1, data1)
        self.assertEqual(result2, data2)
        self.assertEqual(result3, data3)
        write_memory_queue.close()
        read_memory_queue.close()
    
    def test_readHeader_headerCorrectlyParsed(self):
        #Assemble
        topic = "test_topic"
        data = b"test bytes ijrgbiejrvbierbv"
        write_memory_queue = WriteMemoryQueue(topic)
        write_memory_queue.write(data)
        read_memory_queue = ReadMemoryQueue(topic)

        #Act
        index, data_size, message_id = read_memory_queue._read_header(0)

        #Assert
        self.assertIsNotNone(index)
        self.assertEqual(data_size, len(data))
        self.assertIsInstance(message_id, UUID)
        write_memory_queue.close()
        read_memory_queue.close()
    
    def test_readHeader_bufferIsNone_noneIsReturned(self):
        #Assemble
        topic = "test_topic"
        read_memory_queue = ReadMemoryQueue(topic)

        #Act
        result = read_memory_queue._read_header(0)

        #Assert
        self.assertIsNone(result)
        read_memory_queue.close()
    
    def test_createReadMemoryQueueBeforeWriteMemoryQueue_nothingHappens(self):
        #Assemble
        topic = "test_topic"

        #Act
        subscriber = ReadMemoryQueue(topic)
        publisher = WriteMemoryQueue(topic)

        #Assert
        publisher.close()
        subscriber.close()
    
    @patch("emessgee.memory_queue.ReadMemoryBlock")
    def test_close_calledTwice_nothingHappens(self, mock_read_block):
        #Assemble
        topic = "random_topic_name"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        open(topic_filepath, "wb").close()
        mock_read_block().read.return_value = chr(1)
        read_queue = ReadMemoryQueue(topic)

        #Act/Assert
        read_queue.close()
        read_queue.close()