import os
from glob import glob

from emessgee import WriteMemoryBlock, ReadMemoryBlock, error_messages, constants
from .base_test import BaseTest

class TestReadMemoryBlock(BaseTest):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{constants.TMP_FOLDER()}/*")]

    def test_constructor_successfullyCreatesMmapFileToRead(self):
        #Assemble
        topic = "test_topic"
        write_block = WriteMemoryBlock(topic, 100)

        #Act
        memory_block = ReadMemoryBlock(topic) 

        #Assert
        self.assertIsNotNone(memory_block)
        write_block.destroy()
        memory_block.destroy() 
    
    def test_constructor_fileDoesNotExist_isInitializedReturnsFalse(self):
        #Assemble
        topic = "test_topic"

        #Act
        memory_block = ReadMemoryBlock(topic) 

        self.assertFalse(memory_block.is_initialized())

    def test_read_successfullyReturnsData(self):
        #Assemble
        topic = "test_topic"
        data = b"hello there"
        index = 99

        write_block = WriteMemoryBlock(topic, 1000)
        write_block.write(index, data)
        memory_block = ReadMemoryBlock(topic) 

        #Act
        read_data = memory_block.read(index)
        
        #Assert
        self.assertEqual(read_data, data)
    
    def test_read_bufferNotInitialized_getsInitializedOnRead(self):
        #Assemble
        topic = "test_topic"
        data = b"hello there"
        index = 99

        memory_block = ReadMemoryBlock(topic) 
        write_block = WriteMemoryBlock(topic, 1000)
        write_block.write(index, data)
        before_read_state = memory_block.is_initialized()

        #Act
        read_data = memory_block.read(index)
        
        #Assert
        self.assertEqual(data, read_data)

        after_read_state = memory_block.is_initialized()
        self.assertFalse(before_read_state)
        self.assertTrue(after_read_state)

    def test_destroy_closesBufferButDoesNotDeleteFile(self):
        #Assemble
        topic = "test_topic"
        topic_path = os.path.join(constants.TMP_FOLDER(), topic)
        write_block = WriteMemoryBlock(topic, 100)
        memory_block = ReadMemoryBlock(topic) 

        #Act
        memory_block.destroy()

        #Assert
        self.assertTrue(os.path.exists(topic_path))
        write_block.destroy()
    
    