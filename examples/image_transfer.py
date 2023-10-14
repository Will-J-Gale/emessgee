import sys
from threading import Thread

import numpy as np
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
    buffer_size = image_size_bytes * 5
    pub = Publisher(topic, buffer_size)
    index = 0

    while(running):
        try:
            image_bytes = choice(images).tobytes()
            start = time.time()
            pub.send(image_bytes)
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
    sub = Subscriber(topic)
    index = 0

    while(running):
        start = time.time()
        image_bytes = sub.recv()

        if(image_bytes is not None):
            image = np.frombuffer(image_bytes, np.uint8)
            image = image.reshape((1080, 1920, 3))
            dt = time.time() - start
            
            if(index % print_freq == 0):
                print(f"Read {process_id}: {dt:.6f}s")

            index += 1

if __name__ == "__main__":
    import time
    num_subscribers = 5
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