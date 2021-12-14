#include <string>
#include <cmath>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours = seconds / pow(60, 2);
    int minutes = (seconds - (hours * pow(60, 2)))/ 60;
    int second = seconds - ((hours * pow(60, 2)) + (minutes * 60));
    
    char time_string[20];
    sprintf(time_string, "%02d:%02d:%02d", hours, minutes, second);

    return time_string; 
}