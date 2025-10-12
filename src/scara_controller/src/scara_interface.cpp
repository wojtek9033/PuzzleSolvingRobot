#include "scara_controller/scara_interface.hpp"
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <sstream>
#include <iomanip>

namespace scara_controller {

ScaraInterface::ScaraInterface() {

};

ScaraInterface::~ScaraInterface () {
    if (arduino_.IsOpen()) {
        try {
            arduino_.Close();
        } catch (...) {
            RCLCPP_FATAL_STREAM(this->get_logger(), "Something went wrong while closing the connection with port" << port_);
        }
    }
}

CallbackReturn ScaraInterface::on_init(const hardware_interface::HardwareInfo &hardware_info) {
    CallbackReturn result = hardware_interface::SystemInterface::on_init(hardware_info);

    if (result != CallbackReturn::SUCCESS)
        return result; 

    try {
        port_ = info_.hardware_parameters.at("port");
    } catch (const std::out_of_range &e) {
        RCLCPP_FATAL(this->get_logger(), "No serial port provided! Aborting...");
        return CallbackReturn::FAILURE;
    }

    position_states_.resize(info_.joints.size(), 0.0);
    position_commands_.resize(info_.joints.size(), 0.0);
    prev_position_commands_.resize(info_.joints.size(), 0.0);
    last_sent_position_.resize(info_.joints.size(), 0.0);

    return CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> ScaraInterface::export_state_interfaces() {
    std::vector<hardware_interface::StateInterface> state_interfaces;
    for (size_t i = 0; i < info_.joints.size(); i++) {
        state_interfaces.emplace_back(hardware_interface::StateInterface(info_.joints.at(i).name, hardware_interface::HW_IF_POSITION, &position_states_.at(i)));
    }

    return state_interfaces;
}

std::vector<hardware_interface::CommandInterface> ScaraInterface::export_command_interfaces() {
    std::vector<hardware_interface::CommandInterface> command_interfaces;
    for (size_t i = 0; i < info_.joints.size(); i++) {
        command_interfaces.emplace_back(hardware_interface::CommandInterface(info_.joints.at(i).name, hardware_interface::HW_IF_POSITION, &position_commands_.at(i)));
    }

    return command_interfaces;
}

CallbackReturn ScaraInterface::on_activate(const rclcpp_lifecycle::State &previous_state) {
    (void)previous_state;
    RCLCPP_INFO(this->get_logger(), "Starting the robot hardware...");
    std::fill(position_commands_.begin(), position_commands_.end(), 0.0);
    std::fill(prev_position_commands_.begin(), prev_position_commands_.end(), 0.0);
    std::fill(position_states_.begin(), position_states_.end(), 0.0);
    try {
        arduino_.Open(port_);
        arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_9600);
    } catch (...) {
        RCLCPP_FATAL_STREAM(this->get_logger(), "Something went wrong while interacting with the port " << port_);
    }

    RCLCPP_INFO(this->get_logger(), "Hardware started, ready to take commands");
    return CallbackReturn::SUCCESS;
}

CallbackReturn ScaraInterface::on_deactivate(const rclcpp_lifecycle::State &previous_state) {
    (void)previous_state;
    RCLCPP_INFO(this->get_logger(), "Stopping the robot hardware...");
    if (arduino_.IsOpen()) {
        try { 
            arduino_.Close();
        } catch (...) {
            RCLCPP_FATAL_STREAM(this->get_logger(), "Something went wrong while closing the connection with the port " << port_);
            return CallbackReturn::FAILURE;
        }
    }

    RCLCPP_INFO(this->get_logger(), "Hardware stopped");
    return CallbackReturn::SUCCESS;
}

hardware_interface::return_type ScaraInterface::read(const rclcpp::Time &time, const rclcpp::Duration &period) {
    (void)time;
    (void)period;
    std::string data;
    try {
        if(arduino_.IsDataAvailable()) {
        arduino_.ReadLine(data,'\n');
        data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
        data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
        }
    } catch (...) {
        RCLCPP_WARN(this->get_logger(), "Failed to read joints status from serial.");
        return hardware_interface::return_type::OK;
    }

    if(!data.empty()) {
        std::vector<std::string> tokens;
        std::stringstream ss(data);
        std::string token;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.at(0) == "1" && tokens.at(1) == "1" && tokens.at(2) == "1" && tokens.at(3) == "1") {
            position_states_ = position_commands_;
            RCLCPP_INFO(this->get_logger(), "Robot reached target position.");
        }
    }

    return hardware_interface::return_type::OK;
}

bool ScaraInterface::send_command_to_arduino(std::vector<double> position_commands) {
    double base = position_commands.at(0) * 1000.0; // meters to milimeters
    double shoulder = (position_commands.at(1) * 180.0)/ M_PI; 
    double elbow = (position_commands.at(2) * 180.0)/ M_PI;
    double wrist = (position_commands.at(3) * 180.0)/ M_PI;
    double gripper_state = position_commands.at(4);
    std::ostringstream oss;
    oss << std::fixed
        << std::setprecision(3) 
        << 'b'
        << base << ';'
        << 's'
        << shoulder << ';'
        << 'e'
        << elbow << ';'
        << 'w'
        << wrist << ';'
        << 'g'
        << gripper_state;

    std::string msg = oss.str();    
    RCLCPP_INFO_STREAM(this->get_logger(), "Sending command to arduino: " << msg);
    try {
        arduino_.Write(msg + '\n');
    } catch (...) {
        RCLCPP_ERROR_STREAM(this->get_logger(), "Something went wrong while sending the message " << msg << "to the port " << port_);
        return 1;
    }
    return 0;
}

hardware_interface::return_type ScaraInterface::write(const rclcpp::Time &time, const rclcpp::Duration &period) { 
    (void)time;
    (void)period;
    if (position_commands_ == prev_position_commands_)
        return hardware_interface::return_type::OK;

    bool new_command = false;
    for (size_t i = 0; i < position_commands_.size(); i++) {
        if (std::abs(position_commands_.at(i) - last_sent_position_[i]) > epsilon_) {
            new_command = true;
            break;
        }
    }

    if (new_command) {
        if (send_command_to_arduino(position_commands_)) {
            return hardware_interface::return_type::ERROR;
        }
        last_sent_position_ = position_commands_;
    }

    prev_position_commands_ = position_commands_;
    return hardware_interface::return_type::OK;
}

} // namespace scara_controller

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(scara_controller::ScaraInterface, hardware_interface::SystemInterface)