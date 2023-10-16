import os
from glob import glob
from unittest import TestCase
from unittest.mock import patch

from emessgee import Publisher
from emessgee.exceptions import TopicDoesNotExistError
from emessgee.constants import TMP_FOLDER

class TestPublisher(TestCase):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{TMP_FOLDER}/*")]

    def test_constructor_createsMemoryQueue(self):
        #Assemble
        topic = "test_topic"

        #Act
        publisher = Publisher(topic)

        #Assert
        self.assertEqual(len(publisher._topic_queues), 1)
        self.assertIn(topic, publisher._topic_queues)
    
    def test_constructor_customBufferAndQueueSize_memoryQueueCreatedWithCorrectSize(self):
        #Assemble
        topic = "test_topic"
        buffer_size = 1000
        queue_size = 10

        #Act
        publisher = Publisher(topic, buffer_size, queue_size)

        #Assert
        self.assertEqual(len(publisher._topic_queues), 1)
        memory_queue = publisher._topic_queues[topic]
        
        self.assertEqual(memory_queue._buffer_size, buffer_size)
        self.assertEqual(memory_queue._queue_size, queue_size)
    
    @patch("emessgee.publisher.WriteMemoryQueue")
    def test_constructor_createsMemoryQueue(self, mock_write_queue):
        #Assemble
        topic = "test_topic"
        data = b"send data"
        publisher = Publisher(topic)

        #Act
        publisher.send(topic, data)

        #Assert
        self.assertTrue(mock_write_queue().write.called)
    
    @patch("emessgee.publisher.WriteMemoryQueue")
    def test_constructor_createsMemoryQueue(self, mock_write_queue):
        #Assemble
        topic = "test_topic"
        data = b"send data"
        publisher = Publisher(topic)

        #Act/Assert
        with self.assertRaises(TopicDoesNotExistError):
            publisher.send("invalid_topic", data)
        self.assertFalse(mock_write_queue().write.called)
