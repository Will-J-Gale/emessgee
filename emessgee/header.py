from uuid import UUID
from struct import pack, unpack

from emessgee.constants import HEADER_FORMAT, HEADER_LENGTH, ID_BYTES_ENDIEN
from emessgee.exceptions import InvalidHeaderError

    
def header_to_bytes(data_index:int, data_size:int, message_id:UUID):
    return pack(HEADER_FORMAT, data_index, data_size, message_id.bytes)

def header_from_bytes(header_bytes):
    if(len(header_bytes) != HEADER_LENGTH):
        raise InvalidHeaderError
    
    data_index, data_size, id_bytes = unpack(HEADER_FORMAT, header_bytes)
    message_id = UUID(int=int.from_bytes(id_bytes, ID_BYTES_ENDIEN))
    return data_index, data_size, message_id
