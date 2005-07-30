from _pyasynchio import Proactor as _Proactor
from more_handlers import WrapperAcceptHandler, WrapperConnectHandler
from more_handlers import WrapperStreamHandler, WrapperTimerHandler
from more_handlers import force_wrapper, find_wrapper, kill_wrapper

class Proactor(_Proactor):
    def __init__(self):
        _Proactor.__init__(self)

    def open_stream_accept(self, accept_handler, addr):
        wah = force_wrapper(WrapperAcceptHandler, accept_handler, self)
        _Proactor.open_stream_accept(self, wah, addr)

    def close_stream_accept(self, accept_handler):
        wah = find_wrapper(WrapperAcceptHandler, accept_handler, self)
        _Proactor.close_stream_accept(self, wah)
        kill_wrapper(WrapperAcceptHandler, accept_handler, self)

    def open_stream_connect(self, connect_handler, remote, local = ('',0)):
        wch = force_wrapper(WrapperConnectHandler, accept_handler, self)
        _Proactor.open_stream_connect(self, wch, remote, local)

    def close_stream_connect(self, connect_handler):
        wch = force_wrapper(WrapperConnectHandler, connect_handler, self)
        _Proactor.close_stream_connect(self, wch)
        kill_wrapper(WrapperConnectHandler, connect_handler, self)

    def open_stream_read(self, stream_handler, byte_count):
        wsh = find_wrapper(WrapperStreamHandler, stream_handler, self)
        _Proactor.open_stream_read(self, wsh, byte_count)

    def cancel_stream_read(self, stream_handler):
        wsh = find_wrapper(WrapperStreamHandler, stream_handler, self)
        _Proactor.cancel_stream_read(self, wsh)

    def open_stream_write(self, stream_handler, data):
        wsh = find_wrapper(WrapperStreamHandler, stream_handler, self)
        _Proactor.open_stream_write(self, wsh, data)

    def cancel_stream_write(self, stream_handler):
        wsh = find_wrapper(WrapperStreamHandler, stream_handler, self)
        _Proactor.cancel_stream_write(self, wsh)

    def close_active_stream(self, stream_handler):
        wsh = find_wrapper(WrapperStreamHandler, stream_handler, self)
        _Proactor.close_active_stream(self, wsh)
        kill_wrapper(WrapperStreamHandler, stream_handler, self)
    
    def schedule_timer(self, delay, timer_handler):
        wth = force_wrapper(WrapperTimerHandler, timer_handler, self)
        _Proactor.schedule_timer(self, delay, wth)

    def cancel_timer(self, timer_handler):
        wth = find_wapper(WrapperTimerHandler, timer_handler, self)
        _Proactor.cancel_timer(self, wth)
        kill_wrapper(WrapperTimerHandler, timer_handler, self)

    def handle_events(self, delay = None):
        if delay != None:
            _Proactor.handle_events(self, delay)
        else:
            _Proactor.handle_events(self)

    def handle_events_forever(self):
        _Proactor.handle_events_foreve(self)
                                          
    