add_executable(z64fe main.cpp
                     MainWindow.cpp ${CMAKE_SOURCE_DIR}/include/MainWindow.hpp
                     ROM.cpp
                     ROMFileModel.cpp ${CMAKE_SOURCE_DIR}/include/ROMFileModel.hpp
                     utility.cpp)
target_link_libraries(z64fe Qt5::Widgets)