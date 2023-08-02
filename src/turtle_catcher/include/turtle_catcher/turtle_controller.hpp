/**
 * @file turtle_catcher/turtle_controller.hpp
 * @brief Catches a turtle
 */
#ifndef TURTLE_CATCHER__TURTLE_CONTROLLER_HPP_
#define TURTLE_CATCHER__TURTLE_CONTROLLER_HPP_

#include "rclcpp/rclcpp.hpp"
#include "turtle_interface/msg/turtles.hpp"
#include "turtle_interface/msg/turtle.hpp"
#include "turtle_interface/srv/catch_turtle.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <random>
#include <cmath>

namespace turtle_catcher
{
    class TurtleController : public rclcpp::Node
    {
    public:
        TurtleController();

    private:
        /**
         * @brief Callback function to get the pose of the master turtle
         * @param msg message containing the pose of the turtle 
        */
        void CallbackTurtlePose(const turtlesim::msg::Pose::SharedPtr msg);

        /**
         * @brief Callback function to get the information about the alive turtles
         * @param alive_turtles list of alive turtles
        */
        void CallbackAliveTurtles(const turtle_interface::msg::Turtles::SharedPtr alive_turtles);

        /**
         * @brief Fucntion which calls the catch turtle service
         * @param turtleName name of the turtle to catch
        */
        void CatchTurtle(const std::string turtleName);

        /**
         * @brief Monitors the distance between the master turtle and the target turtle and calculates the command velocity
         */
        void ControlLoop();

        /**
         * @brief Calculates the linear difference between the two poses
         * @return std::tuple<double, double, double> difference in x, y and the hypot distance
         */
        std::tuple<double, double, double> GetPoseDifference(const turtle_interface::msg::Turtle turtle);

        /**
         * @brief Publishes teh command velocity for the turtle bot
         * @param linearVelocity linear velocity in x direction
         * @param angularVelocity angular velocity in z direction
         */
        void PublishCommandVelocity(const double linearVelocity, const double angularVelocity);

        std::vector<turtle_interface::msg::Turtle> m_aliveTurtles;                              /**< List of alive turtles*/
        rclcpp::Subscription<turtle_interface::msg::Turtles>::SharedPtr m_aliveTurleSubscriber; /**< Subscribes to the alive turtles information*/
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr m_turtlePoseSubscriber;           /**< Subscribes to the master turtle's pose*/
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr m_cmdVelPublisher;              /**< Publishes the command veloctiy */
        turtle_interface::msg::Turtle m_turtleToCatch;                                          /**< Information abt the turtle to catch*/
        turtlesim::msg::Pose m_masterTurtlePose;                                                /**< Master turtle pose*/
        rclcpp::TimerBase::SharedPtr m_timer;                                                   /**< Calls the control loop to monitor the error*/
        std::vector<std::thread> m_catchThreads;                                                /**< Threads calling catch turtle service*/
        bool m_catchClosestTurtle;                                                              /**< Flag deciding wheter to catch te closest turtle*/
        double m_distanceThreshold;                                                             /**< Minimum distance between master turtle and the turtle to catch*/
    };

} /* namespace turtle_catcher */

#endif /* TURTLE_CATCHER__TURTLE_CONTROLLER_HPP_ */