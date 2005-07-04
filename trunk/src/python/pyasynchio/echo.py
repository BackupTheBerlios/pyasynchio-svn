class Echo:
    def __init__(self, pro, ctx):
        self.pro = pro
        self.ctx = ctx
        self.amount = 102400
        
        ctx.sigStream_.connect(lambda ctx : self.setupStream(ctx))

    def close(self):
        self.pro.close(self.ctx)

    def setupStream(self, stream_ctx):
        stream_ctx.sigRead_.connect(lambda result : self.echo(stream_ctx, result));
        stream_ctx.sigWrite_.connect(lambda result : self.confirm(stream_ctx, result))
        stream_ctx.sigOpen_.connect(lambda handle: self.open(stream_ctx))

    def open(self, stream_ctx):
        self.pro.read(stream_ctx, self.amount)

    def echo(self, stream_ctx, r):
        if (not r.success_) or r.data_ == "shutdown":
            self.pro.cancelRead(stream_ctx)
            self.pro.cancelWrite(stream_ctx)
            self.pro.close(stream_ctx)
            return

        if len(r.data_) > 0:
            self.pro.write(stream_ctx, r.data_)
            self.pro.read(stream_ctx, self.amount)
        else:
            self.pro.close(stream_ctx)

    def confirm(self, stream_ctx, r):
        if not r.success_:
            self.pro.close(stream_ctx)
