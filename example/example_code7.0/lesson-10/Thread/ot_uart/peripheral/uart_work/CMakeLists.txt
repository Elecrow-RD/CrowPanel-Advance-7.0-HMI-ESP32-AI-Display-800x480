FILE(GLOB_RECURSE component_sources "*.c")

idf_component_register(SRCS ${component_sources}
                    INCLUDE_DIRS "include"
                    REQUIRES driver)


