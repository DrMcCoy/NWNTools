from distutils.core import setup, Extension

module1 = Extension('nsscompiler',
                    libraries = ['nsc','nwn'],
                    library_dirs = ['../../_NscLib','../../_NwnLib'],
                    extra_compile_args=['-Wno-sign-compare'],
                    sources = ['compiler.cpp'],
                    extra_link_args=['-Wl,-flat_namespace,-U,_environ'])

setup (name = 'nwntools',
       version = '1.01',
       description = 'nss compiler',
       author = 'By Torlack, python interface by sumpfork',
       author_email = 'sumpfork@users.sf.net',
       url = 'http://www.python.org/doc/current/ext/building.html',
       long_description = '''
nss compiler in python
''',
	ext_modules=[module1])
