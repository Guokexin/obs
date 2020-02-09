#ifndef _LICENSE_H_
#define _LICENSE_H_

#include "StdString.h"
#include "owntype.h"
#include "list"
class CLicense
{
public:
	CLicense(){};
	~CLicense(){};

public:
	CStdString StringEncrypt(CStdString chstr,BYTE Key);
	CStdString StringDecrypt(CStdString chstr,BYTE Key);

	void FindAllDevice(std::list<CStdString> &ls);


};
#endif
