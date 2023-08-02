/**
 * @file turtle_catcher/turtle_spawner.hpp
 * @brief Spawns a turtle at a given frequency
 */
#ifndef TURTLE_CATCHER__TURTLE_SPAWNER_HPP_
#define TURTLE_CATCHER__TURTLE_SPAWNER_HPP_

#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/spawn.hpp"
#include "turtlesim/srv/kill.hpp"
#include "turtle_interface/msg/turtles.hpp"
#include "turtle_interface/msg/turtle.hpp"
#include "turtle_interface/srv/catch_turtle.hpp"
#include <random>

namespace turtle_catcher
{
    class TurtleSpawner : public rclcpp::Node
    {
    public:
        TurtleSpawner();

    private:
        /**
         * Calls the spawn turtle service at a specified frequency 
        */
        void TimerCallBack();

        /**
         * Handles the catch turtle service request, calls the kill service and updated the alive turtle vector
         * @param request service request turtle name
         * @param response service response
        */
        void CatchTurtleCallback(const turtle_interface::srv::CatchTurtle::Request::SharedPtr request,
                                 const turtle_interface::srv::CatchTurtle::Response::SharedPtr response);

        /**
         * Publishes the information of the alive turtles
         */
        void PublishAliveTurtles();

        /**
         * @brief Spawns a new turtle at a random pose
         */
        void SpawnTurtle();

        /**
         * @brief Call the kill service and removes the turtlefrom the list of alive turtles
         * @param turtleName turtle to be killed
         */
        void KillTurtle(const std::string turtleName);

        rclcpp::Service<turtle_interface::srv::CatchTurtle>::SharedPtr m_catchTurtleServer;  /**< Server to handle catch turtle service*/
        rclcpp::Publisher<turtle_interface::msg::Turtles>::SharedPtr m_aliveTurtlePublisher; /**< Publishes the alive turtles information*/
        double m_spawnFrequency;                                                             /**< Frequency at which the turtles are spawned*/
        std::vector<turtle_interface::msg::Turtle> m_aliveTurtles;                           /**< List of alive turtles*/
        rclcpp::TimerBase::SharedPtr m_timer;                                                /**< Calls the spawn turtle service*/
        std::vector<std::thread> m_spawnThreads;                                             /**< Threads calling spawn turtle service*/
        std::vector<std::thread> m_killThreads;                                              /**< Threads calling kill turtle service*/
    };

} /* namespace turtle_catcher */

#endif /* TURTLE_CATCHER__TURTLE_SPAWNER_HPP_ */