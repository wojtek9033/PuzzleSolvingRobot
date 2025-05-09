#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>


class SerialTransmitter : public rclcpp::Node {

public:
    SerialTransmitter() : rclcpp::Node("serial_transmitter_node") {
        declare_parameter<std::string>("port", "/dev/ttyACM0");
        std::string port_ = get_parameter("port").as_string();

        sub_ = create_subscription<std_msgs::msg::String>(
            "serial_transmitter",
             10, 
             std::bind(&SerialTransmitter::msgCallback, this , std::placeholders::_1)
        );

        arduino_.Open(port_);
        arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);

    };

    ~SerialTransmitter() {
        arduino_.Close();
    }

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    LibSerial::SerialPort arduino_;

    void msgCallback(const std_msgs::msg::String &msg) {
        RCLCPP_INFO_STREAM(this->get_logger(), "New message received, publishing on serial port: " << msg.data);
        arduino_.Write(msg.data);
    }
};

int main (int argc, char** argv) {

    rclcpp::init(argc, argv);
    auto node = std::make_shared<SerialTransmitter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}