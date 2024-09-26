cmake_minimum_required(VERSION 3.12)
project(html-show)

add_executable(${PROJECT_NAME} ./html.cpp)
target_link_libraries(${PROJECT_NAME} 
    Qt5::Core
    Qt5::Widgets
    QtNodes
    qtermwidget5
    pthread
    util
)
