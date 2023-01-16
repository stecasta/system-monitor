#include "format.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>

using namespace std::chrono_literals;
using std::string;

// DONE: Complete this helper function
// Using chrono library and std::put_time to format time
string Format::ElapsedTime(long seconds) {
  std::chrono::system_clock::time_point time;
  std::chrono::milliseconds mil(seconds * 1000);
  std::time_t now_time = std::chrono::system_clock::to_time_t(time);
  time -= std::chrono::duration_cast<std::chrono::microseconds>(mil);
  std::time_t start_time = std::chrono::system_clock::to_time_t(time);
  std::time_t duration = now_time - start_time;
  std::stringstream time_str;
  time_str << std::put_time(std::localtime(&duration), "%H:%M:%S");
  return time_str.str();
}
