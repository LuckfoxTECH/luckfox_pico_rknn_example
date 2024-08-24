file(GLOB SRCS *.cc)
file(GLOB HDRS *.h)
add_library(GPIO STATIC ${SRCS} ${HDRS})
