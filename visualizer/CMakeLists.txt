cmake_minimum_required(VERSION 3.20)
project(rose_visualizer LANGUAGES C CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

find_package(Java REQUIRED COMPONENTS Runtime)
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)

include(FetchContent)
FetchContent_Declare(glad
    URL https://codeload.github.com/Dav1dde/glad/tar.gz/refs/tags/v2.0.8
    URL_HASH SHA256=44f06f9195427c7017f5028d0894f57eb216b0a8f7c4eda7ce883732aeb2d0fc
    SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(rose_glad STATIC REPRODUCIBLE API gl:compatibility=3.0 EXTENSIONS GL_ARB_framebuffer_object GL_EXT_framebuffer_object)

set(parser_dir "${CMAKE_CURRENT_BINARY_DIR}/parser")
set(parser_sources
    "${parser_dir}/node.cpp"
    "${parser_dir}/lexer.cpp"
    "${parser_dir}/typeinfo.cpp"
    "${parser_dir}/list.cpp"
    "${parser_dir}/analysis.cpp"
    "${parser_dir}/parser.cpp"
)

add_custom_command(
    OUTPUT ${parser_sources}
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${parser_dir}"
    COMMAND "${Java_JAVA_EXECUTABLE}" -jar "${CMAKE_CURRENT_SOURCE_DIR}/tools/sablecc.jar"
            -t cxx -d "${parser_dir}" "${CMAKE_CURRENT_SOURCE_DIR}/rose.sablecc"
    DEPENDS rose.sablecc tools/sablecc.jar
    VERBATIM
)

add_executable(rose main.cpp translate.cpp renderer.cpp music.cpp ${parser_sources})
target_compile_features(rose PRIVATE cxx_std_11)
target_include_directories(rose PRIVATE "${parser_dir}/rose")
target_link_libraries(rose PRIVATE PkgConfig::GLFW rose_glad PkgConfig::PORTAUDIO OpenGL::GL)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(rose PRIVATE -Wno-write-strings)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(portability_test tests/portability.cpp music.cpp)
    target_compile_features(portability_test PRIVATE cxx_std_11)
    target_link_libraries(portability_test PRIVATE PkgConfig::PORTAUDIO)
    add_test(NAME portability COMMAND portability_test "${CMAKE_CURRENT_BINARY_DIR}/watch-test.rose")
endif()

install(TARGETS rose RUNTIME DESTINATION .)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../examples" DESTINATION .)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../README.txt" DESTINATION .)