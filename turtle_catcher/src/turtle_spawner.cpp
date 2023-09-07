#include "turtle_catcher/turtle_spawner.hpp"
#include "turtle_catcher/utils.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

namespace turtle_catcher
{
    TurtleSpawner::TurtleSpawner() : Node("turtle_spawner_node")
    {
        this->declare_parameter("spawn_frequency", 1.0);
        m_spawnFrequency = this->get_parameter("spawn_frequency").as_double();
        m_timer = this->create_wall_timer(std::chrono::milliseconds((int)(1000.0 / m_spawnFrequency)),
                                          std::bind(&TurtleSpawner::TimerCallBack, this));
        m_aliveTurtlePublisher = this->create_publisher<turtle_interface::msg::Turtles>("alive_turtles", 10);
        m_catchTurtleServer = this->create_service<turtle_interface::srv::CatchTurtle>("catch_turtle",
                                                                                       std::bind(&TurtleSpawner::CatchTurtleCallback, this, _1, _2));
    }

    void TurtleSpawner::TimerCallBack()
    {
        m_spawnThreads.push_back(std::thread(std::bind(&TurtleSpawner::SpawnTurtle, this)));
    }

    void TurtleSpawner::CatchTurtleCallback(const turtle_interface::srv::CatchTurtle::Request::SharedPtr request,
                                            const turtle_interface::srv::CatchTurtle::Response::SharedPtr response)
    {
        m_killThreads.push_back(std::thread(std::bind(&TurtleSpawner::KillTurtle, this, request->name)));
        response->success = true;
    }

    void TurtleSpawner::PublishAliveTurtles()
    {
        auto message = turtle_interface::msg::Turtles();
        message.turtles = m_aliveTurtles;
        m_aliveTurtlePublisher->publish(message);
    }

    void TurtleSpawner::SpawnTurtle()
    {
        auto client = this->create_client<turtlesim::srv::Spawn>("spawn");

        while (!client->wait_for_service(std::chrono::seconds(1)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for spwan service server to be up!!");
        }

        auto request = std::make_shared<turtlesim::srv::Spawn::Request>();
        request->x = GenerateRandomNumber(0.0, 10.0);
        request->y = GenerateRandomNumber(0.0, 10.0);
        request->theta = GenerateRandomNumber(0.0, 2 * M_PI);

        auto future = client->async_send_request(request);
        try
        {
            auto response = future.get();

            if (response->name.empty())
            {
                return;
            }

            RCLCPP_INFO(this->get_logger(), "Created a turtle with name =  %s", response->name.c_str());
            auto turtle = turtle_interface::msg::Turtle();
            turtle.x = request->x;
            turtle.y = request->y;
            turtle.theta = request->theta;
            turtle.name = response->name;
            m_aliveTurtles.push_back(turtle);
            PublishAliveTurtles();
        }
        catch (const std::exception &e)
        {
            RCLCPP_ERROR(this->get_logger(), "Could not spawn a turtle!");
        }
    }

    void TurtleSpawner::KillTurtle(const std::string turtleName)
    {
        auto client = this->create_client<turtlesim::srv::Kill>("kill");
        while (!client->wait_for_service(std::chrono::seconds(1)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for kill service server to be up!!");
        }

        auto request = std::make_shared<turtlesim::srv::Kill::Request>();
        request->name = turtleName;

        auto future = client->async_send_request(request);

        try
        {
            std::ignore = future.get();
            auto it = std::find_if(m_aliveTurtles.begin(), m_aliveTurtles.end(), [&](const turtle_interface::msg::Turtle &turtle)
                                   { return turtle.name == turtleName; });

            // If the element is found, erase it from the vector
            if (it != m_aliveTurtles.end())
            {
                m_aliveTurtles.erase(it);
                PublishAliveTurtles();
            }
        }
        catch (const std::exception &e)
        {
            RCLCPP_ERROR(this->get_logger(), "Could not kill a turtle!");
        }
    }
}
