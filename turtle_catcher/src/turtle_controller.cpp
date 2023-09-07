#include "turtle_catcher/turtle_controller.hpp"

namespace turtle_catcher
{
    TurtleController::TurtleController() : Node("turle_controller_node")
    {
        m_aliveTurleSubscriber = this->create_subscription<turtle_interface::msg::Turtles>("alive_turtles",
                                                                                           10, std::bind(&TurtleController::CallbackAliveTurtles, this, std::placeholders::_1));
        m_cmdVelPublisher = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

        m_turtlePoseSubscriber = this->create_subscription<turtlesim::msg::Pose>("turtle1/pose", 10,
                                                                                 std::bind(&TurtleController::CallbackTurtlePose, this, std::placeholders::_1));

        m_timer = this->create_wall_timer(std::chrono::milliseconds(10), std::bind(&TurtleController::ControlLoop, this));

        this->declare_parameter("distance_threshold", 0.5);
        m_distanceThreshold = this->get_parameter("distance_threshold").as_double();
        this->declare_parameter("catch_closest_turtle", true);

        m_catchClosestTurtle = this->get_parameter("catch_closest_turtle").as_bool();
    }

    void TurtleController::CallbackTurtlePose(const turtlesim::msg::Pose::SharedPtr msg)
    {
        m_masterTurtlePose = *msg.get();
    }

    void TurtleController::CallbackAliveTurtles(const turtle_interface::msg::Turtles::SharedPtr alive_turtles)
    {
        m_aliveTurtles = alive_turtles->turtles;

        if (m_aliveTurtles.empty())
        {
            return;
        }

        if (m_catchClosestTurtle)
        {
            auto minDistance = std::numeric_limits<double>::max();
            turtle_interface::msg::Turtle closestTurtle{};

            for (const auto &turtle : m_aliveTurtles)
            {
                const auto &[_, __, distance] = GetPoseDifference(turtle);
                if (distance < minDistance)
                {
                    closestTurtle = turtle;
                    minDistance = distance;
                }
            }
            m_turtleToCatch = closestTurtle;
        }
        else
        {
            m_turtleToCatch = m_aliveTurtles[0];
        }
    }

    void TurtleController::CatchTurtle(const std::string turtleName)
    {
        auto client = this->create_client<turtle_interface::srv::CatchTurtle>("catch_turtle");
        while (!client->wait_for_service(std::chrono::seconds(1)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for service server to be up!!");
        }

        auto request = std::make_shared<turtle_interface::srv::CatchTurtle::Request>();
        request->name = turtleName;

        auto future = client->async_send_request(request);
        try
        {
            auto response = future.get();
            if (response->success)
            {
                RCLCPP_INFO(this->get_logger(), "Caught a turtle with name =  %s", turtleName.c_str());
            }
            else
            {
                RCLCPP_ERROR(this->get_logger(), "Failed to catch a turtle with name =  %s", turtleName.c_str());
            }
        }
        catch (const std::exception &e)
        {
            RCLCPP_ERROR(this->get_logger(), "Service call failed!");
        }
    }

    void TurtleController::ControlLoop()
    {
        if (m_aliveTurtles.empty())
        {
            return;
        }

        const auto &[errorX, errorY, distance] = GetPoseDifference(m_turtleToCatch);

        auto linearVelocity = 0.0;
        auto angularVelocity = 0.0;

        if (distance < m_distanceThreshold)
        {
            // caught the turtle
            m_catchThreads.push_back(std::thread(std::bind(&TurtleController::CatchTurtle, this, m_turtleToCatch.name)));
        }
        else
        {
            double steeringTheta = std::atan2(errorY, errorX);
            double goalDiff = steeringTheta - m_masterTurtlePose.theta;

            // normalize theta
            if (goalDiff > M_PI)
            {
                goalDiff -= 2 * M_PI;
            }
            else if (goalDiff < -M_PI)
            {
                goalDiff += 2 * M_PI;
            }

            linearVelocity = 2 * distance;
            angularVelocity = 6 * goalDiff;
        }

        PublishCommandVelocity(linearVelocity, angularVelocity);
    }

    std::tuple<double, double, double> TurtleController::GetPoseDifference(const turtle_interface::msg::Turtle turtle)
    {
        double errorX = turtle.x - m_masterTurtlePose.x;
        double errorY = turtle.y - m_masterTurtlePose.y;
        double distance = std::hypot(errorX, errorY);
        return std::make_tuple(errorX, errorY, distance);
    }

    void TurtleController::PublishCommandVelocity(const double linearVelocity, const double angularVelocity)
    {
        auto cmdVelocityMessage = geometry_msgs::msg::Twist();
        cmdVelocityMessage.linear.x = linearVelocity;
        cmdVelocityMessage.angular.z = angularVelocity;
        m_cmdVelPublisher->publish(cmdVelocityMessage);
    }
}