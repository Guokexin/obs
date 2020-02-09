#ifndef GENERALUTILS_H
#define GENERALUTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>

#include "owntype.h"

class  CGeneralUtils
{
public:
	CGeneralUtils();
	virtual ~CGeneralUtils();

private:
	UINT x_Find(const std::string &szInput, const std::string &szSpeciStr, int startafter = 0);	// 查找指定字符串

public:
	BOOL CheckValidString(const std::string &str);		// 检测字符串的有效性
	UINT SplitterString(const std::string &szInput,		// 按分隔符拆分字符串
		const std::string &szDelimiter,
		std::vector<std::string>& arrayElems);
	void BubbleSort(std::vector<std::string>& mysortArray, int count, bool ascend);
	BOOL GetStringValue(const std::string &szInString, const char* szKey, char* szValue);
	BOOL GetStringValue(const char* szInString, const char* szKey, int &nValue);
	BOOL GetStringValue(const char* szInString, const char* szKey, bool &bValue);
	BOOL GetKeyValue(const std::string &strExpress, const std::string strKey, std::string &strValue, BOOL bEndKeyValue=FALSE );
	BOOL GetKeyValue(const std::string &strExpress, const std::string strKey, int &nValue);
};

#endif
