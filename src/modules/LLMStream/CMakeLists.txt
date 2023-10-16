add_executable(LLMStream Module.h Module.cpp main.cpp)

find_package(YARP 3.8 REQUIRED COMPONENTS os sig dev)
target_link_libraries(LLMStream PRIVATE ${YARP_LIBRARIES})
install(TARGETS LLMStream DESTINATION bin)