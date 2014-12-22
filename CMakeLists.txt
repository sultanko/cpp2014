cmake_minimum_required(VERSION 2.8.9)
project(cursach)

#find_package(Qt5Core)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)


set(SOURCE_FILES
    abstractsocket.cpp
    clientsocket.cpp
    epollengineer.cpp
    main.cpp
    mainwindow.cpp
    serversocket.cpp
    supertcpmanager.cpp
    httprequest.cpp
    httpresponse.cpp
    httpserver.cpp
    )


set(cursach_HEADERS
    abstractsocket.h
    clientsocket.h
    epollengineer.h
    mainwindow.h
    serversocket.h
    supertcpmanager.h
    httprequest.h
    httpresponse.h
    httpserver.h
    )

set(cursach_FORMS
    mainwindow.ui)


#QT5_WRAP_CPP(cursach_HEADERS_MOC ${cursach_HEADERS})
#QT5_WRAP_UI(cursach_FORMS_HEADERS ${cursach_FORMS})

#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})
add_executable(cursach ${SOURCE_FILES}
                ${cursach_HEADERS_MOC}
                ${cursach_FORMS_HEADERS})

qt5_use_modules(cursach Widgets)
TARGET_LINK_LIBRARIES(cursach ${QT_LIBRARIES})
