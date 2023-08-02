#include "rclcpp/rclcpp.hpp"
#include "turtle_catcher/turtle_spawner.hpp"

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<turtle_catcher::TurtleSpawner>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}