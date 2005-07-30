import handlers

def bind_attr(wrapper, handler, function_name):
    if hasattr(handler, function_name):
        setattr(wrapper, function_name, getattr(handler, function_name))

def force_wrapper(wrapper_class, handler, proactor):
    if not (handler, proactor) in wrapper_class.lut:
        wrapper_class.lut[(handler, proactor)] = {
            'value' : wrapper_class(handler, proactor)
            , 'refcount' : 0 }

    wrapper_class.lut[(handler, proactor)]['refcount'] += 1
    return wrapper_class.lut[(handler, proactor)]['value']

def find_wrapper(wrapper_class, handler, proactor):
    return wrapper_class.lut[(handler, proactor)]['value']

def kill_wrapper(wrapper_class, handler, proactor):
    del wrapper_class.lut[(handler, proactor)]


class WrapperAcceptHandler(handlers.AbstractAcceptHandler):
    lut = {}

    def __init__(self, rh, pro):
        self.__rh = rh
        self.__pro = pro
        handlers.AbstractAcceptHandler.__init__(self)
        bind_attr(self, rh, 'conn_validate')
        bind_attr(self, rh, 'conn_completed')
        
    def conn_validate(self, result, remote, local):
        return True

    def conn_completed(self, result):
        pass

    def make_stream_handler(self):          
        raw_stream_handler = self.__rh.make_stream_handler()
        return force_wrapper(WrapperStreamHandler, raw_stream_handler
                            , self.__pro)

class WrapperConnectHandler(handlers.AbstractConnectHandler):
    lut = {}

    def __init__(self, rh, pro):
        self.__rh = rh
        self.__pro = pro
        handlers.AbstractConnectHandler.__init__(self)
        bind_attr(self, rh, 'conn_validate')
        bind_attr(self, rh, 'conn_completed')

    def conn_validate(self, result, remote, local):
        return True

    def conn_completed(self, result):
        pass

    def make_stream_handler(self):          
        raw_stream_handler = self.__rh.make_stream_handler()
        return force_wrapper(WrapperStreamHandler, raw_stream_handler
                            , self.__pro)

    
class WrapperStreamHandler(handlers.AbstractStreamHandler):
    lut = {}

    def __init__(self, rh, pro):
        self.__rh = rh
        self.__pro = pro
        handlers.AbstractStreamHandler.__init__(self)
        bind_attr(self, rh, 'notify_initialized')
        bind_attr(self, rh, 'notify_endpoints')
        bind_attr(self, rh, 'read_completed')
        bind_attr(self, rh, 'write_completed')

    def notify_endpoints(self, remote, local):
        self.remote_address = remote
        self.local_address = local

    def notify_initialized(self):
        pass
   
    def read_completed(self, read_result):
        pass

    def write_completed(self, write_result):
        pass


class WrapperTimerHandler(handlers.AbstractTimerHandler):
    lut = {}

    def __init__(self, rh, pro):    
        self.__rh = rh
        self.__pro = pro
        handlers.AbstractTimerHandler.__init__(self)
        
    def notify_elapsed(self, tv):   
        tuppy = (self.__rh, self.__pro)
        WrapperTimerHandler.lut[tuppy]['refcount'] -= 1
        if WrapperTimerHandler.lut[tuppy]['refcount'] == 0:
            del WrapperTimerHandler.lut[tuppy]
        self.__rh.notify_elapsed(tv)


