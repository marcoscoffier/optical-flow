
package = "opticalflow"
version = "1.0-1"

source = {
   url = "opticalflow-1.0-1.tgz"
}

description = {
   summary = "This is a simple wrapper around the optical-flow algorithm developped/published by C.Liu",
   detailed = [[
   Eventually could become a wrapper for more algorithms but for now just:

   C. Liu. Beyond Pixels: Exploring New Representations and Applications
   for Motion Analysis. Doctoral Thesis. Massachusetts Institute of 
   Technology. May 2009.

   More at: http://people.csail.mit.edu/celiu/OpticalFlow/
   ]],
   homepage = "",
   license = "MIT/X11" -- or whatever you like
}

dependencies = {
   "lua >= 5.1",
   "torch",
   "xlua"
}

build = {
   type = "cmake",

   cmake = [[
         cmake_minimum_required(VERSION 2.8)

         set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

         # infer path for Torch7
         string (REGEX REPLACE "(.*)lib/luarocks/rocks.*" "\\1" TORCH_PREFIX "${CMAKE_INSTALL_PREFIX}" )
         message (STATUS "Found Torch7, installed in: " ${TORCH_PREFIX})

         find_package (Torch REQUIRED)
         find_package (Matlab REQUIRED)

	 SET(CMAKE_CXX_FLAGS "-DMATLAB_FOUND")
   	 MESSAGE(STATUS "Using Matlab datastructs")

         SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

   	 INCLUDE_DIRECTORIES (${MATLAB_INCLUDE_DIR} ${TORCH_INCLUDE_DIR} 
	   ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/generic)
	 MESSAGE(STATUS "matlab include: " ${MATLAB_INCLUDE_DIR})
   	 MESSAGE(STATUS "project include: " ${PROJECT_SOURCE_DIR})
   	 MESSAGE(STATUS "cmake module: " ${CMAKE_MODULE_PATH})
   	 add_library(celiu SHARED celiu.cpp)

	 link_directories (${TORCH_LIBRARY_DIR})
	 target_link_libraries(celiu ${TORCH_LIBRARIES} ${MATLAB_LIBRARIES})
	 install_files(/lua/opticalflow init.lua) 
	 install_targets(/lib celiu) 

   ]],

   variables = {
      CMAKE_INSTALL_PREFIX = "$(PREFIX)"
   }
}
