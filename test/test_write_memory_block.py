import os
from glob import glob

from emessgee import constants, WriteMemoryBlock, error_messages, BufferWriteCode
from .base_test import BaseTest

class TestWriteMemoryBlock(BaseTest):
    def tearDown(self):
        [os.remove(file) for file in glob(f"{constants.TMP_FOLDER()}/*")]

    def test_constructor_successfullyCreatesMmapFile(self):
        #Assemble
        size = 10
        topic = "test_topic"

        #Act
        memory_block = WriteMemoryBlock(topic, size) 

        #Assert
        self.assertIsNotNone(memory_block)
        write_block_file_path = os.path.join(constants.TMP_FOLDER(), topic)
        self.assertEqual(os.path.getsize(write_block_file_path), size)

        memory_block.destroy()

    def test_constructor_fileAlreadyExists_raisesPublisherAlreadyExistsError(self):
        #Assemble
        size = 10
        topic = "test_topic"
        topic_filepath = os.path.join(constants.TMP_FOLDER(), topic)
        open(topic_filepath, "wb").close()

        #Act/Assert
        try:
            WriteMemoryBlock(topic, size) 
        except RuntimeError as e:
            self.assertEqual(str(e), error_messages.FILE_ALREADY_EXISTS())

    def test_write_successfullyWritesDataToMMapFile(self):
        #Assemble
        topic = "test_topic"
        data = b"hello there"
        memory_block = WriteMemoryBlock(topic, 50) 

        #Act
        memory_block.write(0, data)
        
        #Assert
        topic_file_path = os.path.join(constants.TMP_FOLDER(), topic)
        with open(topic_file_path, "rb") as file:
            self.assertIn(data, file.read())
    
    def test_writeByte_dataIsBiggerThanBufferSize_errorRaised(self):
        #Assemble
        topic = "test_topic"
        index = 0
        data = b"12345678910111213"
        buffer_size = 10
        memory_block = WriteMemoryBlock(topic, buffer_size) 

        #Act/Assert
        result = memory_block.write(index, data)
        self.assertEqual(result, BufferWriteCode.DATA_TOO_LARGE)
    
    def test_writeByte_indexIsGreaterThanBufferSize_returnsIndexTooLarge(self):
        #Assemble
        topic = "test_topic"
        buffer_size = 10
        index = buffer_size + 10
        data = b"12345678910111213"
        memory_block = WriteMemoryBlock(topic, buffer_size) 

        #Act/Assert
        result = memory_block.write(index, data)
        self.assertEqual(result, BufferWriteCode.INDEX_TOO_LARGE)
        
    
    def test_destroy_successfullyClosesBufferAndRemovesFile(self):
        #Assemble
        topic = "test_topic"
        memory_block = WriteMemoryBlock(topic, 100) 
        topic_file_path = os.path.join(constants.TMP_FOLDER(), topic)
        exists_before_close = os.path.exists(topic_file_path)

        #Act
        memory_block.destroy()
        
        #Assert
        exists_after_close = os.path.exists(topic_file_path)
        self.assertTrue(exists_before_close)
        self.assertFalse(exists_after_close)
    
    def test_close_calledTwice_nothingHappensSecondTime(self):
        #Assemble
        topic = "test_topic"
        memory_block = WriteMemoryBlock(topic, 100) 

        #Act/Assert
        memory_block.destroy()
        memory_block.destroy()