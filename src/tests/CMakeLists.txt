cmake_minimum_required(VERSION 3.22)

project(3DViewerTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем пути для исходников модели
file(GLOB MODEL_SOURCES
    "${CMAKE_SOURCE_DIR}/../model/*.cpp"
    "${CMAKE_SOURCE_DIR}/../model/*.h"
)

# Создаём статическую библиотеку модели
add_library(viewer_model STATIC ${MODEL_SOURCES})

# Указываем язык для целевой библиотеки
set_target_properties(viewer_model PROPERTIES LINKER_LANGUAGE CXX)

# Добавляем флаги компиляции
target_compile_options(viewer_model PRIVATE
    -g          # Включение отладочной информации
    -O0         # Отключение оптимизаций
    -Wall       # Включение всех предупреждений
    -Wextra     # Включение дополнительных предупреждений
    -Wpedantic  # Включение строгих стандартов C++
    --coverage  # Включение покрытия кода
    -pedantic   # Включение строгих стандартов C++
)

# Настройка Google Test
enable_testing()
find_package(GTest REQUIRED)

# Указываем пути для тестов
file(GLOB TEST_SOURCES
    "${CMAKE_SOURCE_DIR}/*.cpp"
)

# Добавляем тесты
add_executable(run_tests ${TEST_SOURCES})

# Линкуем библиотеку и GTest
target_link_libraries(run_tests PRIVATE viewer_model GTest::GTest GTest::Main pthread)

# Указываем директорию с заголовочными файлами
target_include_directories(run_tests PRIVATE "${CMAKE_SOURCE_DIR}/../")

# Определяем тесты
add_test(NAME ModelTests COMMAND run_tests)

# Добавляем команду для сборки библиотеки перед тестами
add_dependencies(run_tests viewer_model)

# Поддержка покрытия кода
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif()

find_program(LCOV lcov)
find_program(GENHTML genhtml)

add_custom_target(coverage
    COMMAND ${LCOV} --capture --directory . --output-file coverage.info --exclude "*tests/*" --exclude "*c++/*" --exclude "*gtest/*"
    COMMAND ${GENHTML} coverage.info --output-directory ../../report/coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
)
