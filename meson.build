project('vector_games', 'c')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

sokol = declare_dependency(
  dependencies : [
    dependency('X11'),
    dependency('xi'),
    dependency('xcursor'),
    dependency('GL'),
    dependency('dl'),
    dependency('threads'),
    m_dep,
  ],
  include_directories : include_directories('./3d_party/sokol')
)

# TODO: Account for different platforms
shdc = find_program('./3d_party/sokol-tools-bin/bin/linux/sokol-shdc')
# TODO: Different shader type depending on platform
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
