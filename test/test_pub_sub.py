import os
from glob import glob

import numpy as np

from emessgee import Publisher, Subscriber, constants
from .base_test import BaseTest

class TestPubSub(BaseTest):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{constants.TMP_FOLDER()}/*")]

    def test_pubsub_successfullyPublishesAndSubscriberReceivesData(self):
        #Assemble
        topic = "test_topic"
        buffer_size = 1000
        queue_size = 2
        data = self.random_data(buffer_size-1)

        publisher = Publisher([topic], buffer_size, queue_size)
        subscriber = Subscriber([topic])

        #Act
        publisher.send(topic, data)
        result = subscriber.recv(topic)

        #Assert
        self.assertIsNotNone(result)
        self.assertEqual(result.data.tobytes(), data)
        publisher.close()
        subscriber.close()
    
    def test_pubsub_multipleTopicsForPublisher_2SeparateSubscribers_eachSubscriberGetsCorrectData(self):
        #Assemble
        topic_1 = "test_topic_1"
        topic_2 = "test_topic_2"
        buffer_size = 1000
        queue_size = 2
        data_1 = self.random_data(50)
        data_2 = self.random_data(75)

        publisher = Publisher([topic_1, topic_2], buffer_size, queue_size)
        subscriber_1 = Subscriber([topic_1])
        subscriber_2 = Subscriber([topic_2])

        #Act
        publisher.send(topic_1, data_1)
        publisher.send(topic_2, data_2)
        result_1 = subscriber_1.recv(topic_1)
        result_2 = subscriber_2.recv(topic_2)

        #Assert
        self.assertIsNotNone(result_1)
        self.assertIsNotNone(result_2)
        self.assertEqual(result_1.data.tobytes(), data_1)
        self.assertEqual(result_2.data.tobytes(), data_2)
        publisher.close()
        subscriber_1.close()
        subscriber_2.close()
    
    def test_pubsub_2Publishers_1Subcriber_subscriberGetsDataFromBothTopics(self):
        #Assemble
        topic_1 = "test_topic_1"
        topic_2 = "test_topic_2"
        buffer_size = 1000
        queue_size = 2
        data_1 = self.random_data(20)
        data_2 = self.random_data(45)

        publisher_1 = Publisher([topic_1], buffer_size, queue_size)
        publisher_2 = Publisher([topic_2], buffer_size, queue_size)
        subscriber = Subscriber([topic_1, topic_2])

        #Act
        publisher_1.send(topic_1, data_1)
        publisher_2.send(topic_2, data_2)
        result_1 = subscriber.recv(topic_1)
        result_2 = subscriber.recv(topic_2)

        #Assert
        self.assertIsNotNone(result_1)
        self.assertIsNotNone(result_2)
        self.assertEqual(result_1.data.tobytes(), data_1)
        self.assertEqual(result_2.data.tobytes(), data_2)
        publisher_1.close()
        publisher_2.close()
        subscriber.close()
    
    def test_pubsub_publishOnDifferentTopic_subscriberReceivesNothing(self):
        #Assemble
        topic_1 = "test_topic_1"
        topic_2 = "test_topic_2"
        buffer_size = 1000
        queue_size = 2
        data = self.random_data(20)

        publisher = Publisher([topic_1], buffer_size, queue_size)
        subscriber = Subscriber([topic_1, topic_2])

        #Act
        publisher.send(topic_2, data)
        result = subscriber.recv(topic_1)

        # #Assert
        self.assertIsNotNone(result)
        self.assertFalse(result.valid)
        self.assertIsNone(result.data)
        self.assertEqual(result.size, 0)
        publisher.close()
        subscriber.close()
    
    def test_pubsub_1Publisher_2Subscribers_largeImageData_bothSubscribersGetImage(self):
        #Assemble
        topic = "test_topic_1"
        image = self.create_random_image()
        image_bytes = image.tobytes()

        buffer_size = image.data.nbytes + 10
        queue_size = 2

        publisher = Publisher([topic], buffer_size, queue_size)
        subscriber_1 = Subscriber([topic])
        subscriber_2 = Subscriber([topic])

        #Act
        publisher.send(topic, image.tobytes())
        result_1 = subscriber_1.recv(topic)
        result_2 = subscriber_2.recv(topic)

        #Assert
        self.assertIsNotNone(result_1)
        self.assertIsNotNone(result_2)
        self.assertEqual(result_1.data.tobytes(), image_bytes)
        self.assertEqual(result_2.data.tobytes(), image_bytes)
        
        publisher.close()
        subscriber_1.close()
        subscriber_2.close()
    
    def test_pubsub_1Publisher_2Subscribers_largeImageData_multipleWrites_bothSubscribersGetAllImages(self):
        #Assemble
        num_images = 4
        image_list = [self.create_random_image() for _ in range(num_images)]
        topic = "test_topic_1"
        buffer_size = (image_list[0].data.nbytes * num_images) + 1000 #Make buffer big enough for 4 images
        queue_size = 6

        publisher = Publisher([topic], buffer_size, queue_size)
        subscriber_1 = Subscriber([topic])
        subscriber_2 = Subscriber([topic])

        #Act
        for image in image_list:
            publisher.send(topic, image.data.tobytes())

        for image in image_list:
            result_1 = subscriber_1.recv(topic)
            result_2 = subscriber_2.recv(topic)

            #Assert
            self.assertIsNotNone(result_1)
            self.assertIsNotNone(result_2)
            self.assertEqual(result_1.data.tobytes(), image.data.tobytes())
            self.assertEqual(result_2.data.tobytes(), image.data.tobytes())
        
        publisher.close()
        subscriber_1.close()
        subscriber_2.close()