import os
from glob import glob

from emessgee import Subscriber, constants
from .base_test import BaseTest

class TestPubSub(BaseTest):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{constants.TMP_FOLDER()}/*")]

    def test_subscriber_recv_noDataPublishedOnTopic_validIsFalse(self):
        #Assemble
        topic = "test_topic_1"
        subscriber = Subscriber([topic])

        #Act
        result = subscriber.recv(topic)

        # #Assert
        self.assertIsNotNone(result)
        self.assertFalse(result.valid)
        self.assertIsNone(result.data)
        self.assertEqual(result.size, 0)
        subscriber.close()