# Turtle Catcher - Turtle Controller


This package provides a Turtle Catcher node that catches a turtle in a simulated environment using ROS2 (Robot Operating System 2).

## Overview

The Turtle Controller node in the "turtle_catcher" package is responsible for catching a turtle in a simulated environment. It monitors the distance between the master turtle and the target turtle and calculates the command velocity required to catch the target turtle.
# Turtle Catcher - Turtle Spawner

## Overview

The Turtle Spawner node in the "turtle_catcher" package is responsible for spawning turtles at a specified frequency. It also handles the catch turtle service request, calls the kill service, and updates the list of alive turtles.

## Requirements

- ROS2 (humble or later)
- turtlesim package

## Installation

Clone this repository into your ROS2 workspace and build it using colcon:

```bash
cd ~/ros2_ws/src
git clone https://github.com/Keshavraj024/ros2_turtle_catcher.git
cd ~/ros2_ws
colcon build
source install/setup.bash
```

## Usage

Run the Turtle Catcher node:

```bash
ros2 launch turtle_bringup turtle_catcher.launch.py
```


