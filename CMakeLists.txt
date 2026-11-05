cmake_minimum_required(VERSION 3.22)
project(SampleSquare VERSION 0.1.0 LANGUAGES CXX)

# Configuración de JUCE
set(JUCE_VERSION 8.0.0)
set(JUCE_MODULES
    juce_audio_basics
    juce_audio_devices
    juce_audio_formats
    juce_audio_processors
    juce_audio_utils
    juce_data_structures
    juce_dsp
    juce_gui_basics
    juce_gui_extra
)

# Descargar/Clonar JUCE si no existe
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
    execute_process(
        COMMAND git clone --depth 1 --branch ${JUCE_VERSION} https://github.com/juce-framework/JUCE.git ${CMAKE_CURRENT_SOURCE_DIR}/JUCE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE JUCE_CLONE_RESULT
    )
endif()

add_subdirectory(JUCE)

# Aplicación GUI
juce_add_gui_app(SampleSquare
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
)

file(GLOB_RECURSE SAMPLESQUARE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
target_sources(SampleSquare PRIVATE ${SAMPLESQUARE_SOURCES})

target_include_directories(SampleSquare PRIVATE Source)

target_compile_definitions(SampleSquare PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Enlace de módulos
target_link_libraries(SampleSquare PRIVATE
    juce_audio_utils
    juce_audio_devices
    juce_audio_formats
    juce_audio_processors
    juce_audio_basics
    juce_data_structures
    juce_gui_extra
    juce_dsp
    ${JUCE_RECOMMENDED_LDFLAGS}
)

# Configuración de artefactos (Linux/Windows/macOS)
set_target_properties(SampleSquare PROPERTIES
    OUTPUT_NAME SampleSquare
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/SampleSquare_artefacts/$<CONFIG>"
)
