# SCARA-ROS2

## TODO: Prerequisites - OpenCV

## Prerequisites - libcamera & rpicam
### libcamera
- Do not install libcamera* packages from apt system
- Install the following packages before building libcamera

        sudo apt install -y build-essential
        sudo apt install -y libboost-dev
        sudo apt install -y libgnutls28-dev openssl libtiff-dev pybind11-dev
        sudo apt install -y qtbase5-dev libqt5core5a libqt5widgets5t64
        sudo apt install -y meson cmake
        sudo apt install -y python3-yaml python3-ply
        sudo apt install -y libglib2.0-dev libgstreamer-plugins-base1.0-dev

- clone [libcamera repo](https://github.com/raspberrypi/libcamera) to local, navigate to and build with:

    >   meson setup build --buildtype=release -Dpipelines=rpi/vc4,rpi/pisp -Dipas=rpi/vc4,rpi/pisp -Dv4l2=true -Dgstreamer=disabled -Dtest=false -Dlc-compliance=disabled -Dcam=enabled -Dqcam=disabled -Ddocumentation=disabled -Dpycamera=enabled
- install with:

        ninja -C build
        sudo ninja -C build install

- LD_PRELOAD to point to libraries installed by ninja build system

        export LD_PRELOAD=/usr/local/lib/aarch64-linux-gnu/libcamera.so.0.4:/usr/local/lib/aarch64-linux-gnu/libcamera-base.so.0.4:/usr/local/lib/aarch64-linux-gnu/libpisp.so.1

### rpicam
- Install the following packages before

        sudo apt install libavcodec-dev libavdevice-dev libavformat-dev libswresample-dev
        sudo apt install -y cmake libboost-program-options-dev libdrm-dev libexif-dev
        sudo apt install -y meson ninja-build
- clone [rpicam-apps repo](https://github.com/raspberrypi/rpicam-apps) to local, navigate to and build

    > meson setup build -Denable_libav=enabled -Denable_drm=enabled -Denable_egl=enabled -Denable_qt=enabled -Denable_opencv=disabled -Denable_tflite=disabled -Denable_hailo=disabled

    > meson compile -C build

    > sudo meson install -C build

    > sudo ldconfig