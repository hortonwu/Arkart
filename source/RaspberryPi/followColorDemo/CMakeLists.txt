cmake_minimum_required(VERSION 2.8)
project( blob )
find_package( OpenCV REQUIRED )
find_package( OpenMP REQUIRED )

if(OPENMP_FOUND)
	message("OPENMP FOUND")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -lrt -pthread")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -lrt -pthread")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -lrt -pthread")
endif()

add_executable( blob TaskScheduling_Pi.cpp	 )
target_link_libraries( blob ${OpenCV_LIBS} ${OpenMP_LIBS} -lrt)
