#include <vector>
#include <string>

using std::string;

#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    
    /*
    Algorithm to calculate:

    Idle = idle + iowait;
    NonIdle = user + nice + system + irq + softirq + steal;
    Total = Idle + NonIdle;

    CPU utilization is - NonIdle/Total
    */


    //NonIdle = user + nice + system + irq + softirq + steal
    float NonIdle = LinuxParser::ActiveJiffies();
    //Idle = idle + iowait
    float Idle = LinuxParser::IdleJiffies();

    if((Idle + NonIdle) == 0){
        return 0.0;}
    else{
        return (NonIdle/(Idle + NonIdle));}
}