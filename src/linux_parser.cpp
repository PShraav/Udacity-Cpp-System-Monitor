#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  filestream.close();
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// System memory utilization
float LinuxParser::MemoryUtilization() { 
  float memTotal = 1.0, memFree = 1.0, value;
  float memory;
  string line, key;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memTotal = value;}
        if (key == "MemFree") {
          memFree = value;}
          break;
      }
    }
  
  memory = (memTotal - memFree)/memTotal;
  }
  stream.close();
  return memory; 
}

// System uptime
long LinuxParser::UpTime() { 
  long upTime, upTimeIdle;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> upTimeIdle;
  }
  stream.close();
  return upTime; 
}

// Number of jiffies for the system
long LinuxParser::Jiffies() { 
  return UpTime() * sysconf(_SC_CLK_TCK); 
}

// Number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
   string line, value;
   long activeJiffers = 0.0;
   std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
   if(stream.is_open()){
     while(std::getline(stream, line)){
     std::istringstream linestream(line);
     for(int i = 0; i<17; i++){
       linestream >> value;
       if(i>=13){
         activeJiffers += std::stol(value);
       }
     }
   }
   }
   stream.close();
  return activeJiffers; 
}

// Number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long activeJiffers;
  auto cpuVector = LinuxParser::CpuUtilization();
  vector<int> cpuElement{CPUStates::kUser_, CPUStates::kNice_, CPUStates::kSystem_, CPUStates::kIRQ_, CPUStates::kSoftIRQ_,
                          CPUStates::kSteal_, CPUStates::kGuest_, CPUStates::kGuestNice_};
  for(int i: cpuElement){
    activeJiffers += std::stol(cpuVector[i]);
  }
  return activeJiffers; 
}

// Number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idleJiffies = 0;
  auto cpuVector = LinuxParser::CpuUtilization();
  vector<int> cpuElement{CPUStates::kIdle_, CPUStates::kIOwait_};
  for(int i: cpuElement){
    idleJiffies += std::stol(cpuVector[i]);
  }
  return idleJiffies; 
}

// CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpuUtilization;
  string line, cpu, cpuData;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while(linestream >> cpuData){
      cpuUtilization.push_back(cpuData);
    }
  }
  stream.close();
  return cpuUtilization; 
}

// Total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key, value;
  int totalProcesses = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
    std::istringstream linestream(line);
     while(linestream >> key >> value){
       if(key == "processes"){
        totalProcesses = std::stoi(value);
        break;}
      }
    }
  }
  stream.close();
  return totalProcesses; 
}  

// Number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key, value;
  int runningProcesses = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
    std::istringstream linestream(line);
     while(linestream >> key >> value){
       if(key == "procs_running"){
        runningProcesses = std::stoi(value);
        break;}
      }
    }
  }
  stream.close();
  return runningProcesses; 
}

// Command associated with a process
string LinuxParser::Command(int pid) { 
  string command, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
     while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::istringstream linestream(line);
      linestream >> command;
      std::replace(command.begin(), command.end(), '_', ' ');
    }
  }
  stream.close();
  return command; 
}

// Memory used by a process
string LinuxParser::Ram(int pid) { 
  string ram, line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
   while (std::getline(stream, line)) {
    std::istringstream linestream(line);
    while(linestream >> key >> value){
      if(key == "VmRSS:"){
        ram = std::to_string(std::stol(value)/1024);
        break;
      }
    }
   }
  }
  stream.close();
  return ram; 
}

// User ID associated with a process
string LinuxParser::Uid(int pid) { 
  string uid, line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
   while (std::getline(stream, line)) {
    std::istringstream linestream(line);
    while(linestream >> key >> value){
      if(key == "Uid:"){
        uid = value;
        break;
      }
    }
   }
  }
  stream.close();
  return uid; 
}

// User associated with a process
string LinuxParser::User(int pid) { 
  string user, line, userName, uid, x;
  string processUid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
   if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), '-', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> userName >> x >> uid){
        if(uid == processUid){
          user = userName;
          break;
        }
      }
    }
  }
  stream.close();
  return user; 
}

// Uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line, value;
  long upTime;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      for(int i = 0; i<22; i++){
        linestream >> value;
        if(i == 21){
         upTime = std::stol(value);
        }
      }
    }
  }
  stream.close();
  return (LinuxParser::UpTime() - (upTime/sysconf(_SC_CLK_TCK))); 
}

// CPU utilization of the process
float LinuxParser::CpuUtilization(int pid){
  float cpuUsage;
  float systemActiveJiffies = LinuxParser::ActiveJiffies();
  float systemIdleJiffies = LinuxParser::IdleJiffies();
  
  cpuUsage = (LinuxParser::ActiveJiffies(pid) / (systemActiveJiffies + systemIdleJiffies));
    
  return cpuUsage; 
}
