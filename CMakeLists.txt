cmake_minimum_required(VERSION 3.16)

project(SistemaDiE VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(Qt6 REQUIRED COMPONENTS SerialPort)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)


# Set the build output location to a common directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
add_subdirectory(OpenXLSX)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SistemaDiE
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        config.h config.cpp config.ui
        ledindicator.cpp ledindicator.h
        OpenXLSX/external/nowide/nowide/args.hpp OpenXLSX/external/nowide/nowide/cenv.hpp OpenXLSX/external/nowide/nowide/config.hpp OpenXLSX/external/nowide/nowide/convert.hpp OpenXLSX/external/nowide/nowide/cstdio.hpp OpenXLSX/external/nowide/nowide/cstdlib.hpp OpenXLSX/external/nowide/nowide/encoding_errors.hpp OpenXLSX/external/nowide/nowide/encoding_utf.hpp OpenXLSX/external/nowide/nowide/filebuf.hpp OpenXLSX/external/nowide/nowide/fstream.hpp OpenXLSX/external/nowide/nowide/iostream.hpp OpenXLSX/external/nowide/nowide/scoped_ptr.hpp OpenXLSX/external/nowide/nowide/stackstring.hpp OpenXLSX/external/nowide/nowide/system.hpp OpenXLSX/external/nowide/nowide/utf.hpp OpenXLSX/external/nowide/nowide/utf8_codecvt.hpp OpenXLSX/external/nowide/nowide/windows.hpp OpenXLSX/external/pugixml/pugiconfig.hpp OpenXLSX/external/pugixml/pugixml.cpp OpenXLSX/external/pugixml/pugixml.hpp OpenXLSX/external/zippy/zippy.hpp OpenXLSX/headers/IZipArchive.hpp OpenXLSX/headers/XLCell.hpp OpenXLSX/headers/XLCellIterator.hpp OpenXLSX/headers/XLCellRange.hpp OpenXLSX/headers/XLCellReference.hpp OpenXLSX/headers/XLCellValue.hpp OpenXLSX/headers/XLColor.hpp OpenXLSX/headers/XLColumn.hpp OpenXLSX/headers/XLCommandQuery.hpp OpenXLSX/headers/XLComments.hpp OpenXLSX/headers/XLConstants.hpp OpenXLSX/headers/XLContentTypes.hpp OpenXLSX/headers/XLDateTime.hpp OpenXLSX/headers/XLDocument.hpp OpenXLSX/headers/XLException.hpp OpenXLSX/headers/XLFormula.hpp OpenXLSX/headers/XLIterator.hpp OpenXLSX/headers/XLMergeCells.hpp OpenXLSX/headers/XLProperties.hpp OpenXLSX/headers/XLRelationships.hpp OpenXLSX/headers/XLRow.hpp OpenXLSX/headers/XLRowData.hpp OpenXLSX/headers/XLSharedStrings.hpp OpenXLSX/headers/XLSheet.hpp OpenXLSX/headers/XLStyles.hpp OpenXLSX/headers/XLWorkbook.hpp OpenXLSX/headers/XLXmlData.hpp OpenXLSX/headers/XLXmlFile.hpp OpenXLSX/headers/XLXmlParser.hpp OpenXLSX/headers/XLZipArchive.hpp OpenXLSX/sources/utilities/XLUtilities.hpp OpenXLSX/sources/XLCell.cpp OpenXLSX/sources/XLCellIterator.cpp OpenXLSX/sources/XLCellRange.cpp OpenXLSX/sources/XLCellReference.cpp OpenXLSX/sources/XLCellValue.cpp OpenXLSX/sources/XLColor.cpp OpenXLSX/sources/XLColumn.cpp OpenXLSX/sources/XLComments.cpp OpenXLSX/sources/XLContentTypes.cpp OpenXLSX/sources/XLDateTime.cpp OpenXLSX/sources/XLDocument.cpp OpenXLSX/sources/XLFormula.cpp OpenXLSX/sources/XLMergeCells.cpp OpenXLSX/sources/XLProperties.cpp OpenXLSX/sources/XLRelationships.cpp OpenXLSX/sources/XLRow.cpp OpenXLSX/sources/XLRowData.cpp OpenXLSX/sources/XLSharedStrings.cpp OpenXLSX/sources/XLSheet.cpp OpenXLSX/sources/XLStyles.cpp OpenXLSX/sources/XLWorkbook.cpp OpenXLSX/sources/XLXmlData.cpp OpenXLSX/sources/XLXmlFile.cpp OpenXLSX/sources/XLXmlParser.cpp OpenXLSX/sources/XLZipArchive.cpp OpenXLSX/OpenXLSX.hpp
        recurso.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SistemaDiE APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SistemaDiE SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SistemaDiE
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SistemaDiE PRIVATE Qt${QT_VERSION_MAJOR}::Widgets OpenXLSX::OpenXLSX)
target_link_libraries(SistemaDiE PRIVATE Qt6::SerialPort)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SistemaDiE)
endif()
set_target_properties(SistemaDiE PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SistemaDiE
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SistemaDiE)
endif()
