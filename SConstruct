# master SConstruct for tigerbeetle
#
# author: Philippe Proulx <eepp.ca>


import os


# build mode (debug/release; default to release)
build_mode = ARGUMENTS.get('mode', 'release')

if build_mode not in ['debug', 'release']:
    sys.stderr.write('Oh noes: only "debug" and "release" build modes are supported\n')
    Exit(1)

# C++ flags and defines
ccflags = [
    '-std=c++11',
    '-Wall',
    '-pedantic-errors',
]

cppdefines = []

if build_mode == 'debug':
    ccflags += ['-g', '-O0']
    cppdefines += ['_DEBUG']
elif build_mode == 'release':
    ccflags += ['-O2']
    cppdefines += ['NDEBUG']

# this is to allow colorgcc
custom_env = {
    'PATH': os.environ['PATH'],
    'TERM': os.environ['TERM'],
    'HOME': os.environ['HOME'],
}

root_env = Environment(CCFLAGS=ccflags,
                       CPPDEFINES=cppdefines,
                       ENV=custom_env)

root_env.Append(LINKFLAGS = Split('-z origin'))

if 'CXX' in os.environ:
    root_env['CXX'] = os.environ['CXX']

if 'LIBDELOREAN_CPPPATH' in os.environ:
    root_env.Append(CPPPATH=[os.environ['LIBDELOREAN_CPPPATH']])
else:
    root_env.Append(CPPPATH=['#/contrib/libdelorean/include'])


if 'LIBDELOREAN_LIBPATH' in os.environ:
    root_env.Append(LIBPATH=[os.environ['LIBDELOREAN_LIBPATH']])
else:
    root_env.Append(LIBPATH=['#/contrib/libdelorean/src'])
    root_env.Append(RPATH = [ root_env.Literal(os.path.join('\\$$ORIGIN', os.pardir, 'contrib/libdelorean/src')) ])

if 'BABELTRACE_CPPPATH' in os.environ:
    root_env.Append(CPPPATH=[os.environ['BABELTRACE_CPPPATH']])
else:
    root_env.Append(CPPPATH=['#/contrib/babeltrace/include'])

if 'BABELTRACE_LIBPATH' in os.environ:
    root_env.Append(LIBPATH=[os.environ['BABELTRACE_LIBPATH']])
else:
    root_env.Append(LIBPATH=['#/contrib/babeltrace/lib/.libs'])
    root_env.Append(RPATH = [ root_env.Literal(os.path.join('\\$$ORIGIN', os.pardir, 'contrib/babeltrace/lib/.libs')) ])

if 'BABELTRACE_CTF_LIBPATH' in os.environ:
    root_env.Append(LIBPATH=[os.environ['BABELTRACE_CTF_LIBPATH']])
else:
    root_env.Append(LIBPATH=['#/contrib/babeltrace/formats/ctf/.libs'])
    root_env.Append(RPATH = [ root_env.Literal(os.path.join('\\$$ORIGIN', os.pardir, 'contrib/babeltrace/formats/ctf/.libs')) ])

#if 'LD_LIBRARY_PATH' in os.environ:
    #root_env['ENV']['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH']

Export('root_env')

apps = SConscript(os.path.join('src', 'SConscript'))
