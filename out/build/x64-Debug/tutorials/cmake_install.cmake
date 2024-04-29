# Install script for directory: D:/graphics/tutorials

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/graphics/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_camera/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_cpp101/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_hello_glsl/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_lighting/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_matrix/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_mesh/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_opengl/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_shader_programming/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_texture/cmake_install.cmake")
  include("D:/graphics/out/build/x64-Debug/tutorials/tutorial_transform/cmake_install.cmake")

endif()

