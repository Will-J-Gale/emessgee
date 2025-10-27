# emessgee
Zero copy IPC publish/subscribe messaging with single publisher multiple subscribers using memory maped files.

## Setup
* cpp
    * `./build.sh`
    * With examples: `build.sh --build_examples`
* python
    * `./build.sh --build_python`

## Examples
### Python Single topic
```c++
#include <iostream>

#include <emessgee.h>

int main()
{
    std::string topic = "pub_sub_topic";
    size_t buffer_size = 1000;
    size_t queue_size = 2;
    emessgee::Publisher publisher(topic, buffer_size, queue_size);
    emessgee::Subscriber subscriber(topic);

    std::string data = "hello there";
    std::cout << "Published: " << data << std::endl;
    publisher.send(topic, (emessgee::byte*)data.c_str(), data.size());

    emessgee::ReadResult result = subscriber.recv(topic);

    if(result.valid)
    {
        std::string result_message((char*)result.data, result.size);
        std::cout << "Received: " << result_message << std::endl;
    }

    return 0;
}

```

### Python Single topic
```python
from emessgee import Publisher, Subscriber

buffer_size = 1000
queue_size = 2
topic = "publish_test"
send_data = b"some byte data"

pub = Publisher([topic], buffer_size, queue_size)
pub.send(topic, send_data)

subscriber = Subscriber([topic])
recv_result = subscriber.recv(topic)

print(f"Sent:       {send_data}")
print(f"Received:   {recv_result.data.tobytes()}")
```


More examples can be found in `examples` folder

### Running C++ examples
``` shell
./build/examples/pub_sub/pub_sub
./build/examples/pub_sub_threads/publish_sub_threads
./build/examples/multiple_topics_1/multiple_topics_1
./build/examples/multiple_topics_2/multiple_topics_2
./build/examples/publish_image/publish_image <path_to_image>
./build/examples/publish_video/publish_video <path_to_video>
```

### Running python
``` shell
python examples/pub_sub.py
python examples/multiple_topics_1.py
python examples/multiple_topics_2.py
python examples/queue_size.py
python examples/publish_image.py
python examples/publish_video.py <path_to_video>
```
