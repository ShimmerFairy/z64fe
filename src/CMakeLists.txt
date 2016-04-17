add_executable(z64fe main.cpp
                     MainWindow.cpp ${CMAKE_SOURCE_DIR}/include/MainWindow.hpp
                     ROMFileWidget.cpp ${CMAKE_SOURCE_DIR}/include/ROMFileWidget.hpp
                     ROMInfoWidget.cpp ${CMAKE_SOURCE_DIR}/include/ROMInfoWidget.hpp
                     ROM.cpp
                     ROMFileModel.cpp ${CMAKE_SOURCE_DIR}/include/ROMFileModel.hpp
                     utility.cpp
                     yaz0.cpp
                     Config.cpp
                     ConfigTree.cpp
                     Exceptions.cpp
                     ${CMAKE_SOURCE_DIR}/res/qtres.qrc
                     TextViewer.cpp ${CMAKE_SOURCE_DIR}/include/TextViewer.hpp
                     TextIDModel.cpp ${CMAKE_SOURCE_DIR}/include/TextIDModel.hpp
                     TextConv.cpp
                     TextAST.cpp
                     TextRender.cpp ${CMAKE_SOURCE_DIR}/include/TextRender.hpp
                     Hex/Widget.cpp ${CMAKE_SOURCE_DIR}/include/Hex/Widget.hpp
                     Hex/Cursor.cpp)
target_link_libraries(z64fe Qt5::Widgets Qt5::Concurrent)