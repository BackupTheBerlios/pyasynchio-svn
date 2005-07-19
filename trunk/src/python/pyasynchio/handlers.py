from _pyasynchio import AbstractAcceptHandler as _AbstractAcceptHandler
from _pyasynchio import AbstractConnectHandler as _AbstractConnectHandler
from _pyasynchio import AbstractStreamHandler as _AbstractStreamHandler
from detail import override_constructor

class AbstractAcceptHandler(_AbstractAcceptHandler):
    def __init__(self):
        _AbstractAcceptHandler.__init__(self)

    def conn_validate(self, result, remote, local):
        raise NotImplementedError

    def conn_completed(self, result):
        raise NotImplementedError

    def make_stream_handler(self):
        raise NotImplementedError

class AbstractConnectHandler(_AbstractConnectHandler):
    def __init__(self):
        _AbstractConnectHandler.__init__(self)

    def conn_validate(self, result, remote, local):
        raise NotImplementedError

    def conn_completed(self, result):
        raise NotImplementedError

    def make_stream_handler(self):
        raise NotImplementedError

class AbstractStreamHandler(_AbstractStreamHandler):
    def __init__(self): 
        _AbstractStreamHandler.__init__(self)

    def notify_initialized(self):
        raise NotImplementedError

    def notify_endpoints(self, remote, local):
        raise NotImplementedError

    def read_completed(self, read_result):
        raise NotImplementedError

    def write_completed(self, read_result):
        raise NotImplementedError