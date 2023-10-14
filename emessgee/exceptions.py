class ErrorMessages:
    DATA_NOT_BYTES = "Data should be bytes not {type}"
    DATA_TOO_LARGE = "Data is {data_size}, but should be smaller than {buffer_size}"
    PUBLISHER_ALREADY_EXISTS = "Publisher with topic {topic} already exists. If this is the only publisher, please delete /tmp/{topic}"
    INFINITE_LOOP = "Loop sanity check failed. Possible infiite loop detected"

class DataNotBytesOrStringError(Exception):
    pass

class DataTooLargeError(Exception):
    pass

class PublisherAlreadyExistsError(Exception):
    pass

class InfiniteLoopDetectedError(Exception):
    pass