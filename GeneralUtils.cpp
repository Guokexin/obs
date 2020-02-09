#include "GeneralUtils.h"

CGeneralUtils::CGeneralUtils()
{

}

CGeneralUtils::~CGeneralUtils()
{

}

UINT CGeneralUtils::x_Find(const std::string &szInput, const std::string &szSpeciStr, int startafter)
{
    int nInputLength = szInput.length();
	int nSpeciStrLength = szSpeciStr.length();
	int ipos = startafter;
	int ipos2;
//	char ch;
	BOOL bFind;

	while (ipos < nInputLength)
	{
//		ch = szInput.at(ipos);
		ipos2 = ipos;
		bFind = TRUE;
		for (int i = 0;i < nSpeciStrLength;i++)
		{
			if (szInput.at(ipos2++) != szSpeciStr.at(i))
			{
				bFind = FALSE;
				break;
			}
		}

		if (!bFind)	{ ipos++; }
		else { return ipos; }
	}
	return (ipos >= nInputLength ? -1 : ipos);
}

BOOL CGeneralUtils::CheckValidString(const std::string &str)
{
    if (str == "")
	{
		return FALSE;
	}
	return TRUE;
}

UINT CGeneralUtils::SplitterString(const std::string &szInput, const std::string &szDelimiter, std::vector<std::string>& arrayElems)
{
    int ipos = 0;
	int newpos = -1;
	int sizeD = szDelimiter.length();
	int isize = szInput.length();

	std::vector<int> positions;

	newpos = x_Find(szInput, szDelimiter, 0);

	if (newpos < 0) return 0;

	int numFound = 0;

	while (newpos > ipos)
	{
		numFound++;
		positions.push_back(newpos);
		ipos = newpos;
		newpos = x_Find(szInput, szDelimiter, ipos+sizeD+1);
	}

	for (unsigned int i = 0;i <= positions.size();i++)
	{
		std::string str;
		if (i == 0)
			str = szInput.substr(i, positions[i]);
		else
		{
			int offset = positions[i-1] + sizeD;
			if (offset < isize)
			{
				if (i == positions.size())
					str = szInput.substr(offset);
				else if (i > 0)
					str = szInput.substr(positions[i-1] + sizeD, positions[i] - positions[i-1] -sizeD);
			}
		}

		if (str.length() > 0)
			arrayElems.push_back(str);
	}

	return numFound;
}

void CGeneralUtils::BubbleSort(std::vector<std::string>& mysortArray, int count, bool ascend)
{
    for (int i = 0; i < count-1; i++)
		for (int j = 0; j < count-1-i; j++)
			if (ascend)
			{// 升序
				if (mysortArray[j] > mysortArray[j+1])
				{
					std::string temp = mysortArray[j];
					mysortArray[j] = mysortArray[j+1];
					mysortArray[j+1] = temp;
				}
			}
			else
			{// 降序
				if (mysortArray[j] < mysortArray[j+1])
				{
					std::string temp = mysortArray[j];
					mysortArray[j] = mysortArray[j+1];
					mysortArray[j+1] = temp;
				}
			}
}

BOOL CGeneralUtils::GetStringValue(const std::string &szInString, const char* szKey, char* szValue)
{
    char szTmp[256];
	memset( szTmp, 0, sizeof(szTmp) );
	sprintf( szTmp, "%s%s", szKey, "=" );

	char szTmpValue[10240];
    memset( szTmpValue, 0, sizeof(szTmpValue) );

	//memset( szValue, 0, sizeof(szValue) );

	if( (szInString == "") || (szKey == NULL) || (szValue == NULL) )
	{
		return FALSE;
	}

    if( (strlen(szKey) == 0) || (strlen(szKey) > 255) )
	{
		return FALSE;
    }

    const char* szString = strstr( szInString.c_str(), szTmp );
	if( szString != NULL )
    {
        strncpy( szTmpValue, szString+strlen(szTmp), strlen(szString)-strlen(szTmp) );

		int pos = strcspn( szTmpValue, ";" );
        if( pos >= 0 )
		{
			memset( szTmpValue+pos, 0, 10240-pos );
        }

		strcpy( szValue, szTmpValue );
    }

    return TRUE;
}

//BOOL CGeneralUtils::GetStringValue(const char* szInString, const char* szKey, int &nValue)
//{
//    char szValue[32];
//	memset( szValue, 0, sizeof(szValue) );
//	if( GetStringValue( szInString, szKey, szValue ) )
//	{
//		if( strlen(szValue) > 0 )
//		{
//			nValue = atoi(szValue);
//		}
//	}
//	else
//	{
//		return FALSE;
//	}
//
//
//	return TRUE;
//}

//BOOL CGeneralUtils::GetStringValue(const char* szInString, const char* szKey, bool &bValue)
//{
//    int nValue = 0;
//	if( GetStringValue( szInString, szKey, nValue ) )
//	{
//		if( 0 == nValue )
//		{
//			bValue = FALSE;
//		}
//		else
//		{
//			bValue = TRUE;
//		}
//	}
//	else
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}

//BOOL CGeneralUtils::GetKeyValue(const std::string &strExpress, const std::string strKey, std::string &strValue, BOOL bEndKeyValue)
//{
//    std::string strEqualExpress;
//	if(strExpress.empty() || strKey.empty())
//		return FALSE;
//
//	int nKeyLength = strKey.length();
//	int nKeyPos = strExpress.find(strKey);
//	int nEqualPos = strExpress.find("=",nKeyPos+nKeyLength);
//	int nSemicolonPos = 0;
//
//	if( bEndKeyValue )
//	{
//		nSemicolonPos = strExpress.length();
//	}
//	else
//	{
//		nSemicolonPos = strExpress.find(";",nEqualPos);
//	}
//
//	if(nKeyPos>=0 && nEqualPos>0)
//	{
//		if(nSemicolonPos < 0)
//			nSemicolonPos = strExpress.length() + 1;
//
//		strValue = strExpress.substr(nEqualPos+1,nSemicolonPos - nEqualPos - 1);
//		strValue.erase(strValue.begin(), std::find_if(strValue.begin(), strValue.end(),
//                                                std::not1(std::ptr_fun(::isspace))));
//		strValue.erase(std::find_if(strValue.rbegin(), strValue.rend(),
//                                                std::not1(std::ptr_fun(::isspace))).base(),
//                                                    strValue.end());
//
//		return TRUE;
//	}
//	else
//		return FALSE;
//}

//BOOL CGeneralUtils::GetKeyValue(const std::string &strExpress, const std::string strKey, int &nValue)
//{
//    std::string strValue;
//	if(GetKeyValue(strExpress,strKey,strValue))
//	{
//		if(!strValue.empty())
//		{
//			nValue = atoi(strValue.c_str());
//			return TRUE;
//		}
//		else
//		{
//			nValue = -1;
//			return FALSE;
//		}
//	}
//
//	return FALSE;
//}
