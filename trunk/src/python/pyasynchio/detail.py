def override_constructor(cls, cons):
    def override_func(cls, *args, **kwargs):
        return cons(*args, **kwargs)
    dc = {'__new__' : override_func}
    from new import classobj
    return classobj(cls.__name__, (cls,), dc)
