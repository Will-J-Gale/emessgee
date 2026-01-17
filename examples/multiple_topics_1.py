try:
    from emessgee import Publisher, Subscriber
except ImportError:
    #Running from source folder
    import os
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from emessgee import Publisher, Subscriber

buffer_size = 100
queue_size = 2
topic1 = "topic_1"
topic2 = "topic_2"
send_data1 = b"some byte data"
send_data2 = b"more bytes"

pub1 = Publisher([topic1], buffer_size, queue_size)
pub2 = Publisher([topic2], buffer_size, queue_size)

pub1.send(topic1, send_data1)
pub2.send(topic2, send_data2)

subscriber = Subscriber([topic1, topic2])
recv_data1 = subscriber.recv(topic1)
recv_data2 = subscriber.recv(topic2)

print(f"Pub 1 Sent:       {send_data1}")
print(f"Pub 2 Sent:       {send_data2}")
print(f"Sub Received topic 1:   {bytes(recv_data1.data)}")
print(f"Sub Received topic 2:   {bytes(recv_data2.data)}")