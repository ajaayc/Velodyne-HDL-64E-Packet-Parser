# Velodyne HDL-64E LiDAR Calibration and Point Cloud Visualizer

I wrote this C++ utility to analyze packets of data emitted by the Velodyne HDL-64E LiDAR, stored in a .pcap file. A .pcap file of LiDAR data can be created by setting up a recording with Wireshark or its internal process, Dumpcap, while the Velodyne LiDAR is connected via an ethernet cable to the PC.

This utility is capable of reading the .pcap file and:
* parsing packets and outputting a .csv file containing the sensor's internal calibration parameters
* outputting frames containing X, Y, Z points and intensity values for each laser into a .csv file
* presenting a visualization of each data frame using VTK*

*The functionality of visualizing frames of points using VTK is slightly broken at the time of this writing. I recommend using the Python code from [github.com/ajaayc/Velodyne-VLP-16](https://github.com/ajaayc/Velodyne-VLP-16) to visualize frames of data generated from this utility.

At some point, I will clean up this code and write better documentation for how to use the utility. In the mean time, if you have any questions, feel free to write up an issue.

Preliminary build instructions:
```
mkdir build
cd build
cmake ..
make
```

More info about the packet structure of the LiDAR's output can be found in the [Velodyne HDL-64E Manual](https://www.velodynelidar.com/lidar/products/manual/HDL-64E%20Manual.pdf).