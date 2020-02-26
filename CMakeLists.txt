set(MODULE_NAME Files)
set(TARGET_NAME Pargon${MODULE_NAME})
set(MAIN_HEADER Include/Pargon/${MODULE_NAME}.h)

set(PUBLIC_HEADERS
	Include/Pargon/Files/Directory.h
	Include/Pargon/Files/File.h
)

set(SOURCES
	Source/Core/Directory.cpp
	Source/Core/File.cpp
)

set(DEPENDENCIES
	PargonContainers
	PargonTypes
)

set(WIN32_SOURCES
	Source/Win32/Files.Win32.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	list(APPEND SOURCES ${WIN32_SOURCES})
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include/Pargon/" FILES ${MAIN_HEADER})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include/Pargon/${MODULE_NAME}/" PREFIX Include FILES ${PUBLIC_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source/" FILES ${SOURCES})

add_library(${TARGET_NAME} STATIC)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)
target_include_directories(${TARGET_NAME} PUBLIC Include)
target_include_directories(${TARGET_NAME} PRIVATE Source)
target_link_libraries(${TARGET_NAME} PUBLIC ${DEPENDENCIES})
target_sources(${TARGET_NAME} PRIVATE "${MAIN_HEADER}" "${PUBLIC_HEADERS}" "${SOURCES}")
