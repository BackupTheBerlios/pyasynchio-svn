def main():
    from sys import argv
    from os.path import abspath, join
    cfg = argv[1]
    space_src_path = abspath(join('..', '..', 'src', 'python'))
    bin_path = abspath(join('..', '..', 'bin', cfg))
    dst_path = abspath(cfg)
    from os import system, chdir
    system('python ../../src/python/gather.py %s %s %s' %
        (space_src_path, bin_path, dst_path))
    chdir(cfg)
    system('python run_unittests.py')

if __name__ == '__main__':
    main()

    
