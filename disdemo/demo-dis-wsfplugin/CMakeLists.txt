project(demo-dis)

include (GenerateExportHeader)
include(swdev_project)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} PLUGIN
HDRS
   ${HDRS}
SRCS
   ${SRCS}
)

# Add the library with sources
#add_library(${PROJECT_NAME} ${SRCS})

# Generate header with the __declspec export macro defined.
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
                          "${CMAKE_CURRENT_SOURCE_DIR}"
                          "${PROJECT_BINARY_DIR}/source")

#for WsfDisInterface.hpp
include_directories(${WSF_ROOT}/wsf/source/dis)

target_link_libraries(${PROJECT_NAME} ${WSF_LIBS})