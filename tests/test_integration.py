import os
import string
import random
from glob import glob
from unittest import TestCase

from emessgee import Publisher, Subscriber
from emessgee.constants import TMP_FOLDER

class TestSubscriber(TestCase):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{TMP_FOLDER}/*")]

    def random_bytes(self, size:int):
        letters = string.ascii_lowercase
        return ''.join(random.choice(letters) for i in range(size))

    def test_integration_publisherSUccessfullySendsToSubscriber(self):
         #Assemble
        topic = "test_topic"
        send_data = b"some random bytes"
        publisher = Publisher(topic)
        subscriber = Subscriber(topic)
        publisher.send(topic, send_data)

        #Act
        received_data = subscriber.recv(topic)

        #Assert
        self.assertIsNotNone(received_data)
        self.assertEqual(received_data, send_data)
        publisher.close()
        subscriber.close()
    
    def test_integration_publisherSendsToQueueSubscriberReadsFromQueue(self):
         #Assemble
        queue_size = 5
        num_messages = 10
        topic = "test_topic"
        sent_messages = [self.random_bytes(10).encode() for _ in range(num_messages)]
        publisher = Publisher(topic, queue_size=queue_size)
        subscriber = Subscriber(topic)
        received_messages = []

        #Act
        for i in range(num_messages):
            publisher.send(topic, sent_messages[i])
        
        for i in range(num_messages):
            data = subscriber.recv(topic)
            received_messages.append(data)

        #Assert
        self.assertEqual(received_messages[0], sent_messages[5])
        self.assertEqual(received_messages[1], sent_messages[6])
        self.assertEqual(received_messages[2], sent_messages[7])
        self.assertEqual(received_messages[3], sent_messages[8])
        self.assertEqual(received_messages[4], sent_messages[9])

        for message in received_messages[5:]:
            self.assertIsNone(message)

        publisher.close()
        subscriber.close()
    
    def test_integration_2PublishersWithDifferentTopics_1SubscriberWith2Topics_dataCorrectlySent(self):
         #Assemble
        topic1 = "test_topic_1"
        topic2 = "test_topic_2"
        send_data_topic_1 = b"some random bytes - 1"
        send_data_topic_2 = b"some more bytes - 2"
        publisher1 = Publisher(topic1)
        publisher2 = Publisher(topic2)
        subscriber = Subscriber([topic1, topic2])

        publisher1.send(topic1, send_data_topic_1)
        publisher2.send(topic2, send_data_topic_2)

        #Act
        received_data1 = subscriber.recv(topic1)
        received_data2 = subscriber.recv(topic2)

        #Assert
        self.assertEqual(received_data1, send_data_topic_1)
        self.assertEqual(received_data2, send_data_topic_2)
        publisher1.close()
        publisher2.close()
        subscriber.close()
    
    def test_integration_1PublisherWith2Topics_2Subscribers_correctlySendsData(self):
         #Assemble
        topic1 = "test_topic_1"
        topic2 = "test_topic_2"
        send_data_topic_1 = b"some random bytes - 1"
        send_data_topic_2 = b"some more bytes - 2"
        publisher = Publisher([topic1, topic2])
        subscriber1 = Subscriber(topic1)
        subscriber2 = Subscriber(topic2)

        publisher.send(topic1, send_data_topic_1)
        publisher.send(topic2, send_data_topic_2)

        #Act
        received_data1 = subscriber1.recv(topic1)
        received_data2 = subscriber2.recv(topic2)

        #Assert
        self.assertEqual(received_data1, send_data_topic_1)
        self.assertEqual(received_data2, send_data_topic_2)
        publisher.close()
        subscriber1.close()
        subscriber2.close()