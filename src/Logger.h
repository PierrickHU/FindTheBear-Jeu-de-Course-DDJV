#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

class Logger
{
public:
   Logger();
   /* virtual */ ~Logger();

   void Init(const std::string& _logPath, const std::string& _logFileName);
   void Log(const std::string& _data, bool _bAutoNewLine = true);

protected:

   std::ofstream mDebugFile;
};

#endif // !LOGGER_H