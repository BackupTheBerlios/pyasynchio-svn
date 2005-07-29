from pyasynchio import AbstractAcceptHandler, AbstractStreamHandler

class Echo(AbstractAcceptHandler):
    class SH(AbstractStreamHandler):
        def __init__(self, pro):
            AbstractStreamHandler.__init__(self)
            self.pro = pro
            self.amount = 102400

        def notify_endpoints(self, remote, local):
            pass

        def notify_initialized(self):
            self.pro.open_stream_read(self, self.amount)

        def read_completed(self, read_result):
            if (not read_result.success_) or (read_result.data_ == 'shutdown'):
                self.pro.close_active_stream(self)
                return

            if len(read_result.data_) > 0:
                self.pro.open_stream_write(self, read_result.data_)
                self.pro.open_stream_read(self, self.amount)

        def write_completed(self, write_result):
            pass
        

    def __init__(self, pro):
        AbstractAcceptHandler.__init__(self)
        self.pro = pro

    def close(self):
        self.pro.close_stream_accept(self)

    def conn_validate(self, result, remote, local):
        return True

    def conn_completed(self, result):
        pass

    def make_stream_handler(self):
        return self.SH(self.pro)

    def notify_endpoints(self, remote, local):
        pass

    def notify_initialized(self):
        self.pro.open_stream_read(self, self.amount)


