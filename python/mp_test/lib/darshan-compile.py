from distutils.core import setup, Extension

module1 = Extension('darshan',
                    sources = ['darshan.c'])

setup (name = 'darshan',
       version = '1.0',
       description = 'This is a darshan package',
       ext_modules = [module1])
