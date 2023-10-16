import random
import string

try:
    from emessgee import Publisher, Subscriber
except ImportError:
    #Running from source folder
    import os
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from emessgee import Publisher, Subscriber

def random_bytes(size:int):
   letters = string.ascii_lowercase
   return ''.join(random.choice(letters) for i in range(size))

topic = "publish_test"
queue_size = 5
num_messages = 10
print(f"Queue size: {queue_size}")

pub = Publisher(topic, queue_size=queue_size)
for i in range(num_messages):
    send_data = f"{i}_{random_bytes(5)}".encode()
    pub.send(send_data)
    print(f"Sent:       {send_data.decode()}")

print("-" * 50)

subscriber = Subscriber(topic)
for i in range(num_messages):
    recv_data = subscriber.recv()
    if(recv_data is not None):
        print(f"Received:   {recv_data.decode()}")
    else:
        print(f"Received:   None")