import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from emessgee import Params


#Act


def main():
    key = "key_1"
    params = Params()
    print("Reading!")
    read_value = params.read_int(key)

    print(read_value)

if __name__ == "__main__":
    main()