FILE(GLOB_RECURSE component_sources "src/*.c" "src/*.cpp" "src/**/*.c" "src/**/*.cpp")
idf_component_register(SRCS ${component_sources} INCLUDE_DIRS "include")
