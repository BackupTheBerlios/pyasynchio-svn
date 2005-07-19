import boost
import ace
import std
from _pyasynchio import Proactor, TimerSignal
from handlers import AbstractAcceptHandler, AbstractConnectHandler, AbstractStreamHandler

import detail

#AcceptContext = detail.override_constructor(AcceptContext, AcceptContext.Create)
#ConnectContext = detail.override_constructor(ConnectContext, ConnectContext.Create)
#StreamContext = detail.override_constructor(StreamContext, StreamContext.Create)
TimerSignal = detail.override_constructor(TimerSignal, TimerSignal.Create)

