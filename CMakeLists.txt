cmake_minimum_required(VERSION 3.15)
project(xml_benchmarks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем XML-библиотеки
add_subdirectory(ext/tinyxml2)
add_subdirectory(ext/pugixml)

# RapidXML (header-only)
include_directories(${CMAKE_SOURCE_DIR}/ext/rapidxml/include)

# Бенчмарки
add_executable(tinyxml2_test benchmark/tinyxml2_test.cpp)
target_link_libraries(tinyxml2_test PRIVATE tinyxml2)

add_executable(pugixml_test benchmark/pugixml_test.cpp)
target_link_libraries(pugixml_test PRIVATE pugixml)

add_executable(rapidxml_test benchmark/rapidxml_test.cpp)
target_include_directories(rapidxml_test PRIVATE
    ${CMAKE_SOURCE_DIR}/ext/rapidxml
)

# Генератор XML
add_executable(xml_generator xml_generator.cpp)

# Тест-раннер
add_executable(test_runner test_runner.cpp)