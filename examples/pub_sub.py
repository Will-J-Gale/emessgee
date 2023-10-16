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

if __name__ == "__main__":
    topic = "publish_test"
    send_data = random_bytes(10).encode()

    pub = Publisher(topic)
    pub.send(topic, send_data)

    subscriber = Subscriber(topic)
    recv_data = subscriber.recv(topic)

    print(f"Sent:       {send_data.decode()}")
    print(f"Received:   {recv_data.decode()}")