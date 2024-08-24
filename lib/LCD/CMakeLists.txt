file(GLOB SRCS *.cc)
file(GLOB HDRS *.h)
 
add_library(LCD STATIC ${SRCS} ${HDRS})
target_link_libraries(LCD PUBLIC Config)