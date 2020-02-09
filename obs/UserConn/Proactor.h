#ifndef _SERVER_H_
#define _SERVER_H_
//#include "StdString.h"
#include <string>
#include <ace/Asynch_IO.h>

class Server : public ACE_Service_Handler
{
public:
	Server();
	~Server();

	virtual void open(ACE_HANDLE handle,\
			ACE_Message_Block & message_block);
    void closeServer();
//	void send_to_softphone(CStdString msg);
	void send_to_softphone(const std::string &msg);
protected:
	virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);
	virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result & result);
private:
	int initiate_read_stream(void);
	int initiate_write_stream(ACE_Message_Block& mb,size_t nbytes);
	void start_write(ACE_Message_Block* mblk);
	bool process_message(ACE_Message_Block* mb);

	ACE_HANDLE handle_;
	ACE_Asynch_Read_Stream rs_;
	ACE_Asynch_Write_Stream ws_;
};

#endif
