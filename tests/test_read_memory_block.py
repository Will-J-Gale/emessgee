import os
from glob import glob
from unittest import TestCase

from emessgee.constants import TMP_FOLDER
from emessgee.memory_block import ReadMemoryBlock, WriteMemoryBlock
from emessgee.exceptions import FailedCreatingMmapBufferError

class TestReadMemoryBlock(TestCase):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{TMP_FOLDER}/*")]

    def test_constructor_successfullyCreatesMmapFileToRead(self):
        #Assemble
        topic = "test_topic"
        write_block = WriteMemoryBlock(topic)

        #Act
        memory_block = ReadMemoryBlock(topic) 

        #Assert
        self.assertIsNotNone(memory_block)
        write_block.close()
        memory_block.close()
    
    def test_constructor_fileExistsButNotTrucatedYet_raisesError(self):
        #Assemble
        topic = "test_topic"
        open(os.path.join(TMP_FOLDER, topic), "wb").close()

        #Act/Assert
        with self.assertRaises(FailedCreatingMmapBufferError):
            ReadMemoryBlock(topic) 

    def test_constructor_fileDoesNotExist_raisesError(self):
        #Assemble
        topic = "test_topic"

        #Act/Assert
        with self.assertRaises(FileNotFoundError):
            ReadMemoryBlock(topic) 

    def test_read_successfullyReturnsData(self):
        #Assemble
        topic = "test_topic"
        data = b"hello there"
        index = 99

        write_block = WriteMemoryBlock(topic)
        write_block.write(index, data)
        memory_block = ReadMemoryBlock(topic) 

        #Act
        read_data = memory_block.read(index, len(data))
        
        #Assert
        self.assertEqual(read_data, data)
    
    def test_close_closesBufferButDoesNotDeleteFile(self):
        #Assemble
        topic = "test_topic"
        topic_path = os.path.join(TMP_FOLDER, topic)
        write_block = WriteMemoryBlock(topic)
        memory_block = ReadMemoryBlock(topic) 

        #Act
        memory_block.close()

        #Assert
        self.assertTrue(os.path.exists(topic_path))
        write_block.close()