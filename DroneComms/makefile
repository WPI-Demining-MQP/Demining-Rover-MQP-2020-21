CXX = g++
CXXFLAGS = -Wall
main: main.o DroneComms.o PathPlan.o
	$(CXX) $(CXXFLAGS) -o main main.o DroneComms.o PathPlan.o
main.o: main.cpp DroneComms.h PathPlan.h
	$(CXX) $(CXXFLAGS) -c main.cpp
DroneComms.o: DroneComms.h
PathPlan.o: PathPlan.h
