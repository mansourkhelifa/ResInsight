cmake_minimum_required (VERSION 2.8)

project (cafTensor)

include_directories (
    ${LibCore_SOURCE_DIR}
)

set( PROJECT_FILES
	cafTensor3.cpp
	cafTensor3.h
	cafTensor3.inl
)


add_library( ${PROJECT_NAME}
 	${PROJECT_FILES}
)

target_link_libraries( ${PROJECT_NAME} LibCore )

source_group("" FILES ${PROJECT_FILES})