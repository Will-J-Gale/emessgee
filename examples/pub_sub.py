import random
import string

try:
    from emessgee import Publisher, Subscriber
except ImportError:
    #For when running from source folder
    import os
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from emessgee import Publisher, Subscriber

def random_bytes(size:int):
   letters = string.ascii_lowercase
   return ''.join(random.choice(letters) for i in range(size))

if __name__ == "__main__":
    topic = "publish_test"
    buffer_size = 100
    queue_size = 3
    send_data = random_bytes(10).encode()

    pub = Publisher([topic], buffer_size, queue_size)
    pub.send(topic, send_data)

    subscriber = Subscriber([topic])
    recv_result = subscriber.recv(topic)

    print(f"Sent:       {send_data}")
    print(f"Received:   {recv_result.data.tobytes()}")