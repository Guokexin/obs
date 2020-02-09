#ifndef _PUB_H_
#define _PUB_H_

#include "StdString.h"

class OBSMsg
{
public:
	OBSMsg(){fd = -1;sMsg="";}
	~OBSMsg(){};
	int fd;
	CStdString sMsg;
           
        //std::list<string>  phonelist;
        //std::string addr;
        //std::string warn_type;
};
#endif
