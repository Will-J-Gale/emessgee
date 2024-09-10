import os
from glob import glob

from emessgee import WriteMemoryQueue, constants
from .base_test import BaseTest

class TestWriteMemoryQueue(BaseTest):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{constants.TMP_FOLDER()}/*")]

    def test_constructor_createsFileForMemoryMapping(self):
        #Assemble
        topic = "test_topic"
        buffer_size = 100
        queue_size = 3
        topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)

        #Act
        write_queue = WriteMemoryQueue(topic, buffer_size, queue_size)

        #Assert
        self.assertTrue(os.path.exists(topic_filepath))
        self.assertGreater(os.path.getsize(topic_filepath), buffer_size)
        write_queue.close()
    
    def test_close_successfullyClosesQueue(self):
        #Assemble
        queue_size = 10
        topic = "test_topic"
        topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)
        write_queue = WriteMemoryQueue(topic, 1000, queue_size)

        #Act/Assert
        write_queue.close()
        self.assertFalse(os.path.exists(topic_filepath))