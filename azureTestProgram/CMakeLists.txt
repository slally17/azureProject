# DON'T USE

cmake_minimum_required(VERSION 3.15)
project(azureKinectProgram CXX)

add_executable(testMain main.cpp)
set_property(TARGET testMain PROPERTY CXX_STANDARD 11)
set_property(TARGET testMain PROPERTY VS_PACKAGE_REFERENCES "Microsoft.Azure.Kinect.Sensor_1.4.1;Microsoft.Azure.Kinect.BodyTracking_1.0.1;Microsoft.Azure.Kinect.BodyTracking.Dependencies.cuDNN_0.9.1;Microsoft.Azure.Kinect.BodyTracking.Dependencies_0.9.1;")

enable_testing()