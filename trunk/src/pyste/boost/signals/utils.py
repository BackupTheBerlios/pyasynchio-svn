def export_signal(signal_type, header, override_call = True):
    Include("space-pyste/signal_connect_override.hpp")
    Include("space-pyste/signal_call_override.hpp")

    signal = Class(signal_type, header)
    set_policy(signal.combiner, return_internal_reference(1))
    exclude(signal.connect)
    class_code(signal, '.def("connect", &connect_override_one<%s>)' % signal_type)
    class_code(signal, '.def("connect", &connect_override_two<%s>)' % signal_type)
    class_code(signal, '.def("connect", &connect_override_three<%s>)' % signal_type)
    class_code(signal, '.def("connect", &connect_override_four<%s>)' % signal_type)
    if override_call:
        exclude(signal.operator['()'])
        class_code(signal, '.def("__call__", &call_override<%s>::call)' % signal_type)
    return signal
