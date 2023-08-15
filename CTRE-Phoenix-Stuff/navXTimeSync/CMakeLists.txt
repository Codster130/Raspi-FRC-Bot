add_definitions(-std=c++11 -Wall -Wextra -Wno-switch)

add_library(
	navXTimeSync
	AHRS.cpp
	AHRS.h
	AHRSProtocol.h
	ContinuousAngleTracker.cpp
	ContinuousAngleTracker.h
	IBoardCapabilities.h
	IIOCompleteNotification.h
	IIOProvider.h
	IMUProtocol.h
	IMURegisters.h
	InertialDataIntegrator.cpp
	InertialDataIntegrator.h
	ITimestampedDataSubscriber.h
	OffsetTracker.cpp
	OffsetTracker.h
	SerialIO.cpp
	SerialIO.h
	SerialPort.cpp
)