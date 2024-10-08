import sys
from threading import Thread

import numpy as np

try:
    from emessgee import Publisher, Subscriber
except ImportError:
    #Running from source folder
    import os
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from emessgee import Publisher, Subscriber

topic = "image"
running = True
print_freq = 20

def pub_main():
    import time
    from random import choice
    global topic, runnning, print_freq

    images = [np.random.random((1080, 1920, 3)) for i in range(10)]
    images = [(image * 255).astype(np.uint8) for image in images]
    image_size_bytes = sys.getsizeof(images[0])
    queue_size = 5
    buffer_size = image_size_bytes * queue_size
    pub = Publisher([topic], buffer_size, queue_size)
    index = 0

    while(running):
        try:
            image_bytes = choice(images).tobytes()
            start = time.time()
            pub.send(topic, image_bytes)
            write_dt = time.time() - start

            if(index % print_freq == 0):
                print(f"Write: {write_dt:.6f}s")

            index += 1
            time.sleep(1/100)
        except KeyboardInterrupt:
            break

def sub_main(process_id):
    import time
    global topic, running, print_freq
    sub = Subscriber([topic])
    index = 0

    while(running):
        start = time.time()
        recv_result = sub.recv(topic)

        if(recv_result.valid):
            image = recv_result.data.reshape((1080, 1920, 3))
            dt = time.time() - start
            
            if(index % print_freq == 0):
                print(f"Read {process_id}: {dt:.6f}s")

            index += 1

if __name__ == "__main__":
    import time
    num_subscribers = 3
    sub_threads = []
    pub_thread = Thread(target=pub_main)
    pub_thread.start()

    for i in range(num_subscribers):
        sub_thread = Thread(target=sub_main, args=(i,))
        sub_thread.start()
        sub_threads.append(sub_thread)

    while(True):
        try:
            time.sleep(0.1)
        except:
            break
    
    running = False
    pub_thread.join()
    [s.join() for s in sub_threads]
    print("Exiting")