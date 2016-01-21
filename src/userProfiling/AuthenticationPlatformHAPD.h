/*
 * AuthenticationPlatformHAPD.h
 *
 *  Created on: Oct 17, 2012
 *      Author: ufit
 */

#ifndef AUTHENTICATIONPLATFORMHAPD_H_
#define AUTHENTICATIONPLATFORMHAPD_H_


#include "PracticalSocket.h"
#include "UPMonitoring.h"
#include <vector>
extern "C"{
	#include "uloop_message_api.h"
}
/**
 * NEW Hostapd Message Type
 * UserId:Id,MAC:12:23:44:,IP:192.168.1.1,Token:Empty,State:Authenticated
 */
struct HAPMessage{
	std::string mac;
	std::string message;
};
struct RMMessage{
	std::string mac;
	std::string cryptoid;
	std::string token;
};
struct DHCPObj{
	string mac;
	string IP;
	string ts;
};
class AuthenticationPlatformHAPD{

private:
	static AuthenticationPlatformHAPD* m_pInstance;

	static const string AUTHENTICATED;
	static const string ASSOCIATED;
	static const string DISASSOCIATED;
	static const  string DEAUTHENTICATED;
	static const int MESSAGEBUFFERSIZE=1000;
public:
	static AuthenticationPlatformHAPD* Instance();
	HAPMessage message;
	TCPServerSocket *sock;

	static void *run(void *context);
	static void* listenHostapdMessage(void*);
	static void* listenRMMessage(void*);

	AuthenticationPlatformHAPD();
	~AuthenticationPlatformHAPD();
	void getHostpadMessage(TCPSocket *sock,HAPMessage&);
	void interpretMessage(HAPMessage& message);
	void getClientInfoMessage(RMMessage&);
	void convertMessageInHAPForm(string str,HAPMessage& mes);
	bool checkUserInAuthenticatedList(string& mac);
	void createNewUserObject(UserAuthListObject& user,RMMessage& m);
	string getIPFromDHCPTable(string& mac);
	vector<DHCPObj> readDHCPFile();
	void getAuthenticatedUser(string& mac,UserAuthListObject* user);
	bool userInList(string& mac);
	static void *getIP(void *obj);//for IP address from ARP File
	void receiveRMMessage(cryptoid	cid,double token);
	void createNewUserObject(UserAuthListObject& user,HAPMessage& message);

};



#endif /* AUTHENTICATIONPLATFORMHAPD_H_ */
