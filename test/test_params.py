import os
import struct

from emessgee import Params, BufferWriteCode
from .base_test import BaseTest

class TestParams(BaseTest):
    def test_params_successfulylWriteAndReadParameter(self):
        #Assemble
        key = "parameter"
        value = 1234
        params = Params()

        #Act
        write_result = params.write_int(key, value)
        read_result = params.read_int(key)

        #Assert
        self.assertEqual(write_result, BufferWriteCode.SUCCESS)
        self.assertEqual(read_result, value)

    def test_params_multipleParameters_successfullyWritesAndReads(self):
        #Assemble
        key_1 = "key_1"
        key_2 = "key_2"
        key_3 = "key_3"
        key_4 = "key_4"
        key_5 = "key_5"

        value_1 = 1234
        value_2 = True
        value_3 = 1.9
        value_4 = "hello there"

        params = Params()

        #Act
        write_result_1 = params.write_int(key_1, value_1)
        write_result_2 = params.write_bool(key_2, value_2)
        write_result_3 = params.write_double(key_3, value_3)
        write_result_4 = params.write_string(key_4, value_4)
        write_result_5 = params.write_int(key_5, 9999)

        read_result_1 = params.read_int(key_1)
        read_result_2 = params.read_bool(key_2)
        read_result_3 = params.read_double(key_3)
        read_result_4 = params.read_string(key_4)
        read_result_5 = params.read_int(key_5)
        

        #Assert
        self.assertEqual(write_result_1, BufferWriteCode.SUCCESS)
        self.assertEqual(write_result_2, BufferWriteCode.SUCCESS)
        self.assertEqual(write_result_3, BufferWriteCode.SUCCESS)
        self.assertEqual(write_result_4, BufferWriteCode.SUCCESS)
        self.assertEqual(write_result_5, BufferWriteCode.SUCCESS)
        self.assertEqual(read_result_1, value_1)
        self.assertEqual(read_result_2, value_2)
        self.assertEqual(read_result_3, value_3)
        self.assertEqual(read_result_4, value_4)
        self.assertEqual(read_result_5, 9999)
    
    def test_params_readBeforeParamExists(self):
        #Assemble
        key = "parameter_that_does_not_exist"
        params = Params()

        #Act/Assert
        with self.assertRaises(RuntimeError):
            params.read_int(key)
        
    def test_params_multipleParams_counterGetsIncrementsAndDecrementedAndFilesAreDestroyedAtEnd(self):
        #Assemble
        params_1 = Params()
        params_2 = Params()
        params_3 = Params()
        params_4 = Params()

        #Act
        result = params_1.read_int("num_params_instances")
        params_1.close()
        params_2.close()
        params_3.close()
        params_4.close()

        #Assert
        self.assertEqual(result, 4)
        self.assertEqual(len(os.listdir("/tmp/emessgee/params")), 0)
    
    def test_params_successfullyReadAndWriteBytes(self):
        #Assemble
        key = "key_bytes"
        data = b"asdkonerkjverikvn"
        params = Params()

        #Act
        write_result = params.write_bytes(key, data)
        read_result = params.read_bytes(key, len(data))

        #Assert
        self.assertEqual(write_result, BufferWriteCode.SUCCESS)
        self.assertEqual(read_result, data)

        params.close()
    
    def test_params_successfullyReadAndWriteStructBytes(self):
        #Assemble
        key = "key_bytes"
        write_value_1 = 99
        write_value_2 = True
        write_value_3 = 3.14
        struct_format = "I?d"
        write_bytes = struct.pack(struct_format, write_value_1, write_value_2, write_value_3)
        params = Params()

        #Act
        write_result = params.write_bytes(key, write_bytes)
        read_bytes = params.read_bytes(key, len(write_bytes))
        read_value_1, read_value_2, read_value_3 = struct.unpack(struct_format, read_bytes)

        #Assert
        self.assertEqual(write_result, BufferWriteCode.SUCCESS)
        self.assertEqual(read_value_1, write_value_1)
        self.assertEqual(read_value_2, write_value_2)
        self.assertEqual(read_value_3, write_value_3)

        params.close()