import os
from glob import glob
from unittest import TestCase

from emessgee.constants import TMP_FOLDER
from emessgee.memory_block import MemoryBlock
from emessgee.exceptions import PublisherAlreadyExistsError, MemoryBlockIsReadOnlyError

class TestMemoryBlock(TestCase):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{TMP_FOLDER}/*")]

    def test_constructor_successfullyCreatesMmapFile(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)

        #Act
        memory_queue = MemoryBlock(topic, create=True) 

        #Assert
        self.assertIsNotNone(memory_queue)
        memory_queue.close()

    def test_constructor_fileAlreadyExists_raisesPublisherAlreadyExistsError(self):
        #Assemble
        topic = "test_topic"
        topic_filepath = os.path.join(TMP_FOLDER, topic)
        open(topic_filepath, "wb").close()

        #Act/Assert
        with self.assertRaises(PublisherAlreadyExistsError):
            MemoryBlock(topic, create=True) 

    def test_constructor_createIsFalse_fileDoesNotExist_raisesFileNotFoundError(self):
        #Assemble
        topic = "test_topic"

        #Act/Assert
        with self.assertRaises(FileNotFoundError):
            MemoryBlock(topic, create=False) 
        
    def test_write_successfullyWritesDataToMMapFile(self):
        #Assemble
        topic = "test_topic"
        data = b"hello there"
        memory_queue = MemoryBlock(topic, create=True) 

        #Act
        memory_queue.write(0, data)
        
        #Assert
        with open(memory_queue._filepath, "rb") as file:
            self.assertIn(data, file.read())
    
    def test_write_successfullyWritesDataToMMapFile(self):
        #Assemble
        topic = "test_topic"
        data = b"hello there"
        write_queue = MemoryBlock(topic, create=True) 
        read_queue = MemoryBlock(topic) 

        #Act/Assert
        with self.assertRaises(MemoryBlockIsReadOnlyError):
            read_queue.write(0, data)

    def test_writeFlag_successfullyWritesFlag(self):
        #Assemble
        topic = "test_topic"
        index = 100
        state = True
        memory_queue = MemoryBlock(topic, create=True) 

        #Act
        memory_queue.write_flag(index, state)
        
        #Assert
        with open(memory_queue._filepath, "rb") as file:
            written_data = file.read()
            self.assertEqual(written_data[index], state)
    
    def test_read_successfullyReturnsData(self):
        #Assemble
        topic = "test_topic"
        data = b"hello there"
        index = 99
        memory_queue = MemoryBlock(topic, create=True) 
        memory_queue.write(index, data)

        #Act
        read_data = memory_queue.read(index, len(data))
        
        #Assert
        self.assertEqual(read_data, data)
    
    def test_close_successfullyClosesBufferAndRemovesFile(self):
        #Assemble
        topic = "test_topic"
        memory_queue = MemoryBlock(topic, create=True) 
        exists_before_close = os.path.exists(memory_queue._filepath)

        #Act
        memory_queue.close()
        
        #Assert
        exists_after_close = os.path.exists(memory_queue._filepath)
        self.assertTrue(exists_before_close)
        self.assertFalse(exists_after_close)
    
    def test_close_calledTwice_nothingHappensSecondTime(self):
        #Assemble
        topic = "test_topic"
        memory_queue = MemoryBlock(topic, create=True) 

        #Act/Assert
        memory_queue.close()
        memory_queue.close()