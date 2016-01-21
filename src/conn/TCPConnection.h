/*
 * TCPConnection.h
 *
 *  Created on: Mar 7, 2013
 *      Author: cem
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_
#include <string.h>
#include "PracticalSocket.h"
extern "C"{
	#include "uloop_message_api.h"
}

class TCPConnection {
public:
	static char measure_what[50];
	static UloopMessage *ulm ;
	static int conn ;
	void sendGetQoSMeasurements();
	void initQoSModule();
	string receiveMessage();
	void sendResponseMessage(string response);
	void sendRequestMessage(int request);
	void closeModule();
	std::string sendRequestGetResponse( int request);
	void sendQoSResponseMessage(std::string response, int request);
	void sendQoSResponseMessage(string adelay,string abitrate,string ajitter,string apacketloss );

	std::string sendQoSRequestGetResponse( int request);
	static const unsigned int RCVBUFSIZE = 32;
	static const unsigned int BUFFIRSTSIZEINCHAR=10;
	TCPConnection();
	virtual ~TCPConnection();
	std::string getMessageInSmallChunks(TCPSocket* sock);
	std::string getMessageFirstSizeInInt(TCPSocket *sock);//For hostpad
	bool sendMessageInSmallChunks(string ip, string& text,int echoServPort);
	bool sendMessageFirstSizeInInt(string ip,string text,int port);
	std::string getMessageLocal(TCPSocket *sock);
	bool sendMessageLocal( string text,int port);
	double sendReqToUBMRecvMessageFromUBM(int& reqNumber,TCPServerSocket *sock);
};

#endif /* TCPCONNECTION_H_ */
