/*
 * TCPConnection.cpp
 *
 *  Created on: Mar 7, 2013
 *      Author: cem
 */

#include "TCPConnection.h"
#include <iostream>           // For cerr and cout
#include "Utilities.h"
#include <netinet/in.h>
#include "CommLang.h"
#include "Definitions.h"
extern "C"{
	#include "uloop_message_api.h"
}

//char TCPConnection::measure_what[50];
//UloopMessage *TCPConnection::ulm = NULL;
//int TCPConnection::conn = -1;

TCPConnection::TCPConnection() {
	// TODO Auto-generated constructor stub

}

TCPConnection::~TCPConnection() {
	// TODO Auto-generated destructor stub
}

// start the ULOOP message module
//void TCPConnection::initQoSModule(){
//
//	init_module(QOS_MODULE);
//}

//void TCPConnection::sendResponseMessage(string response){
//
//	//release the received message
//	send_measurement_request(conn, response.c_str());
//
//	if(ulm)
//		free_uloop_message(ulm);
//
//	//close the received connection
//	close_connection(conn);
//}

//void TCPConnection::sendRequestMessage(int request){
//
//	std::string str="";
//	int conn = connect_to_qos();
//	send_measurement_request(conn, "averages");
//
//	char clientip[20], bandwidth[20];
//	recv_measurement_request( conn, bandwidth);
//	fprintf(stderr, "Client: %s\n", bandwidth);
//
//	close_connection(conn);
//}
//receive Message
//string TCPConnection::receiveMessage(){
//	// recv uloop message on the pointer, and return the open socket connection id
//	Utilities::errorCatcher("TCPConnection receiveMessage",3);
//
//	conn = recv_uloop_unix_msg(&ulm);
//	// parse received message as a measurement request
//	parse_measurement_request(ulm, measure_what);
//	fprintf(stderr, "Request: %s\n", measure_what);
//	return measure_what;
//}
//
//
////send Message to QoS Client
//void TCPConnection::sendQoSResponseMessage(string adelay,string abitrate,string ajitter,string apacketloss ){
//
//	send_average_qos_measurements(conn, "127.0.0.1", adelay.c_str(),abitrate.c_str() , ajitter.c_str(), apacketloss.c_str());
//
//	//release the received message
//	if(ulm)
//		free_uloop_message(ulm);
//
//		 //close the received connection
//	close_connection(conn);
//}

//send Message
//void TCPConnection::sendQoSResponseMessage(string response, int request){
//
//	if(CommLang::getAverageDelayTime==request){
//		 //send a reply message
//		send_average_qos_measurements(conn, "ip", response.c_str(), "", "", "");
//	}
//	else if(CommLang::getAverageJitter==request){
//		send_average_qos_measurements(conn, "ip", "", "", "", response.c_str());
//	}
//	else if(CommLang::getBitRate==request){
//		send_average_qos_measurements(conn, "ip", response.c_str(), "", "", "");
//	}
//	else if(CommLang::getPacketLoss==request){
//		send_average_qos_measurements(conn, "ip", "", "", response.c_str(), "");
//	}
//	//release the received message
//	if(ulm)
//		free_uloop_message(ulm);
//
//		 //close the received connection
//	close_connection(conn);
//}
//For Client Tracking Request
//void TCPConnection::sendGetQoSMeasurements(){
//
//	int conn = connect_to_qos();
//	send_measurement_request(conn, "QoSMeasurements");
//	char clientip[20], avgdelay[20], avgbitrate[20], avgpktloss[20], avgjitter[20];
//	recv_average_qos_measurements(conn, clientip, avgdelay, avgbitrate, avgpktloss, avgjitter);
//	fprintf(stderr, "Client: %s\n", clientip);
//	fprintf(stderr, "Delay: %s bitrate: %s pktloss: %s jitter: %s\n", avgdelay, avgbitrate, avgpktloss, avgjitter);
//	close_connection(conn);
//}


////close Module
//void TCPConnection::closeModule(){
//	 //close the ULOOP message module
//	close_module(QOS_MODULE);
//}

//request a measurement and return the result of concerning request
//std::string TCPConnection::sendQoSRequestGetResponse( int request){
//	std::string response="";
//	int conn = connect_to_qos();
//	//string a=Utilities::convertIntToString(request).c_str();
//	send_measurement_request(conn, Utilities::convertIntToString(request).c_str());
//
//	char clientip[20], avgdelay[20], avgbitrate[20], avgpktloss[20], avgjitter[20];
//
//	recv_average_qos_measurements(conn, clientip, avgdelay, avgbitrate, avgpktloss, avgjitter);
//	fprintf(stderr, "Client: %s\n", clientip);
//	fprintf(stderr, "Delay: %s bitrate: %s pktloss: %s jitter: %s\n", avgdelay, avgbitrate, avgpktloss, avgjitter);
//	close_connection(conn);
//
//	if(CommLang::getAverageDelayTime==request){
//		response=avgdelay;
//	}
//	else if(CommLang::getAverageJitter==request){
//		response=avgjitter;
//	}
//	else if(CommLang::getBitRate==request){
//		response=avgbitrate;
//	}
//	else if(CommLang::getPacketLoss==request){
//		response=avgpktloss;
//	}
//
//	std::string ad=avgdelay;
//	std::string abr=avgbitrate;
//	std::string apl=avgpktloss;
//	std::string aj=avgjitter;
//
//	cout<<ad<<":"<<abr<<":"<<apl<<":"<<aj<<endl;
//
//	return response;
//}

/**
 * The following functions are implemented for TCP Socket communications between internal and external component so as to facilitate
 * their inter-communications.
 */

/*
 * Send Message to clients
 */
bool TCPConnection::sendMessageInSmallChunks(string ip, string& text, int echoServPort) {
	string servAddress = ip;

	const char *echoString = text.c_str();
	int echoStringLen = strlen(echoString);   // Determine input length
	try {
		// Establish connection with the echo server
		TCPSocket sock(servAddress, echoServPort);
		// Send the string to the echo server
		sock.send(echoString, echoStringLen);
		return true;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		//exit(1);
		return false;
	}
	return false;
}
/*
 * Receive Message from Clients by means of the defined buffer size.
 */
std::string TCPConnection::getMessageInSmallChunks(TCPSocket *sock) { //,string servAddress,int echoServPort
	std::string str;
	try {

		cout << sock->getForeignAddress() << ":";cout << sock->getForeignPort();
		char echoBuffer[RCVBUFSIZE];
		int recvMsgSize;
		while ((recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE)) > 0) { // Zero means// end of transmission

			echoBuffer[recvMsgSize] = '\0';
			cout << ":echo " << echoBuffer << endl;
			str.append(echoBuffer);
		}
		cout << ":all " << str << endl;
		delete sock;

	} catch (SocketException &e) {
		cerr << e.what() << endl;

	}
	return str;
}
/*
 * Obtain sent data by Client
 * Get first the length of the package from socket and then reserve a place in the previous received length for the incoming packet.
 */
std::string TCPConnection::getMessageFirstSizeInInt(TCPSocket *sock) {
	int val[1];
	try {
		//get packet size
		sock->recv(val, sizeof(val));
		uint32_t size = ntohl(val[0]);
		char echoBuffer[size];
		//read packet itself
		sock->recv(echoBuffer, size);
		echoBuffer[ val[0]] = '\0';
		delete sock;
		return echoBuffer;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
	}
}
/*
 * Transmit data to Client with respect to its IP address.
 * Send primarly the size of the packet will be trasmitted, then send packet itself.
 */
bool TCPConnection::sendMessageFirstSizeInInt(string ip, string text,
		int port) {
	try {
		TCPSocket *sock = new TCPSocket(ip, port);
		uint32_t size = htonl(strlen(text.c_str()));
		//sent packet size
		sock->send(&size, sizeof(size));
		//sent packet itself
		sock->send(text.c_str(), size);
		delete sock;
		return true;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		return false;
	}
	return false;
}
/*
 *Get Local Message From Client by means of two socket communications.
 *There is any conversion from network-to-host or host-to-network
 */
std::string TCPConnection::getMessageLocal(TCPSocket *sock) {

	try {
		int val[1];
		//get packet size
		sock->recv(val, sizeof(val));
		cout<<val[0]<<endl;
		char echoBuffer[val[0]];
		//read packet itself
		sock->recv(echoBuffer, val[0]);
		echoBuffer[ val[0]] = '\0';

		Utilities::errorCatcher("getMessageLocal",1);
		cout<<echoBuffer<<endl;
		string a=echoBuffer;
		delete sock;
		return a;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		sleep(3);
		getMessageLocal(sock);
	}
	return "";
}
/*
 * Send data to the local user via two socket transmission(2 message subsequently)
 * There is any conversion from network-to-host or host-to-network
 */

bool TCPConnection::sendMessageLocal(string text,int port) {
	try {

		TCPSocket *sock = new TCPSocket("127.0.0.1", port);
		const char *echoString =text.c_str();
		int val[1];
		val[0]=strlen(echoString);
		sock->send(val, sizeof(val));
		sock->send(echoString,val[0]);
		delete sock;
		Utilities::errorCatcher("sendMessageLocal",1);

		return true;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		return false;
	}
	return false;
}

/*
 * Send Request Message to Client,
 * Once its response comes, return it to the caller
 */
double TCPConnection::sendReqToUBMRecvMessageFromUBM(int& reqNumber,TCPServerSocket *sock) {

	Utilities::errorCatcher("sendReqToUBMRecvMessageFromUBM",1);

	int measurementPlaneServer = Definitions::MPServerPortNumber;
	int loadBalancingServerPort = 14555;
	string localAddress = "127.0.0.1";

	TCPConnection *senReqToMP = new TCPConnection();

	// SEND REQUEST MESSAGE TO MEASUREMENT PLANE
	CommLang *cm=new CommLang();
	senReqToMP->sendMessageLocal(cm->createRequestMesObj(loadBalancingServerPort,localAddress,reqNumber),measurementPlaneServer);


	string mes="";
	mes=senReqToMP->getMessageLocal(sock->accept());
	delete senReqToMP;

	double value =Utilities::convertStringDouble(mes);

	delete cm;
	return value;

}

 /*A SIMPLE EXAMPLE FOR CLIENT AND SERVER
//CLIENT
int main(int argc, char *argv[]) {

	string servAddress = argv[1]; // First arg: server address
	char *echoString = argv[2];   // Second arg: string to echo

	try {
		TCPConnection *con = new TCPConnection();
		for (;;) {
			//con->sendMessageInSmallChunks("192.168.1.1",echoString,12121);

			con->sendMessageFirstSizeInInt("192.168.1.1", echoString, 12121);
			//sendMessage();
			sleep(2);
		}
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}
//SERVER
int main(int argc, char *argv[]) {

	try {
		TCPConnection *con = new TCPConnection();
		// Server Socket object
		TCPServerSocket servSock(12121);
		for (;;) {   // Run forever
			//cout<<con->getMessageInSmallChunks(servSock.accept())<<endl;
			cout << con->getMessageFirstSizeInInt(servSock.accept()) << endl;

		}
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	// NOT REACHED

	return 0;
}
*/

