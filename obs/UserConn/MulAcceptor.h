#ifndef _MUL_ACCEPTOR_H_
#define _MUL_ACCEPTOR_H_

#include "Proactor.h"
//#include <ace/Asynch_Acceptor>
#include <ace/Asynch_Acceptor.h>
class MulAcceptor : public ACE_Asynch_Acceptor<Server>
{
	public:
		MulAcceptor();
		~MulAcceptor();
};

#endif
