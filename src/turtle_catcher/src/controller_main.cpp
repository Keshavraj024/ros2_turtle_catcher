#include "rclcpp/rclcpp.hpp"
#include "turtle_catcher/turtle_controller.hpp"

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<turtle_catcher::TurtleController>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}