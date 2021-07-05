project('rime', 'cpp', default_options : ['warning_level=3', 'cpp_debugstl=true'], meson_version : '>=0.55.0')

#インクルードディレクトリ
include_dir = include_directories('include')

if not meson.is_subproject()

#コンパイルオプション調整
cppcompiler = meson.get_compiler('cpp').get_argument_syntax()

if cppcompiler == 'msvc'
    options = ['/std:c++latest', '/source-charset:utf-8', '/Zc:__cplusplus']
elif cppcompiler == 'gcc'
    options = ['-std=c++2b']
endif

#VSプロジェクトに編集しうるファイルを追加する
vs_files = ['include/rime.hpp']

#依存プロジェクト
#boostut_proj = subproject('boost.ut')
#boostut_dep = boostut_proj.get_variable('boostut_dep')
boostut_dep = dependency('boost.ut')

exe = executable('rime_test', 'test/rime_test.cpp', include_directories : include_dir, extra_files : vs_files, cpp_args : options, dependencies : [boostut_dep])
test('rime test', exe)

else

# subprojectとして構築時は依存オブジェクトの宣言だけしとく
rime_dep = declare_dependency(include_directories : include_directories('include'))

endif
