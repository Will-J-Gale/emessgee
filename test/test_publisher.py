import os
from glob import glob

from emessgee import Publisher, BufferWriteCode, constants, error_messages
from .base_test import BaseTest

class TestPublisher(BaseTest):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{constants.TMP_FOLDER()}/*")]

    def test_publisher_constructor_createsFileForMemoryMapping(self):
        #Assemble
        topic = "test_topic"
        buffer_size = 100
        queue_size = 3
        topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)

        #Act
        publisher = Publisher([topic], buffer_size, queue_size)

        #Assert
        self.assertTrue(os.path.exists(topic_filepath))
        self.assertGreater(os.path.getsize(topic_filepath), buffer_size)
        publisher.close()
    
    def test_publisher_constructor_multipleTopics_createsFilesForAllTopics(self):
        #Assemble
        topics = [
            "topic_1",
            "topic_2",
            "topic_3",
        ]
        buffer_size = 100
        queue_size = 3

        #Act
        publisher = Publisher(topics, buffer_size, queue_size)

        #Assert
        for topic in topics:
            topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)
            self.assertTrue(os.path.exists(topic_filepath))
            self.assertGreater(os.path.getsize(topic_filepath), buffer_size)

        publisher.close()
    
    def test_publisher_constructor_create2PublishersWithSameTopic_runtimeErrorRaised(self):
        #Assemble
        topic = "test_topic_138434"
        buffer_size = 100
        queue_size = 3

        #Act
        with self.assertRaises(RuntimeError) as context:
            publisher1 = Publisher([topic], buffer_size, queue_size)
            publisher2 = Publisher([topic], buffer_size, queue_size)

        #Assert
        self.assertEqual(str(context.exception), error_messages.FILE_ALREADY_EXISTS())
    
    def test_publisher_close_multipleTopics_closesAllFilesForTopics(self):
        #Assemble
        topics = [
            "topic_1",
            "topic_2",
            "topic_3",
        ]
        buffer_size = 100
        queue_size = 3
        publisher = Publisher(topics, buffer_size, queue_size)

        #Act
        publisher.close()

        #Assert
        for topic in topics:
            topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)
            self.assertFalse(os.path.exists(topic_filepath))
    
    def test_publisher_close_callMultipleTimes_nothingHappens(self):
        #Assemble
        topics = [
            "topic_1",
            "topic_2",
            "topic_3",
        ]
        buffer_size = 100
        queue_size = 3
        publisher = Publisher(topics, buffer_size, queue_size)

        #Act
        publisher.close()
        publisher.close()
        publisher.close()

        #Assert
        for topic in topics:
            topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)
            self.assertFalse(os.path.exists(topic_filepath))
    
    def test_publisher_write_returnsSuccessCode(self):
        #Assemble
        topic = "test_topic"
        buffer_size = 100
        queue_size = 3
        topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)
        publisher = Publisher([topic], buffer_size, queue_size)

        #Act
        write_code = publisher.send(topic, b"2983jhwri")

        #Assert
        self.assertEqual(write_code, BufferWriteCode.SUCCESS)
        publisher.close()
    
    def test_publisher_write_topicDoesNotExist_returnsTopicDoesNotExistError(self):
        #Assemble
        topic = "test_topic"
        buffer_size = 100
        queue_size = 3
        topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)
        publisher = Publisher([topic], buffer_size, queue_size)

        #Act
        write_code = publisher.send("invalid_topic", b"2983jhwri")

        #Assert
        self.assertEqual(write_code, BufferWriteCode.TOPIC_DOES_NOT_EXIST)
        publisher.close()