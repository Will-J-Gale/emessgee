import atexit
from typing import Union, List

from emessgee.memory_queue import ReadMemoryQueue
from emessgee.exceptions import TopicDoesNotExistError, ErrorMessages

class Subscriber:
    def __init__(self, topics:Union[str, List[str]]):
        self._topics = topics if isinstance(topics, list) else [topics]
        self._topic_queues = {
            topic: ReadMemoryQueue(topic)
            for topic in self._topics
        }
        self._queue_index = 0
         
        atexit.register(self.close)
    
    def recv(self, topic):
        if(topic not in self._topic_queues.keys()):
            raise TopicDoesNotExistError(
                ErrorMessages.TOPIC_DOES_NOT_EXIST.format(topics=self._topic_queues.keys())
            ) 
        
        return self._topic_queues[topic].read()

    def close(self):
        for topic_queue in self._topic_queues.values():
            topic_queue.close()