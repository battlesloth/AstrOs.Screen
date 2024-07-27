FILE(GLOB_RECURSE UI_Sources ${CMAKE_SOURCE_DIR} "ui/*.c")

idf_component_register(SRCS 
    "main.cpp" 
    ${UI_Sources}
        INCLUDE_DIRS "." "ui")

