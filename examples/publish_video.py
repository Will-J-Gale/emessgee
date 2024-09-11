import sys
import time
from threading import Thread
from argparse import ArgumentParser

import cv2
import numpy as np

try:
    from emessgee import Publisher, Subscriber
except ImportError:
    #Running from source folder
    import os
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from emessgee import Publisher, Subscriber

parser = ArgumentParser()
parser.add_argument("video_path")

TOPIC = "video"
RUNNING = True

def pub_main(video_path):
    global TOPIC, RUNNING

    video = cv2.VideoCapture(video_path)
    width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    num_frames = int(video.get(cv2.CAP_PROP_FRAME_COUNT))

    image_size_bytes = width * height * 3
    queue_size = 5
    buffer_size = image_size_bytes * queue_size
    pub = Publisher([TOPIC], buffer_size, queue_size)

    start = time.time()
    dts = []

    for _ in range(num_frames):
        try:
            if(not RUNNING):
                break

            valid, image = video.read()

            if(valid):
                pub.send(TOPIC, image.tobytes())

                current_time = time.time()
                dt = current_time - start
                start = current_time
                print(dt)
                dts.append(dt)
        except KeyboardInterrupt:
            break
    
    RUNNING = False
    pub.close()
    video.release()

    avg_dt = np.mean(dts) * 1000
    print(f"Avg pub dt: {avg_dt}ms")

def main(args):
    global TOPIC, RUNNING

    video = cv2.VideoCapture(args.video_path)
    width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    video.release()

    sub = Subscriber([TOPIC])
    pub_thread = Thread(target=pub_main, args=(args.video_path, ))
    pub_thread.start()

    start = time.time()
    dts = []

    try:
        while(RUNNING):
            result = sub.recv(TOPIC)
            if(result.valid):
                image = np.frombuffer(result.data, np.uint8).reshape((height, width, 3))

                current_time = time.time()
                dt = current_time - start
                start = current_time
                dts.append(dt)

                cv2.imshow("image", image)
                cv2.waitKey(1)

    except KeyboardInterrupt:
        pass

    sub.close()
    RUNNING = False
    pub_thread.join()
    avg_dt = np.mean(dts) * 1000
    print(f"Average sub dt: {avg_dt}ms")

if __name__ == "__main__":
    args = parser.parse_args()
    main(args)