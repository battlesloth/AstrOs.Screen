FILE(GLOB_RECURSE UI_Sources ${CMAKE_SOURCE_DIR} "ui/*.c")
FILE(GLOB_RECURSE Wifi_Sources ${CMAKE_SOURCE_DIR} "wifi_controller/*.cpp")   
FILE(GLOB_RECURSE Utility ${CMAKE_SOURCE_DIR} "utility/*.c")   
FILE(GLOB_RECURSE Storage_c ${CMAKE_SOURCE_DIR} "storage/*.c")
FILE(GLOB_RECURSE Storage_cpp ${CMAKE_SOURCE_DIR} "storage/*.cpp")
FILE(GLOB_RECURSE Scripts ${CMAKE_SOURCE_DIR} "scripts/*.cpp")

idf_component_register(SRCS 
    "main.cpp" 
    ${UI_Sources}
    ${Wifi_Sources}
    ${Utility}
    ${Storage_c}
    ${Storage_cpp}
    ${Scripts}
        INCLUDE_DIRS 
        "." 
        "ui" 
        "wifi_controller" 
        "utility"
        "storage"
        "scripts")

