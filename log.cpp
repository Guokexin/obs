#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
CLog::CLog()
{

}

CLog::CLog(const char *name)
{
    Init(name);
}

void CLog::Init(const char *name)
{
	if(strcmp(name,"root") != 0)
	{
	//	std::cout<<name<<std::endl;
	    m_log = log4cxx::Logger::getLogger(name);
	}
	else
	{
	//	std::cout<<name<<std::endl;
		m_log = log4cxx::Logger::getRootLogger();
	}
}

void CLog::Log(const char *fmt, ...)
{
    int n;
    int size = 1024*2;
    char *p, *np;
    va_list ap;

    if ((p = (char*)malloc(size)) == NULL)
        return ;

    while (1)
    {

        va_start(ap, fmt);
        n = vsnprintf(p, size, fmt, ap);
        va_end(ap);

        if (n > -1 && n < size)
        {
            Log(std::string(p));
			free(p);
            return ;
        }

        if (n > -1)
            size = n+1;
        else
            size *= 2;

        if ((np = (char*)realloc (p, size)) == NULL)
        {
            free(p);
            return;
        }
        else
        {
            p = np;
        }
    }

}

void CLog::Log(const std::string &str)
{
    LOG4CXX_INFO(m_log, str.c_str());
}
