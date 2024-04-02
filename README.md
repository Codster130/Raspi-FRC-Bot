# Raspi-FRC-Bot

This project is intended to take FRC Hardware (more specifically CAN devices from Cross the Road Electronics) and create a GitHub repository that is more or less plug-and-play for demo robots and teaching new team members how to program a robot. This project intends to remove all of the super specialty hardware that restricts how quickly one can turn on and connect to a robot and remove all costly hardware such as the RoboRIO.

This project also eliminates the need for spending time connecting to and making sure all of the FRC measures are in place for things to work. By removing the RoboRIO we are eliminating the need for a laptop and radio. This also allows the user to have the peace of mind that no matter where they decide to set up, they will have a robust and safe connection to the robot without any wifi issues.

At the same time, the Raspberry Pi is used as a hotspot for users to connect to change code no matter the place or time and allows for connection to view things and show how devices work, such as the Limelight and PhotonVision.

The list of capabilities of this project includes:
Using CTRE’s Phoenix libraries to control and CANFD device, using C++, and learning programming that can be easily transferred over to C++ for a competition bot
Using a Protocol known as SBUS to allow the use of Traditional RC controllers like the FrSky Taranis X9D
Make use of a Raspberry Pi 3 and use it to drive a robot with a SocketCAN device, in theory, this capability should be able to be used with a Jetson Nano.
Use Vision processing with a coprocessor (like on a RoboRIO) on something like a Limelight or separate Raspberry Pi running PhotonVision.

Thanks to CTRE for their help debugging, the original project can be found here: https://github.com/CrossTheRoadElec/Phoenix5-Linux-Example

 
ALL DOCUMENTATION CAN BE FOUND HERE https://docs.google.com/document/d/1tkJT5t-uBpuLZOYx7LnefO_a3mP3kBmnRmVOSr5K3BI/edit?usp=sharing
