#include "stdafx.h"

#include "Logger.h"

#include <ctime>
#include <chrono>
#include <iomanip> // put_times
#include <sstream>
#include <string>

Logger::Logger()
{

}

/*virtual*/ Logger::~Logger()
{
   mDebugFile.close();
}

void Logger::Init(const std::string& _logPath, const std::string& _logFileName)
{
   const std::string logFileName = _logPath + "\\" + _logFileName;

   mDebugFile = std::ofstream(logFileName);

   auto now = std::chrono::system_clock::now();
   auto in_time_t = std::chrono::system_clock::to_time_t(now);

   struct tm time_info;
   localtime_s(&time_info, &in_time_t);

   std::stringstream ss;
   ss << std::put_time(&time_info, "%Y-%m-%d %X") << std::endl;

   Log(ss.str());
}

void Logger::Log(const std::string& _data, bool _bAutoNewLine)
{
   mDebugFile << _data;

   if (_bAutoNewLine)
   {
      mDebugFile << std::endl;
   }

   mDebugFile.flush();
}