#include <string>
#include <cmath>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours = seconds / 3600;
    seconds = seconds % 3600;
    int minutes = seconds / 60;
    int second = seconds % 60;
    
    char timeString[20];
    sprintf(timeString, "%02d:%02d:%02d", hours, minutes, second);

    return timeString; 
}