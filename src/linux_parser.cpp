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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float MemTotal = 1.0, MemFree = 1.0, Value;
  float Memory;
  string line, key;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> Value) {
        if (key == "MemTotal") {
          MemTotal = Value;}
        if (key == "MemFree") {
          MemFree = Value;}
          break;
      }
    }
  
  Memory = (MemTotal - MemFree)/MemTotal;
  }
  return Memory; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long UpTime, UpTime_idle;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> UpTime >> UpTime_idle;
  }
  return UpTime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return UpTime() * sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
   string line, value;
   long active_jiffers = 0.0;
   std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
   if(stream.is_open()){
     std::getline(stream, line);
     std::istringstream linestream(line);
     for(int i = 0; i<17; i++){
       linestream >> value;
       if(i>=13){
         active_jiffers += std::stol(value);
       }
     }
   }
  
  return active_jiffers; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long active_jiffies;
  auto cpu_vector = LinuxParser::CpuUtilization();
  vector<int> cpu_element{CPUStates::kUser_, CPUStates::kNice_, CPUStates::kSystem_, CPUStates::kIRQ_, CPUStates::kSoftIRQ_,
                          CPUStates::kSteal_, CPUStates::kGuest_, CPUStates::kGuestNice_};
  for(int i: cpu_element){
    active_jiffies += std::stol(cpu_vector[i]);
  }
  return active_jiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idle_jiffies = 0;
  auto cpu_vector = LinuxParser::CpuUtilization();
  vector<int> cpu_element{CPUStates::kIdle_, CPUStates::kIOwait_};
  for(int i: cpu_element){
    idle_jiffies += std::stol(cpu_vector[i]);
  }
  return idle_jiffies; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpu_utilization;
  string line, cpu, cpu_data;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while(linestream >> cpu_data){
      cpu_utilization.push_back(cpu_data);
    }
  }
  return cpu_utilization; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key, value;
  int total_processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
    std::istringstream linestream(line);
     while(linestream >> key >> value){
       if(key == "processes"){
        total_processes = std::stoi(value);
        break;}
      }
    }
  }
  return total_processes; 
}  

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key, value;
  int running_processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
    std::istringstream linestream(line);
     while(linestream >> key >> value){
       if(key == "procs_running"){
        running_processes = std::stoi(value);
        break;}
      }
    }
  }
  return running_processes; 
}

// TODO: Read and return the command associated with a process
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
  return command; 
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string ram, line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
   while (std::getline(stream, line)) {
    std::istringstream linestream(line);
    while(linestream >> key >> value){
      if(key == "VmSize:"){
        ram = std::to_string(std::stol(value)/1024);
        break;
      }
    }
   }
  }
  return ram; 
}

// TODO: Read and return the user ID associated with a process
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
  return uid; 
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string user, line, user_name, uid, x;
  string process_uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
   if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), '-', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> user_name >> x >> uid){
        if(uid == process_uid){
          user = user_name;
          break;
        }
      }
    }
  }
  return user; 
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line, value;
  long up_time;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      for(int i = 0; i<22; i++){
        linestream >> value;
        if(i == 21){
         up_time = std::stol(value);
        }
      }
    }
  }
  return (up_time/sysconf(_SC_CLK_TCK)); 
}

// Read and return CPU utilization of the process
float LinuxParser::CpuUtilization(int pid){
  float cpu_usage, total_time = 0.0, seconds = 0.0;
    string line, value, utime, stime, cutime, cstime, starttime;
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if(stream.is_open()){
       std::getline(stream, line);
       std::istringstream linestream(line);
       for(int i = 0; i<22; i++){
           linestream >> value;
           if(i == 13){
               utime = value;}
           if(i == 14){
               stime = value;}
           if(i == 15){
               cutime = value;}
           if(i == 16){
               cstime = value;}
           if(i == 21){
               starttime = value;}
       }
        long upTime = LinuxParser::UpTime();
        total_time = std::stof(utime + stime + cutime + cstime);
        seconds = upTime - (std::stof(starttime)/sysconf(_SC_CLK_TCK));
        cpu_usage = 100 * ((total_time/sysconf(_SC_CLK_TCK))/seconds);
    }
    return cpu_usage; 
}
