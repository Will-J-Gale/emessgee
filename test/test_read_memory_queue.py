import os
from glob import glob

from emessgee import ReadMemoryQueue, constants
from .base_test import BaseTest

class TestReadMemoryQueue(BaseTest):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{constants.TMP_FOLDER()}/*")]

    def test_constructor_successfullyCreatesReadMemoryQueueThatIsUninitialized(self):
        #Assemble
        topic = "test_topic"

        #Act
        read_queue = ReadMemoryQueue(topic)

        #Assert
        self.assertFalse(read_queue.is_initialized())
    
    def test_close_successfullyClosesQueue(self):
        #Assemble
        topic = "test_topic"
        read_queue = ReadMemoryQueue(topic)

        #Act/Assert
        read_queue.close()