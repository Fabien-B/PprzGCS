cmake_minimum_required(VERSION 3.10)

project(PprzGCS)

include(GNUInstallDirs)

set(CXX_EXTRA_FLAGS "-Wall -Wextra -fno-sized-deallocation")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CXX_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_EXTRA_FLAGS} -Werror")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# find library proj (from 20.04 packages) or PROJ4 (from source)
find_library(PROJ proj)
if(${PROJ} STREQUAL "PROJ-NOTFOUND")
    find_package(PROJ4 REQUIRED)
else()
    set(PROJ4_LIBRARIES proj)
endif()

find_package(LibXml2 REQUIRED)

add_definitions(-DDEFAULT_APP_DATA_PATH="/usr/local/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")

find_package(Qt5 5.12 COMPONENTS Widgets Xml Network Svg REQUIRED)
find_package(IvyQt REQUIRED)
find_package(pprzlinkQt REQUIRED)

set(SOURCE
    ${SOURCE}
    resources/ressources.qrc
)

add_subdirectory(src)

include_directories(
    ${INC_DIRS}
    ${LIBXML2_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Xml
    Qt5::Network
    Qt5::Svg
    ${PROJ4_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    zip
    pprzlinkQt
    IvyQt
)

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/data/ DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")


