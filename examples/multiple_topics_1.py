try:
    from emessgee import Publisher, Subscriber
except ImportError:
    #Running from source folder
    import os
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from emessgee import Publisher, Subscriber

topic1 = "topic_1"
topic2 = "topic_2"
send_data1 = b"some byte data"
send_data2 = b"more bytes"

pub1 = Publisher(topic1)
pub2 = Publisher(topic2)

pub1.send(topic1, send_data1)
pub2.send(topic2, send_data2)

subscriber = Subscriber([topic1, topic2])
recv_data1 = subscriber.recv(topic1)
recv_data2 = subscriber.recv(topic2)

print(f"Pub 1 Sent:       {send_data1.decode()}")
print(f"Pub 2 Sent:       {send_data2.decode()}")
print(f"Sub Received topic 1:   {recv_data1.decode()}")
print(f"Sub Received topic 2:   {recv_data2.decode()}")