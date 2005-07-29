default_defines = ['-DNDEBUG', '-DGCCXML', '-D_MSC_EXTENSIONS'
                    , '-D_WIN32', '-D_WINDOWS', '-D_MT', '-D__STDC__=0', '-D_CPPUNWIND']

def launch(module, includes ,files, out = None, multiple = None):
    import sys
    from sys import argv
    from copy import copy, deepcopy
    del argv[1:]
    argv.extend(default_defines)
    argv.extend(map(lambda include : '-I%s' % include, includes))
    argv.append('--module=%s' % module)
    if out == None:
        out = module
    argv.append('--out=%s' % out)
    if multiple:
        argv.append('--multiple')
    argv_saved = copy(argv)
    argv.extend(files)
    print argv


    from os import system

    def makecmd():
        import string
        return ('python -c "from Pyste import pyste; pyste.main()" ' 
            + string.join(argv[1:], ' '))
    
    system(makecmd())
    
    argv = argv_saved
    if multiple:
        argv.append('--generate-main')
        argv.extend(files)
        print 'generating main'
        system(makecmd())
        
    
    
def main():
    from sys import argv
    glob = {}
    loc = {}
    execfile(argv[1], glob, loc)
    launch(module = loc['module']
        , includes = loc['includes']
        , files = loc['files']
        , out = loc['out']
        , multiple = loc.get('multiple', None))

if __name__ == '__main__':
    main()
