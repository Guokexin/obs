#include "license.h"
#include <pcap.h>
#include <iostream>
CStdString CLicense::StringDecrypt(CStdString chstr,BYTE Key)
{
	BYTE ch1,ch2;
	if(Key>31)
		Key=20;
	CStdString EncryptString=_T("");
	int i=0;
	int Length=chstr.GetLength();
	for(i=0;i<Length;i++)
	{
		ch1=(BYTE)chstr.GetAt(i);
		if(ch1==32) ch1=0;
		ch2=ch1&0x03;
		ch1>>=2;
		(BYTE)(ch2<<=6);
		ch1|=ch2;
		ch1^=Key;
		
		EncryptString+=(UINT)ch1;
	}
	return(EncryptString);
}
void CLicense::FindAllDevice(std::list<CStdString> &ls)
{
	std::cout<<"################################"<<std::endl;
	pcap_if_t* devices = NULL;
	
	
	char errorBuf[PCAP_ERRBUF_SIZE];
	memset(errorBuf, 0, sizeof(errorBuf));
	char* error = errorBuf;
	
	
	if(pcap_findalldevs(&devices, errorBuf) == -1)
	{
	//	LOG4CXX_ERROR(LOG.sysLog, CStdString("pcap error when discovering devices: ") + error);
	}
	else
	{
		if(devices)
		{
			for(pcap_if_t* device=devices; device!=NULL; device=device->next)
			{
				if(!device)
					break;
				
				CStdString deviceName(device->name);
				deviceName.MakeLower();
				
				if(deviceName.Find("dialup") == -1 &&
					deviceName.Find("lo") == -1 &&
					deviceName.Find("any") == -1)
				{
					//pcap_addr_t *a = device->addresses;
					//char* a1 = inet_ntoa(((struct sockaddr_in *)a->addr)->sin_addr); 
				
					//CString ip;
					//ip.Format("%s",a1);

					std::cout<<deviceName.c_str()<<std::endl;

					ls.push_back(deviceName);

					


				}
			}
		}
	}
}

