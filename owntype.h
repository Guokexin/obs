#ifndef OWNTYPE_H
#define OWNTYPE_H

#define TRUE 1
#define FALSE 0

#define REPLACE(x, y, z)\
{\
    size_t n = x.find(y);\
    if(n!=std::string::npos)\
        x.replace(n, strlen(y), z);\
}

typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned int DWORD;
typedef void     *LPVOID;
typedef unsigned char BYTE;
#define _T(x) (x)



#endif
