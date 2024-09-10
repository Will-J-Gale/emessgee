# emessgee
Zero copy IPC publish/subscribe messaging with single publisher multiple subscribers using memory maped files.

## Setup
* cpp
    * `./build.sh`
    * With examples: `build.sh examples`
* python
    * python setup.py build_ext --inplace

## Examples
### Pyhon Single topic
```c++
#include <iostream>

#include <emessgee.h>

int main()
{
    std::string topic = "pub_sub_topic";
    emessgee::Subscriber subscriber(topic);
    emessgee::Publisher publisher(topic);

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

### Pyhon Single topic
```python
from emessgee import Publisher, Subscriber

topic = "publish_test"
send_data = b"some byte data"

pub = Publisher(topic)
pub.send(topic, send_data)

subscriber = Subscriber(topic)
recv_data = subscriber.recv(topic)

print(f"Sent:       {send_data.decode()}")
print(f"Received:   {recv_data.decode()}")
```

More examples can be found in `examples` folder

## Nomenclature 
### Memory Block
A memory mapped file where arbitrary bytes can be read/written

### Memory Queue
A wrapper around a memory block to handle read/writing data with auto incrementing indexes + extra metadata and data headers

* Metadata
    * `sizeof(Metadata)` bytes at the beginning of the data buffer that contains metadata for read/write queues
* Header
    * Allocated bytes after the metadata = `sizeof(MessageHeader) * queue_size` that contains location, size and id of each message
    * e.g.  
    ```
    [metadata, header1, header2, header3, data1_bytes, data2_bytes, data3_bytes]
    ```