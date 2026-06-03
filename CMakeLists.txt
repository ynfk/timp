cmake_minimum_required(VERSION 3.20)

project(andreev-artem-251-351 VERSION 2.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск Qt
# Режим сборки только сервера (для Docker)

# Полная сборка
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Sql)

find_package(OpenSSL REQUIRED)

# Поиск OpenSSL (исправлено)
find_package(OpenSSL REQUIRED)

# ==============================================
# ОБЩИЕ ФАЙЛЫ
# ==============================================
set(COMMON_SOURCES
    crypto/tripledes.cpp
    crypto/sha256.cpp
    math/spline.cpp
    math/gradientdescent.cpp
)

set(COMMON_HEADERS
    protocol.h
    crypto/tripledes.h
    crypto/sha256.h
    math/spline.h
    math/gradientdescent.h
)

# ==============================================
# КЛИЕНТ
# ==============================================
add_executable(client
    main.cpp
    mainwindow.cpp
    client/networkclient.cpp
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
    mainwindow.ui
)

# ==============================================
# СЕРВЕР
# ==============================================
add_executable(server
    server/main_server.cpp
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

# ==============================================
# ЛИНКОВКА КЛИЕНТА (исправлено)
# ==============================================
target_link_libraries(client PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    ${OPENSSL_LIBRARIES}      # OpenSSL
    ssl                        # Явно добавляем ssl
    crypto                     # Явно добавляем crypto
)

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/crypto
    ${CMAKE_CURRENT_SOURCE_DIR}/math
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${OPENSSL_INCLUDE_DIRS}
)

# ==============================================
# ЛИНКОВКА СЕРВЕРА (исправлено)
# ==============================================
target_link_libraries(server PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    ${OPENSSL_LIBRARIES}      # OpenSSL
    ssl                        # Явно добавляем ssl
    crypto                     # Явно добавляем crypto
)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/crypto
    ${CMAKE_CURRENT_SOURCE_DIR}/math
    ${OPENSSL_INCLUDE_DIRS}
)

# ==============================================
# НАСТРОЙКИ ДЛЯ MACOS
# ==============================================
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.stegoapp)
endif()

set_target_properties(client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

message(STATUS "")
message(STATUS "==========================================")
message(STATUS "Собрано 2 исполняемых файла:")
message(STATUS "  - client")
message(STATUS "  - server")
message(STATUS "==========================================")
