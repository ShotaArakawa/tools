# JUCEプラグインのエディタを画面に出さずにPNGへ書き出すツール。
# 対象プラグインのCMakeLists.txtをそのまま取り込み、共有コード(静的ライブラリ)にリンクする。
cmake_minimum_required(VERSION 3.22)
project(PluginSnapshot VERSION 1.0.0)

if (NOT PLUGIN_DIR OR NOT PLUGIN_TARGET)
    message(FATAL_ERROR "-DPLUGIN_DIR=<プラグインのディレクトリ> -DPLUGIN_TARGET=<juce_add_pluginのターゲット名> を指定してください")
endif()

add_subdirectory(${PLUGIN_DIR} plugin)

add_executable(plugin-snapshot main.cpp)
target_compile_features(plugin-snapshot PRIVATE cxx_std_17)
# JuceHeader.hやJucePlugin_*の定義をプラグイン本体と揃える
target_include_directories(plugin-snapshot PRIVATE $<TARGET_PROPERTY:${PLUGIN_TARGET},INCLUDE_DIRECTORIES>)
target_compile_definitions(plugin-snapshot PRIVATE $<TARGET_PROPERTY:${PLUGIN_TARGET},COMPILE_DEFINITIONS>)
target_link_libraries(plugin-snapshot PRIVATE ${PLUGIN_TARGET})
