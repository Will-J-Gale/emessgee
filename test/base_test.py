from unittest import TestCase
from string import ascii_letters
from random import randint, choice

import numpy as np

class BaseTest(TestCase):
    def random_data(self, max_size=100):
        data = bytes()
        for _ in range(randint(1, max_size)):
            data += choice(ascii_letters).encode()
        
        return data
    
    def create_random_image(self, width=1200, height=720):
        image = np.random.random((720, 1280, 3)) * 255
        return image.astype(np.uint8)