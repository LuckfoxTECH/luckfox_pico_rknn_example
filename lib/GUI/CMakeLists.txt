file(GLOB SRCS *.cc)
file(GLOB HDRS *.h)
 
add_library(GUI STATIC ${SRCS} ${HDRS})
target_link_libraries(GUI PUBLIC Config)