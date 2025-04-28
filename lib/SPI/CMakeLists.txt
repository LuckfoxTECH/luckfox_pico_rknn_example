file(GLOB SRCS *.cc)
file(GLOB HDRS *.h)

add_library(SPI STATIC ${SRCS} ${HDRS})
