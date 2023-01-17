#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <fstream>
#include <streambuf>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

float LinuxParser::CpuUtilization() {
  string line, key;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest,
      guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal >> guest >> guest_nice) {
        if (key == "cpu") {
          float total_idle = std::stol(idle) + std::stol(iowait);
          float non_idle = std::stol(user) + std::stol(nice) +
                           std::stol(system) + std::stol(softirq) +
                           std::stol(irq) + std::stol(softirq) +
                           std::stol(steal);
          return non_idle / (total_idle + non_idle);
        }
      }
    }
  }
  return 0.;
}

float LinuxParser::CpuUtilization(int pid) {
  long system_up_time = UpTime();
  string line, el;
  long utime, stime, starttime = 0.;
  float cpu_usage, total_time, start_sec, elapsed_seconds = 0.;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> el) {
      switch (i) {
        case 13:
          utime = std::stol(el);
          break;
        case 14:
          stime = std::stol(el);
          break;
        case 21:
          starttime = std::stol(el);
          break;
        default:
          break;
      }
      i++;
    }
    total_time = (utime + stime) / sysconf(_SC_CLK_TCK);
    start_sec = starttime / sysconf(_SC_CLK_TCK);
    elapsed_seconds = system_up_time - start_sec;
    cpu_usage = total_time / elapsed_seconds;
  }
  return cpu_usage;
}

float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float total_memory, free_memory = 0.;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:")
          total_memory = std::stof(value);
        else if (key == "MemFree:")
          free_memory = std::stof(value);
      }
    }
  }
  return 1. - free_memory / total_memory;
}

int LinuxParser::TotalProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

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

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

long LinuxParser::UpTime() {
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return std::stol(uptime);
}

string LinuxParser::Command(int pid) {
  string cmd;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmd);
  }
  return cmd;
}

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

string LinuxParser::Ram(int pid) {
  string line, key, value, ram;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          ram = std::to_string(std::stol(value) / 1024);
          return ram;
        }
      }
    }
  }
  return ram;
}

string LinuxParser::Uid(int pid) {
  string line, key, value, uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = value;
          return uid;
        }
      }
    }
  }
  return uid;
}

string LinuxParser::User(string uid_in) {
  string line, x, user_name, uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user_name >> x >> uid) {
        if (uid == uid_in) {
          return user_name;
        }
      }
    }
  }
  return user_name;
}

long LinuxParser::UpTime(int pid) {
  long system_up_time = UpTime();
  string line, el;
  long process_start, up_time = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> el) {
      if (i == 21) {
        process_start = std::stol(el) / sysconf(_SC_CLK_TCK);
        up_time = system_up_time - process_start;
        return up_time;
      }
      i++;
    }
  }
  return 0;
}
