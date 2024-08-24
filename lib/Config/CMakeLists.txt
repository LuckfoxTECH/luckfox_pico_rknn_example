file(GLOB SRCS *.cc)
file(GLOB HDRS *.h)
add_library(Config STATIC ${SRCS} ${HDRS})
target_link_libraries(Config PUBLIC GPIO SPI)
