#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>


class SerialTransmitter : public rclcpp::Node {

public:
    SerialTransmitter() : rclcpp::Node("serial_transmitter_node") , running_(true) {
        declare_parameter<std::string>("port", "/dev/ttyACM0");
        std::string port_ = get_parameter("port").as_string();

        sub_ = create_subscription<std_msgs::msg::String>(
            "serial_transmitter",
             10, 
             std::bind(&SerialTransmitter::msgCallback, this , std::placeholders::_1)
        );
        pub_ = create_publisher<std_msgs::msg::String>("serial_receiver", 10);

        arduino_.Open(port_);
        arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_9600);
        read_thread_ = std::thread(&SerialTransmitter::readLoop, this);
        RCLCPP_INFO(this->get_logger(), "Serial transmitter node started");
    };

    ~SerialTransmitter() {
        arduino_.Close();
    }

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    LibSerial::SerialPort arduino_;
    std::thread read_thread_;
    std::atomic<bool> running_;

    void msgCallback(const std_msgs::msg::String &msg) {
        RCLCPP_INFO_STREAM(this->get_logger(), "New message received, publishing on serial port: " << msg.data);
        if (arduino_.IsOpen() == false) {
            RCLCPP_ERROR(this->get_logger(), "Serial port is not open");
            return;
        }
        arduino_.Write(msg.data+'\n');
    }

    void readLoop() {
        std::string buffer;
        while (running_) {
            if (arduino_.IsDataAvailable()) {
                char c;
                arduino_.ReadByte(c, 100);
                if (c == '\n') {
                    // pełna wiadomość
                    auto msg = std_msgs::msg::String();
                    msg.data = buffer;
                    pub_->publish(msg);
                    buffer.clear();
                } else {
                    buffer += c;
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
};

int main (int argc, char** argv) {

    rclcpp::init(argc, argv);
    auto node = std::make_shared<SerialTransmitter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}