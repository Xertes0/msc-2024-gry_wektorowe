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

sokol_backend = 'GLCORE33'
if host_machine.system() == 'emscripten'
  sokol_backend = 'GLES3'
endif

sokol_dep = declare_dependency(
  dependencies : [
    dependency('threads'),
    m_dep,
    sokol_ps_deps,
  ],
  include_directories : include_directories('./3d_party/sokol',
                                            is_system : true),
  compile_args : ['-DSOKOL_' + sokol_backend,
                  '-DSOKOL_NO_DEPRECATED'],
)

# TODO: Account for macos
if build_machine.system() == 'cygwin' or build_machine.system() == 'windows'
  shdc_pathsuf = '/win32/sokol-shdc.exe'
elif build_machine.system() == 'linux'
  shdc_pathsuf = '/linux/sokol-shdc'
else
  error('Your operating system is not supported')
endif

shader_lang = 'glsl330'
if host_machine.system() == 'emscripten'
  shader_lang = 'glsl300es'
endif

# NOTE: Executable may not have executable permission by default
shdc = find_program('./3d_party/sokol-tools-bin/bin' + shdc_pathsuf)
shdc_gen = generator(shdc,
                     output : '@PLAINNAME@.h',
                     arguments : ['--input', '@INPUT@',
                                  '--output', '@OUTPUT@',
                                  '--slang', shader_lang])

shaders = shdc_gen.process('shaders/lines.glsl',
                           'shaders/offscreen.glsl',
                           'shaders/offscreen_v2.glsl')

exe_name_suffix = []
exe_ps_deps = []
if host_machine.system() == 'emscripten'
  exe_name_suffix = 'html'

  shell_path = meson.source_root() + '/html/shell.html'
  emscripten = declare_dependency(extra_files : [shell_path],
                                  link_args : ['-sMAX_WEBGL_VERSION=2',
                                               '-sMIN_WEBGL_VERSION=2',
                                               '--shell-file', shell_path])
  exe_ps_deps = [emscripten]
endif

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
           dependencies : [sokol_dep, hmm_dep] + exe_ps_deps,
           name_suffix : exe_name_suffix)
