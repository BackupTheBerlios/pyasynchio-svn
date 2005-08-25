from os.path import *

try:
    from win32file import CreateHardLink
    def linkit(a,b):
        if not exists(b):
            try:
                CreateHardLink(b,a)
            except:
                import shutil
                shutil.copy(a,b)
    link = linkit
except ImportError:
    import shutil
    link = shutil.copy

def gather(space_src_path, bin_path, dst_path):
    from os import mkdir
    if not exists(dst_path):
        mkdir(dst_path)
    # Ok then, we are ready to link-out space python module
    def walker(dest, dirname, names):
        if ('.svn' in names):
            names.remove('.svn')

        for name in names[:]:

            if isdir(join(dirname, name)):
                if not exists(join(dest, name)):
                    mkdir(join(dest, name))
                names.remove(name)
                walk(join(dirname, name), walker, join(dest, name))

            import fnmatch
            if isfile(join(dirname, name)) and fnmatch.fnmatch(name, '*.py'):
                srcname = join(dirname, name)
                dstname = join(dest, name)
                if exists(dstname): 
                    from os import unlink
                    unlink(dstname)
                link(srcname, dstname)
    walk(space_src_path, walker, dst_path)

    # So, everything is linked-n-ready at this point, lets finish up with binaries

    def linkfile(src_dir, dst_dir, file_name):
        dst_file_name = join(dst_dir, file_name)
        src_file_name = join(src_dir, file_name)
        if exists(dst_file_name):
            from os import unlink
            unlink(dst_file_name)
        link(src_file_name, dst_file_name)

    pyasynchio_dst_path = join(dst_path, 'pyasynchio')
#    ace_dst_path = join(dst_path, 'ace')
#    boost_dst_path = join(dst_path, 'boost')
#    std_dst_path = join(dst_path, 'std')
#    boost_signals_dst_path = join(boost_dst_path, 'signals')
#    try:
#        linkfile(bin_path, dst_path, 'ACE.dll')
#    except:
#        linkfile(bin_path, dst_path, 'ACEd.dll')
#    linkfile(bin_path, dst_path, 'boost_signals.dll')
#    linkfile(bin_path, dst_path, 'boost_python.dll')
    linkfile(bin_path, pyasynchio_dst_path, '_pyasynchio.dll')
#    linkfile(bin_path, ace_dst_path, '_ace.pyd')
#    linkfile(bin_path, std_dst_path, '_std.pyd')
#    linkfile(bin_path, boost_dst_path, '_boost.pyd')
 #   linkfile(bin_path, boost_signals_dst_path, '_boost_signals.pyd')
#    linkfile(bin_path, pyasynchio_dst_path, '_pyasynchio.pyd')

def main():
    from sys import argv
    space_src_path = argv[1]
    bin_path = argv[2]
    space_dst_path = argv[3]
    gather(space_src_path, bin_path, space_dst_path)

if __name__ == "__main__":
    main()