#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>

int main(int argc, char* argv[])
{
    // Initialize ROS and create the Node
    rclcpp::init(argc, argv);
    auto const node = std::make_shared<rclcpp::Node>(
        "hello_moveit", rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

    // Create a ROS logger
    auto const logger = rclcpp::get_logger("hello_moveit");

    // Create the MoveIt MoveGroup Interface
    using moveit::planning_interface::MoveGroupInterface;
    auto move_group_interface = MoveGroupInterface(node, "ur_manipulator");
    move_group_interface.setPlannerId("RRTConnectkConfigDefault");
    move_group_interface.setPoseReferenceFrame("base_link");
    move_group_interface.setStartStateToCurrentState();
    move_group_interface.setGoalTolerance(0.01);
    move_group_interface.setPlanningTime(10.0);
    move_group_interface.setNumPlanningAttempts(100);

    // Set a target Pose
    auto const target_pose = [] {
        geometry_msgs::msg::Pose msg;
        msg.position.x = 0.045382;
        msg.position.y = 0.27871;
        msg.position.z = 0.53807;
        msg.orientation.x = -0.70709;
        msg.orientation.y = 0.000;
        msg.orientation.z = 0.000;
        msg.orientation.w = 0.70707;
        return msg;
    }();
    move_group_interface.setPoseTarget(target_pose);

    // Create a plan to that target pose
    auto const [success, plan] = [&move_group_interface] {
        moveit::planning_interface::MoveGroupInterface::Plan msg;
        auto const ok = static_cast<bool>(move_group_interface.plan(msg));
        return std::make_pair(ok, msg);
    }();

    // Execute the plan
    if (success)
    {
        move_group_interface.execute(plan);
    }
    else
    {
        RCLCPP_ERROR(logger, "Planing failed!");
    }

    // Shutdown ROS
    rclcpp::shutdown();
    return 0;
}