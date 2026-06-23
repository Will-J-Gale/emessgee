import os
import sys
import time
from threading import Thread
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from emessgee import Params
PARAM_NAME = "test_param"
TERMINATE = "terminate"

def write_thread(num_loops=10):
    params = Params()

    for i in range(num_loops):
        params.write_int(PARAM_NAME, i)
        print(f"Wrote param: {i}")
        time.sleep(1)
    
    params.write_bool(TERMINATE, True)
    print("Write finished")

def main():
    params = Params()
    test = False
    params.write_bool(TERMINATE, test)
    params.write_int(PARAM_NAME, 0)

    t = Thread(target=write_thread, args=(5, ))
    t.start()

    term = False
    while(not term):
        term = params.read_bool(TERMINATE)
        int_param = params.read_int(PARAM_NAME)

        print(f"Read: {int_param}")
        time.sleep(0.2)
    
    t.join()

if __name__ == "__main__":
    main()