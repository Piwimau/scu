project(
    'scu',
    'c',
    default_options: [
        'c_std=c23',
        'buildtype=debug',
        'b_ndebug=if-release',
        'warning_level=3',
    ],
    meson_version: '>=1.1',
    version: '0.4.1',
)

cc = meson.get_compiler('c')

if cc.get_id() == 'gcc'
    add_project_arguments(
        '-Wundef',
        '-Wshadow',
        '-Wconversion',
        '-Wdouble-promotion',
        '-Wcast-align',
        '-Wcast-qual',
        '-Wlogical-op',
        '-Wpointer-arith',
        '-Wformat=2',
        '-Wwrite-strings',
        '-Wmissing-declarations',
        '-Wmissing-include-dirs',
        '-Wmissing-prototypes',
        '-Wduplicated-cond',
        '-Wduplicated-branches',
        language: 'c',
    )
    if get_option('native')
        add_project_arguments('-march=native', '-mtune=native', language: 'c')
    endif
endif

incs = include_directories('include')

srcs = files(
    'src/alloc.c',
    'src/array.c',
    'src/assert.c',
    'src/bench.c',
    'src/compare.c',
    'src/equal.c',
    'src/error.c',
    'src/hash-map.c',
    'src/hash-set.c',
    'src/hash.c',
    'src/io.c',
    'src/list.c',
    'src/memory.c',
    'src/prio-queue.c',
    'src/queue.c',
    'src/stack.c',
    'src/string.c',
    'src/time.c',
)

m_dep = cc.find_library('m', required: false)
thread_dep = dependency('threads')

scu_lib = library(
    'scu',
    srcs,
    include_directories: incs,
    dependencies: [m_dep, thread_dep],
    install: true,
)

install_subdir('include/scu', install_dir: 'include')

scu_dep = declare_dependency(include_directories: incs, link_with: scu_lib)

pkg = import('pkgconfig')
pkg.generate(scu_lib)