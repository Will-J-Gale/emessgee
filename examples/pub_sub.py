from emessgee import Publisher, Subscriber

topic = "publish_test"
send_data = b"some byte data"

pub = Publisher(topic)
pub.send(send_data)

subscriber = Subscriber(topic)
recv_data = subscriber.recv()

print(f"Sent:       {send_data.decode()}")
print(f"Received:   {recv_data.decode()}")