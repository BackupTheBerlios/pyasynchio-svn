class Echo:
    class SH:
        def __init__(self, pro):
            self.pro = pro
            self.amount = 102400

        def notify_initialized(self):
            self.pro.open_stream_read(self, self.amount)

        def read_completed(self, read_result):
            if (not read_result.success_) or (read_result.data_ == 'shutdown'):
                self.pro.close_active_stream(self)
                return

            if len(read_result.data_) > 0:
                self.pro.open_stream_write(self, read_result.data_)
                self.pro.open_stream_read(self, self.amount)

    def __init__(self, pro):
        self.pro = pro

    def close(self):
        self.pro.close_stream_accept(self)

    def make_stream_handler(self):
        return self.SH(self.pro)

    def notify_initialized(self):
        self.pro.open_stream_read(self, self.amount)
