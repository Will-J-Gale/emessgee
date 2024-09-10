try:
    from emessgee import Publisher, Subscriber
except ImportError:
    #Running from source folder
    import os
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from emessgee import Publisher, Subscriber


from emessgee import Publisher, Subscriber

buffer_size = 100
queue_size = 3
topic1 = "topic_1decode"
topic2 = "topic_2"
send_data1 = b"some byte data"
send_data2 = b"more bytes"

pub = Publisher([topic1, topic2], buffer_size, queue_size)
pub.send(topic1, send_data1)
pub.send(topic2, send_data2)

subscriber1 = Subscriber([topic1])
subscriber2 = Subscriber([topic2])
recv_data1 = subscriber1.recv(topic1)
recv_data2 = subscriber2.recv(topic2)

print(f"Pub Sent topic 1:       {send_data1}")
print(f"Pub Sent topic 2:       {send_data2}")
print(f"Sub 1 Received:   {recv_data1.data.tobytes()}")
print(f"Sub 2 Received :   {recv_data2.data.tobytes()}")