cmake_minimum_required(VERSION 3.8)
project(turtle_catcher)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(turtlesim REQUIRED)
find_package(turtle_interface REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(turtle_spawner 
  src/spawner_main.cpp 
  src/turtle_spawner.cpp
)

target_include_directories(turtle_spawner PRIVATE include)
ament_target_dependencies(turtle_spawner rclcpp turtlesim turtle_interface)

add_executable(turtle_controller 
  src/controller_main.cpp 
  src/turtle_controller.cpp
)

target_include_directories(turtle_controller PRIVATE include)
ament_target_dependencies(turtle_controller rclcpp turtlesim turtle_interface geometry_msgs)

install(TARGETS
  turtle_spawner
  turtle_controller
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
