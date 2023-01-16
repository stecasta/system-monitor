#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, float cpu_usage)
    : pid_(pid), cpu_utilization_(cpu_usage){};

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  cpu_utilization_ = LinuxParser::CpuUtilization(pid_);
  return cpu_utilization_;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() {
  ram_ = LinuxParser::Ram(pid_);
  return ram_;
}

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(LinuxParser::Uid(pid_)); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return cpu_utilization_ > a.cpu_utilization_;
}