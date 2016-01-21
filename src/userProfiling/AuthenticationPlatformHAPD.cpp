
/*
 * AuthenticationPlatformHAPD.cpp
 *
 *  Created on:  Oct 17, 2012
 *      Author: cem and Mursel
 * Authentication Message Format:
 * {
 * 		"userId"="",
 * 		"IP"="",
 * 		"MAC"="",
 * 		"Token"="",
 * 		"State"=""-> authenticated or deauthenticated
 * }
 */

#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>
#include "UPMain.h"
#include "AuthenticationPlatformHAPD.h"
#include "TCPConnection.h"
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include "Utilities.h"
#include "Definitions.h"
extern "C"{
	#include "uloop_message_api.h"
}
/*Socket Path for Hostapd Messages*/
#define SOCKET_PATH "/var/run/uloop/hostapd_socket"

/*Buffer Length for received hostapd messages*/
#define BUFFLEN 4000

/*
 *Possible Message Types could be received Hosatpd or Resource Manager(RM)
 */
const string AuthenticationPlatformHAPD::ASSOCIATED="Association";
const string AuthenticationPlatformHAPD::DISASSOCIATED="Disassociation";
const string AuthenticationPlatformHAPD::AUTHENTICATED="Authentication";
const string AuthenticationPlatformHAPD::DEAUTHENTICATED="Deauthentication";


struct composedObject{
	AuthenticationPlatformHAPD* aut;
	UserAuthListObject newUser;
};

 AuthenticationPlatformHAPD::AuthenticationPlatformHAPD(){
}
 AuthenticationPlatformHAPD::~AuthenticationPlatformHAPD(){
}
 //singleton pattern required
 AuthenticationPlatformHAPD* AuthenticationPlatformHAPD::m_pInstance=NULL;
 AuthenticationPlatformHAPD* AuthenticationPlatformHAPD::Instance()
 {
    if (!m_pInstance)   // Only allow one instance of class to be generated.
       m_pInstance = new AuthenticationPlatformHAPD();
    return m_pInstance;
 }

/*
 * Listen hostapd Messages
 */
void* AuthenticationPlatformHAPD::listenHostapdMessage(void*) {

	pthread_detach(pthread_self());
	/**
	 * Unix Socket Communication
	 */
	struct sockaddr_un strAddr;
	socklen_t lenAddr;
	int fdSock;
	int fdConn;
	char str[BUFFLEN];
	try{
		/*Open a socket in UNIX domain*/
		if ((fdSock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
			perror("socket");
			//exit(1);
		}
		/*Unlink the path in order to prevent the creation of the conflict on the same file*/
		unlink(SOCKET_PATH);

		/* Unix Domain */
		strAddr.sun_family = AF_UNIX;

		/*Insert Path in parameters*/
		strcpy(strAddr.sun_path, SOCKET_PATH);
		lenAddr = sizeof(strAddr.sun_family) + strlen(strAddr.sun_path);

		/*Bind socket*/
		if (bind(fdSock, (struct sockaddr*) &strAddr, lenAddr) != 0) {
			perror("bind");
			//exit(1);
		}
		/*Start to listen to the clients*/
		if (listen(fdSock, 5) != 0) {
			perror("listen");
			//exit(1);
		}
		int n=0;
		HAPMessage message;
		for (;;) {
			/*Accept client and build a connection between client and server, and continue to the listen*/
			if ((fdConn = accept(fdSock, (struct sockaddr*) &strAddr, &lenAddr))
					>= 0) {
				printf("\n------------| Hostapd Message|--------------- \n");
				/*Receive data*/
				n = recv(fdConn, str, sizeof(str), 0);
				if (n <= 0) {
					if (n < 0)
						perror("recv");
				}
				printf("Message From Hostapd:\n%s", str);

				AuthenticationPlatformHAPD::Instance()->convertMessageInHAPForm(str,message);
				AuthenticationPlatformHAPD::Instance()->interpretMessage(message);
				printf("\n---------------------------------------------- \n");
				/*Close established connection*/
				close(fdConn);
			}
		}
		/*Close the bound connection*/
		close(fdSock);

	} catch (exception& e) {
		cerr << "AuthenticationPlatform: " << e.what() << endl;
		sleep(5);
		AuthenticationPlatformHAPD::listenHostapdMessage(NULL);

	}
	pthread_exit(NULL);

}
void AuthenticationPlatformHAPD::convertMessageInHAPForm(string str,
		HAPMessage& message) {
	message.mac = "";
	message.message = "";

	vector<string> v0, v1;
	v0 = Utilities::split(str, ',');

	for (unsigned int i = 0; i < v0.size(); i++) {
		v1 = Utilities::split(v0[i], '=');
		if (v1[0] == "MAC") {
			message.mac = v1[1];
		} else if (v1[0] == "Message") {
			message.message = v1[1];
		}
	}

}

void AuthenticationPlatformHAPD::interpretMessage(HAPMessage& message) {
	try {
		cout<<"Message: "<<message.message<<endl;
		if (message.message == AuthenticationPlatformHAPD::AUTHENTICATED) {

			cout << "AuthAssoBlock: Client in  " << message.message << " state"
					<< endl;

			AuthenticationPlatformHAPD *ap = NULL;

			ap = AuthenticationPlatformHAPD::Instance();
			cout << "Authentication Platform: Client in Authenticated State  "
					<< endl;

			UserAuthListObject newUser;
			ap->createNewUserObject(newUser, message);
			bool userExist = false;
			//userExist=ap->checkUserInAuthenticatedList(message.mac);
			userExist = UPMain::Instance()->checkUserAuthenticityByMAC(message.mac);
			if (userExist == false) {
				newUser.ip = ap->getIPFromDHCPTable(message.mac);
				cout << "AuthAssoBlock: Mac address : " << newUser.Mac << " "
						<< newUser.ip << endl;

				if (newUser.ip == "0.0.0.0") {//		if (newUser.ip == "0.0.0.0") {//TODO:Change to ==
					cout
							<< "AuthenticationPlatformHAPD:IP address received as 0.0.0.0"
							<< endl;
					sleep(2);
					int rc1;
					pthread_t IPGetter;
					composedObject *composedObj=new composedObject ;
					composedObj->aut = ap;
					composedObj->newUser = newUser; //composedObj.newUser = &newUser;

					if ((rc1 = pthread_create(&IPGetter, NULL, ap->getIP,
							composedObj))) { //upm is already pointer no need to write address like &upm
						printf("AuthAssoBlock:Thread creation failed: %d\n",
								rc1);
					}

				} else {
					cout << "AuthAssoBlock: Mac address: " << newUser.Mac
							<< endl;
					UPMonitoring *upm = new UPMonitoring(&newUser);
					pthread_t newClient;
					cout << "AuthAssoBlock: Mac address: " << upm->Mac << endl;
					int rc = 0;
					if ((rc = pthread_create(&newClient, NULL,
							&UPMonitoring::run, upm)) && upm != NULL) {
						//upm is already pointer no need to write address like &upm
						printf("AuthAssoBlock:Thread creation failed: %d\n",
								rc);
						int attempt = 0;
						while (attempt < 3) {
							bool success = true;
							Utilities::randomBackOfftime(7);
							if ((rc = pthread_create(&newClient, NULL,
									&UPMonitoring::run, upm))) {
								success = false;
								printf(
										"AuthAssoBlock:Thread creation failed: %d\n",
										rc);
								if (attempt == 2) {
									printf(
											"AuthAssoBlock:Thread creation failed,please re-start the program. \n");
								}
							}
							if (success == true) {
								attempt = 3;
							}
						}
					}
					//add user in list
					pthread_mutex_lock(&UPMain::Instance()->mutex1);
					UPMain::Instance()->userAuthList.push_back(newUser);
					pthread_mutex_unlock(&UPMain::Instance()->mutex1);

				}

			} else {
				cout
						<< "Authentication Platform: user is currently authenticated "
						<< endl;
			}
		}

		else if (message.message == AuthenticationPlatformHAPD::DEAUTHENTICATED
				|| message.message
						== AuthenticationPlatformHAPD::DISASSOCIATED) {

			cout << "AuthenticationPlatformHAPD: Client in " << message.message
					<< " state" << endl;

			//	if (AuthenticationPlatformHAPD::Instance()->checkUserInAuthenticatedList(
			//			message.mac)) {
			if (UPMain::Instance()->checkUserAuthenticityByMAC(message.mac)) {
				pthread_mutex_lock(&UPMain::Instance()->mutex1);
				cout << "AuthenticationPlatformHAPD: Authenticated User Count: "
						<< UPMain::Instance()->userAuthList.size() << endl;
				int index;
				for (unsigned int i = 0;
						i < UPMain::Instance()->userAuthList.size(); i++) {
					if (message.mac== UPMain::Instance()->userAuthList[i].Mac) {
						index = i;
						i= UPMain::Instance()->userAuthList.size();
					}
				}
				UPMain::Instance()->userAuthList.erase(UPMain::Instance()->userAuthList.begin() + index);
				cout << "AuthAssoBlock:Authenticated User Count: "
						<< UPMain::Instance()->userAuthList.size() << endl;
				pthread_mutex_unlock(&UPMain::Instance()->mutex1);
			} else {
				cout
						<< "AuthenticationPlatformHAPD: Client has already left the connection area "
						<< endl;
			}
		}
	} catch (exception& e) {
		cerr << "Authentication Platform: " << e.what() << endl;
	}
}

/*
 * Create Client Object based the received Message from RM
 */
void AuthenticationPlatformHAPD::createNewUserObject(UserAuthListObject& user,
		RMMessage& message) {
	try {
		vector<string> list = Utilities::split(message.mac, ':');
		string str = "";
		for (unsigned int i = 0; i < list.size(); i++) {
			str += list[i];
		}
		user.userId = str;
		user.Mac = message.mac;
		user.ip = "";
		user.token = message.token;
		//user.cryptoid=message->cryptoid;
	} catch (exception& e) {
		cerr << "AuthenticationBlock: " << e.what() << endl;
	}
}
/*
 * Create Client Object based the received Message from RM
 */
void AuthenticationPlatformHAPD::createNewUserObject(UserAuthListObject& user,HAPMessage& message){
	try{
		vector<string> list = Utilities::split(message.mac, ':');
		string str = "";
		for (unsigned int i = 0; i < list.size(); i++) {
			str += list[i];
		}
		user.userId=str;
		user.Mac=message.mac;
		user.ip="";
		user.token="";
	}catch(exception& e){
		cerr<<"AuthenticationBlock: "<<e.what()<<endl;
	}
}
/*
 * Run subsequently all components in this class. It is invoked by another component.
 */
void* AuthenticationPlatformHAPD::run(void* obj){

	pthread_detach(pthread_self());
	AuthenticationPlatformHAPD *authObj = (AuthenticationPlatformHAPD*) obj;
	pthread_t hostpadListener;
	int rc1;
	if ((rc1 = pthread_create(&hostpadListener, NULL,&authObj->listenHostapdMessage, NULL))) {
		printf("Measurement Plane:Thread creation failed: %d\n", rc1);
	}

	pthread_join(hostpadListener,NULL);

	pthread_exit(NULL);

}
/*
 * Waits Authentication Messages from Resource Manager(RM) components thus allowing to start user monitoring thread.
 */


void AuthenticationPlatformHAPD::receiveRMMessage(cryptoid	cid,double token){
	cout << "Authentication Platform: Client authentication Message sent by RM Module  "<< endl;
//	try {
//
//		RMMessage message;
//		AuthenticationPlatformHAPD *ap=NULL;
//
//			ap=AuthenticationPlatformHAPD::Instance();
//			//ap->getClientInfoMessage(message);
//
//			cout << "Authentication Platform: Client in Authenticated State  "<< endl;
//
//			UserAuthListObject newUser;
//			ap->createNewUserObject(newUser,message);
//			bool userExist=false;
//			//userExist=ap->checkUserInAuthenticatedList(message.mac);
//			userExist=UPMain::Instance()->checkUserAuthenticityByMAC(
//					message.mac);
//		if(userExist==false){
//			newUser.ip = ap->getIPFromDHCPTable(message.mac);
//			cout<<"AuthAssoBlock: Mac address : "<< newUser.Mac <<" "<<newUser.ip<<endl;
//
//		if (newUser.ip == "0.0.0.0") {
//			cout << "AuthenticationPlatformHAPD:IP address received as 0.0.0.0"<< endl;
//			sleep(2);
//			int rc1;
//			pthread_t IPGetter;
//
//			composedObject *composedObj=new composedObject ;
//			composedObj->aut = ap;
//			composedObj->newUser = newUser; //composedObj.newUser = &newUser;
//
//			if ((rc1 = pthread_create(&IPGetter, NULL,
//					ap->getIP, &composedObj))) { //upm is already pointer no need to write address like &upm
//				printf("AuthAssoBlock:Thread creation failed: %d\n", rc1);
//			}
//
//		} else {
//				cout<<"AuthAssoBlock: Mac address: "<< newUser.Mac<<endl;
//				UPMonitoring *upm = new UPMonitoring(&newUser);
//				pthread_t newClient;
//				cout<<"AuthAssoBlock: Mac address: "<< upm->Mac<<endl;
//				int rc=0;
//				if ((rc = pthread_create(&newClient, NULL,&UPMonitoring::run, upm)) && upm!=NULL) {
//					//upm is already pointer no need to write address like &upm
//					printf("AuthAssoBlock:Thread creation failed: %d\n", rc);
//					int attempt = 0;
//					while (attempt < 3) {
//						bool success = true;
//						Utilities::randomBackOfftime(7);
//						if ((rc = pthread_create(&newClient, NULL,
//								&UPMonitoring::run, upm))) {
//							success = false;
//							printf("AuthAssoBlock:Thread creation failed: %d\n",rc);
//							if (attempt == 2) {
//								printf("AuthAssoBlock:Thread creation failed,please re-start the program. \n");
//							}
//						}
//						if (success == true) {
//							attempt = 3;
//						}
//					}
//				}
//				//add user in list
//				pthread_mutex_lock(&UPMain::Instance()->mutex1);
//				UPMain::Instance()->userAuthList.push_back(newUser);
//				pthread_mutex_unlock(&UPMain::Instance()->mutex1);
//			}
//
//		}else{
//			cout<<"Authentication Platform: user is currently authenticated "<<endl;
//		}
//
//	} catch (exception& e) {
//		cerr << "Authentication Platform" << e.what() << endl;
//	}
}

/*
 * Tries to get IP address by reading regularly DHCP File under /var/dhcp.leases. In case of its existence the User Monitoring Thread
 * will be started. The potential reason to implement this function, IP address can be assigned later after our IP request.
 */
void *AuthenticationPlatformHAPD::getIP(void* obj){

	pthread_detach(pthread_self());
	int loopCount=0;

		composedObject *comp=(composedObject*)obj;
		bool state=false;
		Utilities::errorCatcher("getIP",1);
		while(loopCount<10){
			Utilities::errorCatcher("getIP",22);
			comp->newUser.ip=comp->aut->getIPFromDHCPTable(comp->newUser.Mac);
				if(comp->newUser.ip=="0.0.0.0"){//TODO: check here the form of the IP Address
					cout<<"AuthenticationPlatformHAPD: IP could not be fetched, there could be any record in dhcp.lease \n "
							"file related to this user"<<endl;
					usleep(Utilities::second(2));
					state=false;
				}else{
					state=true;
					cout<<"AuthenticationPlatformHAPD: IP address is received: "<<comp->newUser.ip<<endl;
					UPMonitoring *upm = new UPMonitoring(&comp->newUser);

					int rc;
					pthread_t newClient;
					if ((rc = pthread_create(&newClient, NULL, &UPMonitoring::run, upm))&& upm!=NULL) {
						printf("AuthenticationPlatformHAPD:Thread creation failed: %d\n", rc);
						int attempt = 0;
						while (attempt < 3) {
							bool success = true;
							Utilities::randomBackOfftime(7);
							if ((rc = pthread_create(&newClient, NULL,
									&UPMonitoring::run, upm))) {
								success = false;
								printf("AuthenticationPlatformHAPD:Thread creation failed: %d\n", rc);
								if (attempt == 2) {
									printf(
											"AuthenticationPlatformHAPD:Thread creation failed,please "
											"re-start the program. \n");
								}
							}
							if (success == true) {
								attempt = 3;
							}
						}
					}
					//add user in list
					pthread_mutex_lock(&UPMain::Instance()->mutex1);
					UPMain::Instance()->userAuthList.push_back(comp->newUser);
					pthread_mutex_unlock(&UPMain::Instance()->mutex1);
					loopCount=11;
				}
				loopCount++;
	}
	delete obj;
	if(state==false){
		cout<<"AuthenticationPlatformHAPD: IP could not be fetched, there could be any valid record in dhcp.lease \n "
				"	file related to this user or DHCP record is too old for this user"<<endl;

	}
	pthread_exit(NULL);
}

/*
 * Return the requested User IP address after reading dhcp.leases file
 */
int dhcpCounter=0;
string AuthenticationPlatformHAPD::getIPFromDHCPTable(string& mac){
	//read from dhcp.leases file
	vector<DHCPObj> list=readDHCPFile();
	string ip="0.0.0.0";
	int time=0;
	bool exist=false;
	for(unsigned int i=0;i<list.size();i++){
		if(mac==list[i].mac && time<atoi(list[i].ts.c_str())){
			exist=true;
			ip=list[i].IP;
			time =atoi(list[i].ts.c_str());
		}
	}
	if(exist==false && dhcpCounter<2){
		usleep(Utilities::second(3));
		dhcpCounter++;
		getIPFromDHCPTable(mac);
	}
	dhcpCounter=0;
	return ip;
}
/*
 * Return all Clients' IP in the dhcp.leases file.
 */
vector<DHCPObj> AuthenticationPlatformHAPD::readDHCPFile(){
	vector<DHCPObj> templist;
	std::ifstream infile("/var/dhcp.leases");
	try {
		//int firstLine=0;
		std::string line;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			string ts, mac, ip;
			if ((iss >> ts >> mac >> ip)) {
				if(Utilities::timeCompareWithCurrentTime(200,ts)){
					DHCPObj obj;
					obj.ts=ts;
					obj.IP = ip;
					obj.mac = mac;
					templist.push_back(obj);
				}

			}
		}
		return templist;
	} catch (exception& e) {
		e.what();
		cout << "AuthAssoBlock:DHCP.leases file could not be red" << endl;
		readDHCPFile();
	}
	return templist;
}


