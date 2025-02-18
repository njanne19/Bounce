cmake_minimum_required(VERSION 3.10)

# Define the project name and version 
project(Bounce VERSION 0.1.0 LANGUAGES CXX)

# Set the C++ standard 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# Add the include directory where wintun library is loacated
include_directories(${CMAKE_SOURCE_DIR}/client/include)

# Add all .cpp files in client/src (this will pick up main.cpp) 
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/client/src/*.cpp) 

# Create an executable target from the source files 
add_executable(Bounce ${SRC_FILES})

# Post-build command: copy the wintun.dll to the output directory
add_custom_command(TARGET Bounce POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/client/dll/x64/wintun.dll"
		$<TARGET_FILE_DIR:Bounce>
)