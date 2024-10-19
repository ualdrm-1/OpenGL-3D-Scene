   cmake_minimum_required(VERSION 3.5.0)
   project(opengl_project VERSION 0.1.0)

   cmake_policy(SET CMP0072 NEW)

   find_package(OpenGL REQUIRED)
   find_package(GLEW REQUIRED)
   find_package(glfw3 REQUIRED)
   
   add_executable(kr main.cpp)

   target_link_libraries(kr
      GLEW::GLEW
      glfw
      OpenGL::GL
   )