import os
from glob import glob

from emessgee import WriteMemoryQueue, ReadMemoryQueue, MessageHeader, BufferWriteCode, constants, error_messages
from .base_test import BaseTest

class TestMemoryQueue(BaseTest):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{constants.TMP_FOLDER()}/*")]

    def test_writeAndRead_successfullyWritesAndReadsQueue(self):
        #Assemble
        buffer_size = 1000
        queue_size = 10
        topic = "test_topic"
        write_queue = WriteMemoryQueue(topic, buffer_size, queue_size)
        read_queue = ReadMemoryQueue(topic)
        data = b"jwdhfoijwencvn"

        #Act
        write_queue.write(data)
        result = read_queue.read()

        self.assertTrue(result.valid)
        self.assertEqual(result.data.tobytes(), data)
        self.assertEqual(result.size, len(data))
        write_queue.close()
        read_queue.close()

    def test_multipleReadAndWrites_successfullyQueuesData(self):
        #Assemble
        buffer_size = 10000
        queue_size = 10
        topic = "test_topic"
        num_data = 7
        queue = [self.random_data() for _ in range(num_data)]
        write_queue = WriteMemoryQueue(topic, buffer_size, queue_size)
        read_queue = ReadMemoryQueue(topic)

        #Act
        for data in queue:
            write_queue.write(data)

        for i in range(num_data):
            result = read_queue.read()
            expected_data = queue[i]
            print(expected_data, result.data.tobytes())

            self.assertTrue(result.valid)
            self.assertEqual(result.data.tobytes(), expected_data)
            self.assertEqual(result.size, len(expected_data))
        
        read_queue.close()
        write_queue.close()
    
    def test_write_dataTooLarge_dataTooLargeErrorCodeReturned(self):
        #Assemble
        topic = "test_topic"
        write_queue = WriteMemoryQueue(topic, 80, 1)
        data = b"t" * 101

        #Act
        result = write_queue.write(data)

        self.assertEqual(result, BufferWriteCode.DATA_TOO_LARGE)
        write_queue.close()

    def test_write_writeIndexCorrectlyWrapsAround(self):
        #Assemble
        buffer_size = 5
        queue_size = 5
        topic = "test_topic"
        # topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)
        write_queue = WriteMemoryQueue(topic, buffer_size, queue_size)

        #Act
        for i in range(queue_size + 1):
            result = write_queue.write(b"a")
            self.assertEqual(result, BufferWriteCode.SUCCESS)

        #Assert
        write_queue.close()
    
    def test_close_calledTwice_nothingHappens(self):
        #Assemble
        topic = "test_topic"
        write_queue = WriteMemoryQueue(topic, 100, 10)

        #Act
        write_queue.close()
        write_queue.close()