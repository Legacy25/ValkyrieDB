set(CMAKE_CXX_COMPILER clang++-3.5)

set(CMD_LLVM_INC llvm-config-3.5 --system-libs --libs core native jit bitwriter)
set(CMD_FLAGS llvm-config-3.5 --cxxflags --ldflags)


execute_process(COMMAND ${CMD_LLVM_INC} OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${CMD_FLAGS} OUTPUT_VARIABLE FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS ${FLAGS})


file(GLOB VALKYRIE_INC "include/*.h")
file(GLOB VALKYRIE_SRC "src/*.cpp")

include_directories(PRIVATE include PRIVATE ${LLVM_LIBS})

set(SOURCE_FILES ${VALKYRIE_INC} ${VALKYRIE_SRC})
add_executable(VALKYRIE ${SOURCE_FILES})
