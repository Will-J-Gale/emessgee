import os
from uuid import UUID
from glob import glob
from unittest import TestCase
from unittest.mock import patch

from emessgee import Publisher, Subscriber
from emessgee.constants import TMP_FOLDER
from emessgee.exceptions import TopicDoesNotExistError

class TestSubscriber(TestCase):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{TMP_FOLDER}/*")]

    def test_constructor_createsMemoryQueueForTopic(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)

        #Act
        subscriber = Subscriber(topic)

        #Assert
        self.assertFalse(os.path.exists(topic_filepath))
        self.assertIn(topic, subscriber._topic_queues)
        subscriber.close()
    
    @patch("emessgee.subscriber.ReadMemoryQueue")
    def test_recv_topicNotInQueues_raisesError(self, _):
        #Assemble
        topic = "test_topic"
        subscriber = Subscriber(topic)

        #Act/Assert
        with self.assertRaises(TopicDoesNotExistError):
            subscriber.recv("invalid_topic")

        subscriber.close()