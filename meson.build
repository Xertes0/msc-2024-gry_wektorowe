project('vector_games', 'c',
        default_options : ['warning_level=3'])

add_global_arguments('-Wconversion',
                     '-Wshadow',
                     language : 'c')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

hmm_dep = declare_dependency(
  dependencies : [m_dep],
  include_directories : include_directories('./3d_party/HandmadeMath',
                                            is_system : true),
  # compile_args : ['-DHANDMADE_MATH_NO_SSE'],
)

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

sokol_dep = declare_dependency(
  dependencies : [
    dependency('threads'),
    m_dep,
    sokol_ps_deps,
  ],
  include_directories : include_directories('./3d_party/sokol',
                                            is_system : true),
  compile_args : ['-DSOKOL_GLCORE33', '-DSOKOL_NO_DEPRECATED'],
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


shaders = shdc_gen.process('shaders/lines.glsl')

executable('asteroids',
           'src/sokol.c',
           'src/ast/actor.c',
           'src/ast/asteroid.c',
           'src/ast/asteroids.c',
           'src/ast/object.c',
           'src/ast/pipelines.c',
           'src/ast/ship.c',
           shaders,
           include_directories : include_directories('src'),
           dependencies : [sokol_dep, hmm_dep])
