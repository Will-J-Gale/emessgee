# emessgee
Zero copy IPC publish/subscribe messaging with single publisher multiple subscribers using memory maped files.

## Example
```python
from emessgee import Publisher, Subscriber

topic = "publish_test"
send_data = b"some byte data"

pub = Publisher(topic)
pub.send(send_data)

subscriber = Subscriber(topic)
recv_data = subscriber.recv()

print(f"Sent:       {send_data.decode()}")
print(f"Received:   {recv_data.decode()}")
```
