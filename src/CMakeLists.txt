add_executable(z64fe main.cpp
                     MainWindow.cpp ${CMAKE_SOURCE_DIR}/include/MainWindow.hpp
                     ROMFileWidget.cpp ${CMAKE_SOURCE_DIR}/include/ROMFileWidget.hpp
                     ROM.cpp
                     ROMFileModel.cpp ${CMAKE_SOURCE_DIR}/include/ROMFileModel.hpp
                     utility.cpp
                     HexViewer.cpp ${CMAKE_SOURCE_DIR}/include/HexViewer.hpp
                     yaz0.cpp
                     Config.cpp
                     ConfigTree.cpp
                     Exceptions.cpp
                     ${CMAKE_SOURCE_DIR}/res/qtres.qrc
                     TextViewer.cpp ${CMAKE_SOURCE_DIR}/include/TextViewer.hpp
                     TextIDModel.cpp ${CMAKE_SOURCE_DIR}/include/TextIDModel.hpp
                     TextConv.cpp
                     TextAST.cpp
                     TextRender.cpp ${CMAKE_SOURCE_DIR}/include/TextRender.hpp)
target_link_libraries(z64fe Qt5::Widgets)