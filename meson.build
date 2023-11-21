project('vector_games', 'c')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

# Platform specific sokol dependencies
# TODO: Account for macos
sokol_ps_deps = []
if target_machine.system() == 'linux'
  sokol_ps_deps = [
    dependency('X11'),
    dependency('xi'),
    dependency('xcursor'),
    dependency('GL'),
    dependency('dl'),
  ]
endif

sokol = declare_dependency(
  dependencies : [
    dependency('threads'),
    m_dep,
    sokol_ps_deps,
  ],
  include_directories : include_directories('./3d_party/sokol')
)

# TODO: Account for macos
if build_machine.system() == 'cygwin' or build_machine.system() == 'windows'
  shdc_pathsuf = '/win32/sokol-shdc.exe'
elif build_machine.system() == 'linux'
  shdc_pathsuf = '/linux/sokol-shdc'
else
  error('Your operating system is not supported')
endif

# NOTE: Executable may not have executable permission by default
shdc = find_program('./3d_party/sokol-tools-bin/bin' + shdc_pathsuf)
shdc_gen = generator(shdc,
                     output : '@PLAINNAME@.h',
                     arguments : ['--input', '@INPUT@',
                                  '--output', '@OUTPUT@',
                                  '--slang', 'glsl330'])


shaders = shdc_gen.process('shaders/triangle.glsl')

executable('main',
           'src/main.c',
           shaders,
           dependencies : [sokol])
