import os
import shutil

from emessgee.constants import TMP_FOLDER

def pytest_configure():
    if(not os.path.exists(TMP_FOLDER)):
        os.makedirs(TMP_FOLDER, exist_ok=True)

def pytest_unconfigure():
    if(os.path.exists(TMP_FOLDER)):
        shutil.rmtree(TMP_FOLDER)