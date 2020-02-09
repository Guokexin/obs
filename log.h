#ifndef LOG_H
#define LOG_H

#include <log4cxx/logger.h>
#include <log4cxx/logstring.h>
#include <log4cxx/propertyconfigurator.h>
#include <iostream>
#include <string>

class CLog
{
public:
    CLog();
    CLog(const char *name);

public:
    void Init(const char *name);
    void Log(const char *fmt, ...);
    void Log(const std::string &str);
private:
    log4cxx::LoggerPtr m_log;
};


#endif
