A robotic system to assemble jigsaw puzzles of variable size (currently, maximum tested size is 9 elements) . The robotic arm is a SCARA-like 4 DOF manipulator, and its model is based on the Open Source project "PyBot SCARA Robotic Arm" [Hackday IO](https://hackaday.io/project/175419-pybot-scara-robotic-arm-3d-printed-python). For the purpose of the goal task, the arm models were slighty adjusted. The most important change is the forearm, that is now adjusted to mount a suction cup and a Raspberry Pi HQ Camera. 

## Prerequisities
### Software
* **Ubuntu 24.04 LTS**
* **ROS2 Jazzy**
* **OpenCV**
* **Camera_ROS** [ROS 2 node for libcamera](https://github.com/christianrauch/camera_ros)

### Hardware
* **Raspberry Pi 5** – a platform for ROS2.
* **Arduino Uno** – Stepper Motors, Suction Cup and Valve control.
* **Raspberry Pi HQ Camera with lense relatively high focal length** – akwizycja obrazu puzzli.

## Packages overview
* **solver** – image processing package, it does all the image analysys and determines matching of elements. Provides the end data for the arm.
* **tuner** – a tool to adjust pre-processing parameters and corners detection.
* **bringup** – contains launchfile to run the whole system.
* **controller** – implements `hardware_interface::SystemInterface`, providing interface between Arduino and the Raspberry Pi.
* **description** – URDF/XACRO Files.
* **firmware** – Arduino code (not a ROS2 package!).
* **kinematics** – does the IK calculations. Provides separate topics for two effectors (Camera/Suction Cup).
* **msgs** – provides msgs definitions.
* **server** – action server to handle system workflow.

## Scara Task Server

The server provides three actions:

* **capture** – it moves the arm to predefined positions, to capture images of each element.
* **assemble** – requires the start&goal elements positions to be published first. The arm starts the assembly process based on this data.
* **calibrate** – similar to capture, but moves only to the first predefined position. Utilized by `tuner` node.

## Prerequisities

* **Ubuntu 24.04 LTS**
* **ROS2 Jazzy**
* **OpenCV**

## Installing
TBU.
## Run
TBU.
