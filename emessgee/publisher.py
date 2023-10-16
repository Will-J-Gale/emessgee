import atexit
from typing import Union

from emessgee.memory_queue import WriteMemoryQueue
from emessgee.exceptions import TopicDoesNotExistError, ErrorMessages
from emessgee.constants import DEFAULT_BUFFER_SIZE

class Publisher:
    def __init__(
            self, 
            topic:str, 
            buffer_size:int = DEFAULT_BUFFER_SIZE,
            queue_size:int = 1):
        self._topic_queues = {
            topic: WriteMemoryQueue(topic, buffer_size, queue_size)
        }    
        atexit.register(self.close)

    def send(self, topic, data:Union[bytes, str]):
        if(topic not in self._topic_queues):
            raise TopicDoesNotExistError(
                ErrorMessages.TOPIC_DOES_NOT_EXIST.format(topics=self._topic_queues.keys())
            )

        self._topic_queues[topic].write(data)

    def close(self):
        for topic_queue in self._topic_queues.values():
            topic_queue.close()