# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
VERSION='0.1'
APPNAME='ndn_repo'

from waflib import Build, Logs, Utils, Task, TaskGen, Configure

def options(opt):
    opt.add_option('--debug',action='store_true',default=False,dest='debug',help='''debugging mode''')
    opt.add_option('--test', action='store_true',default=False,dest='_test',help='''build unit tests''')

    # if Utils.unversioned_sys_platform () == "darwin":
    #     pass

    opt.load('compiler_c compiler_cxx boost')

def configure(conf):
    conf.load("compiler_c compiler_cxx")

    if conf.options.debug:
        conf.define ('NDN_REPO_DEBUG', 1)
        conf.add_supported_cxxflags (cxxflags = ['-O0',
                                                 '-Wall',
                                                 '-Wno-unused-variable',
                                                 '-g3',
                                                 '-Wno-unused-private-field', # only clang supports
                                                 '-fcolor-diagnostics',       # only clang supports
                                                 '-Qunused-arguments'         # only clang supports
                                                 ])
    else:
        conf.add_supported_cxxflags (cxxflags = ['-O3', '-g'])

    conf.check_cfg(package='sqlite3', args=['--cflags', '--libs'], uselib_store='SQLITE3', mandatory=True)

    conf.write_config_header('config.h')
    
    conf.check_cfg(package='libndn-cpp-dev', args=['--cflags', '--libs'], uselib_store='NDNCPPDEV', mandatory=True)
    conf.env.append_value('INCLUDES', ['/usr/local/include'])

    conf.load('boost')
    conf.check_boost(lib='system test iostreams filesystem thread')

    if conf.options._test:
        conf.define ('_TESTS', 1)
        conf.env.TEST = 1

def build (bld):
    bld (
        target = "ndn_repo",
        features = ["cxx", "cxxprogram"],
        source = bld.path.ant_glob(['server/server.cc', 'ndn_handle/*.cc','storage/*.cc','storage/sqlite/*.cc', 'helper/*.cc']),
        use = 'BOOST SQLITE3 NDNCPPDEV',
        includes = ".",
        )
    if bld.env.TEST:
        bld (
            target = "test/test_sqlite",
            features = ["cxx", "cxxprogram"],
            source = bld.path.ant_glob(['test/test_sqlite.cc','storage/*.cc','storage/sqlite/*.cc', 'helper/*.cc']),
            use = 'BOOST SQLITE3 NDNCPPDEV',
            includes = ".",
            )
        bld (
            target = "test/producer",
            features = ["cxx", "cxxprogram"],
            source = bld.path.ant_glob(['test/producer.cc','ndn_handle/*.cc','storage/*.cc','storage/sqlite/*.cc', 'helper/*.cc']),
            use = 'BOOST SQLITE3 NDNCPPDEV',
            includes = ".",
            )
        bld (
            target = "test/consumer",
            features = ["cxx", "cxxprogram"],
            source = bld.path.ant_glob(['test/consumer.cc','ndn_handle/*.cc','storage/*.cc','storage/sqlite/*.cc', 'helper/*.cc']),
            use = 'BOOST SQLITE3 NDNCPPDEV',
            includes = ".",
            )
        bld (
            target = "test/test-command-interest",
            features = ["cxx", "cxxprogram"],
            source = bld.path.ant_glob(['test/test-command-interest.cc','ndn_handle/*.cc','storage/*.cc','storage/sqlite/*.cc', 'helper/*.cc']),
            use = 'BOOST SQLITE3 NDNCPPDEV',
            includes = ".",
            )
        bld (
            target = "test/insert_client",
            features = ["cxx", "cxxprogram"],
            source = bld.path.ant_glob(['test/insert_client.cc','ndn_handle/*.cc','storage/*.cc','storage/sqlite/*.cc', 'helper/*.cc']),
            use = 'BOOST SQLITE3 NDNCPPDEV',
            includes = ".",
            )
        bld (
            target = "test/delete_client",
            features = ["cxx", "cxxprogram"],
            source = bld.path.ant_glob(['test/delete_client.cc','ndn_handle/*.cc','storage/*.cc','storage/sqlite/*.cc', 'helper/*.cc']),
            use = 'BOOST SQLITE3 NDNCPPDEV',
            includes = ".",
            )

@Configure.conf
def add_supported_cxxflags(self, cxxflags):
    """
    Check which cxxflags are supported by compiler and add them to env.CXXFLAGS variable
    """
    self.start_msg('Checking allowed flags for c++ compiler')

    supportedFlags = []
    for flag in cxxflags:
        if self.check_cxx (cxxflags=[flag], mandatory=False):
            supportedFlags += [flag]

    self.end_msg (' '.join (supportedFlags))
    self.env.CXXFLAGS += supportedFlags
