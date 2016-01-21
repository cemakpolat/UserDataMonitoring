/*
 * QoSTracker.cpp
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#include "QoSTracker.h"
#include "QoS.h"

#include "TCPConnection.h"
#include <stdlib.h>
#include "UPMain.h"
#include <iostream>
#include "Utilities.h"
#include "Definitions.h"
extern "C"{
	#include "uloop_message_api.h"
}

QoSTracker::QoSTracker(){
	bufferingNumber=0;
}
QoSTracker::~QoSTracker(){
	delete sock;
}


///////////////////

/*
 * QoS External communication thread
 */
void QoSTracker::run(void* object) {

	printf("QoS Tracker started \n");
	QoSTracker *qos = (QoSTracker*) object;

	try {
		UloopMessage *ulm1 = NULL;
		int conn = -1;


		qos->lastQoSTime=Utilities::getCurrentTimeAsSecond();
		int rc;
		pthread_t shifter;
		if ((rc = pthread_create(&shifter, NULL, &QoSTracker::qosShifter,
				qos))) {
			printf("QoS Tracker:QoS Shifter thread creation failed: %d\n", rc);
		}

		while (true) {

			if (UPMain::Instance()->getAuthenticatedUserCount() > 0) {
				cout<<"User Count:"<<UPMain::Instance()->getAuthenticatedUserCount()<<endl;

				low_init_module(QOS_MODULE_IPV4);

			if(conn>=0){
				conn = recv_uloop_ipv4_msg(ulm1);
				double val = 0;
				double averageDelay = 0, averageBitrate = 0, averageJitter = 0,
						averagePacketLoss = 0;

				char* client_ip[20], avgdelay[20], avgbitrate[20],
						avgpktloss[20], avgjitter[20];
				char measure_what[50];
				cout << "=========== EXTERNAL QOS Module =============" << endl;
				if (ulm1 != NULL) {

					switch (uloop_message_type(ulm1)) {
					case ULOOP_MESSAGE_TYPE__ULOOP_QOSMESSAGE:
						/* parse received message as a measurement request */
						parse_external_qos_measurements(ulm1, client_ip,avgdelay, avgbitrate, avgpktloss, avgjitter);

						QoSObject o;
						o.AverageBitRate = atof(avgbitrate);
						o.AverageDelay = atof(avgdelay);
						o.AverageJitter = atof(avgjitter);
						o.AveragePacketLoss =  atof(avgpktloss);
						qos->userIP=client_ip;
						//QoSObject o=qos->getMessage();
							//IP Address
							if (o.AverageDelay!=0) {

								if (UPMain::Instance()->checkUserAuthenticityByIP(qos->userIP)) {
									Utilities::writeConsole("User IP:"+qos->userIP);
									string userId=UPMain::Instance()->getUserNameByIP(qos->userIP);
									qos->userMac=UPMain::Instance()->getUserMAC(qos->userIP);
									qos->shiftQoSFromClientsToLeft(userId,qos->userIP,qos->userMac,Utilities::getCurrentDateAndTimeAsString(),o);

									pthread_mutex_lock(&QoS::Instance()->mutexTimeQoS);
									qos->lastQoSTime=Utilities::getCurrentTimeAsSecond();
									pthread_mutex_unlock(&QoS::Instance()->mutexTimeQoS);
								}
							} else {
								Utilities::writeConsole("QoS:Authenticated user number is 0 and waiting for RBT");
								Utilities::randomBackOfftime();

							}
						break;
					default:
						break;
					}
				}

				if (ulm1) {
					free_uloop_message(ulm1);
				}
				close_connection(conn);
				cout << "====================================" << endl;

			} else {
				cout
						<< "QoS: Authenticated user number is 0 and waiting for RBT"
						<< endl;
				Utilities::randomBackOfftime();
			}
		}
		}
	} catch (exception& e) {
		cerr << "QoS Tracker:" << e.what() << endl;
		QoSTracker::run(object);
	}

}

/////////////

void* QoSTracker::run(void* object) {

	//int portNumber=ConfigParameters::QoSPortNumber;//13132
	int portNumber = Definitions::QoSServerPortNumber;
	sleep(2);
	printf("QoS Tracker started \n");
	QoSTracker *qos = (QoSTracker*) object;

	//create socket and begin to listen
	qos->sock=new TCPServerSocket(portNumber);

	try {
		qos->lastQoSTime=Utilities::getCurrentTimeAsSecond();
		int rc;
		pthread_t shifter;
		if ((rc = pthread_create(&shifter, NULL, &QoSTracker::qosShifter,
				qos))) {
			printf("QoS Tracker:QoS Shifter thread creation failed: %d\n", rc);
		}

		while(true){
			if(UPMain::Instance()->getAuthenticatedUserCount()>0){
				cout<<"User Count:"<<UPMain::Instance()->getAuthenticatedUserCount()<<endl;
				QoSObject o=qos->getMessage();

					//IP Address
					if (o.AverageDelay!=0) {

						if (UPMain::Instance()->checkUserAuthenticityByIP(qos->userIP)) {
							Utilities::writeConsole("User IP:"+qos->userIP);

							string userId=UPMain::Instance()->getUserNameByIP(qos->userIP);
							qos->userMac=UPMain::Instance()->getUserMAC(qos->userIP);
							qos->shiftQoSFromClientsToLeft(userId,qos->userIP,qos->userMac,Utilities::getCurrentDateAndTimeAsString(),o);

							pthread_mutex_lock(&QoS::Instance()->mutexTimeQoS);
							qos->lastQoSTime=Utilities::getCurrentTimeAsSecond();
							pthread_mutex_unlock(&QoS::Instance()->mutexTimeQoS);
						}
					} else {
						Utilities::writeConsole("QoS:Authenticated user number is 0 and waiting for RBT");
						Utilities::randomBackOfftime();

					}
			} else {
				cout<<"QoS: Authenticated user number is 0 and waiting for RBT"<<endl;
				Utilities::randomBackOfftime();
			}
	}
	} catch (SocketException &e) {
		cerr <<"QoS Tracker:" <<e.what() << endl;
		QoSTracker::run(object);
		Utilities::randomBackOfftime();
	} catch (exception& e) {
		cerr <<"QoS Tracker:" <<e.what() << endl;
		QoSTracker::run(object);
	}

	pthread_exit(NULL); //exit from thread, but this object will take place in memory.
	return NULL;
}
void* QoSTracker::qosShifter(void* object){
	pthread_detach(pthread_self());
	QoSTracker *qos=(QoSTracker*)object;
	cout<<"QoS Shifter started"<<endl;
	time_t currentTime;time_t oldTime;
	long  diff;
	try{
		while(true){
			sleep(20);
			 currentTime=Utilities::getCurrentTimeAsSecond();
			pthread_mutex_lock(&QoS::Instance()->mutexTimeQoS);
			 oldTime=qos->lastQoSTime;
			pthread_mutex_unlock(&QoS::Instance()->mutexTimeQoS);
			diff=(currentTime-oldTime);
			//cout<<"QoS: Time diff "<<diff<<endl;
			if(diff>=20){
				QoSObject o;o.AverageBitRate=0;o.AverageDelay=0;o.AverageJitter=0;o.AveragePacketLoss=0;
				qos->shiftQoSFromClientsToLeft("","","","",o);
			}
		}
		pthread_exit(NULL); //exit from thread, but this object will take place in memory.
	}catch(std::exception& e){
		e.what();
		QoSTracker::qosShifter(object);
	}
}

//TODO: assign names with reference &
void QoSTracker::shiftQoSFromClientsToLeft(string userId,string ip,string mac,string time,QoSObject& measurement ){
	unsigned int measurementCount=50;
	QoSPacketObject packet; //TODO: can be defined as a Class instead of struct in order to manage dynamically with memory.
	packet.ip = ip;
	packet.mac = mac;
	packet.userId = userId;
	packet.time = time;
	packet.qosMeasurement = measurement;
	pthread_mutex_lock(&QoS::Instance()->mutexQoS); //TODO:handling mutex exception

	cout<<"QoS Shifter: List size " <<QoS::Instance()->qosList.size()<<endl;

	if (QoS::Instance()->qosList.size() < measurementCount) {
		QoS::Instance()->qosList.push_back(packet);
	} else {
		QoS::Instance()->qosList.erase(QoS::Instance()->qosList.begin() + 0);
		QoS::Instance()->qosList.push_back(packet);
	}
	pthread_mutex_unlock(&QoS::Instance()->mutexQoS);

	bufferingNumber = bufferingNumber + 1;
	if (bufferingNumber == measurementCount) {
		bufferingNumber = 0;
		cout<<"Buffering number reached "<<bufferingNumber<<endl;

		//this->storeQoSMeasurement();
	}
}
/**
 * The incoming message Format :  AverageDelay:5910.313477,AverageJitter:0.001109,AverageBitrate:0.001199,AveragePacketLoss:0.000000
 */
QoSObject QoSTracker::getMessage() {

	//////QOS Object//////////////
	cout<<"QOS Message From CNM:\n"<<endl;

	//This below code block works without issue, please do not remove
	//	TCPConnection *con=new TCPConnection();
	//	string message=con->getMessageInSmallChunks(sock->accept());
	//	delete con;

	string message=this->getMessageInSmallChunks(sock->accept());
//	cout<<"Received Message From CNM:\n"<<message<<endl;

	//this->userIP="127.0.0.1";//sock->getLocalAddress();

	vector<string> v0, v2;
	v0 = Utilities::split(message, ',');

	QoSObject o;
	o.AverageBitRate = 0;
	o.AverageDelay = 0;
	o.AverageJitter = 0;
	o.AveragePacketLoss = 0;

	for (unsigned int i = 0; i < v0.size(); i++) {

		v2 = Utilities::split(v0[i], ':');
		if(v2.size()>0){
			if (Utilities::contains(v2[0],"AverageDelay")) {
				o.AverageDelay = Utilities::convertStringDouble(v2[1]);
			} else if (Utilities::contains(v2[0],"AverageJitter")) {
				o.AverageJitter = Utilities::convertStringDouble(v2[1]);
			} else if (Utilities::contains(v2[0], "AveragePacketLoss")) {
				o.AveragePacketLoss = Utilities::convertStringDouble(v2[1]);
			} else if (Utilities::contains(v2[0],"AverageBitrate")) {

				o.AverageBitRate = Utilities::convertStringDouble(v2[1]);
			}
		}else{
			cout<<"QoS: Message does not contain network measurement"<<endl;
		}
	}
//	cout << "AverageDelay: " << o.AverageDelay << endl;
//	cout << "AverageBitRate: " << o.AverageBitRate << endl;
//	cout << "AverageJitter: " << o.AverageJitter << endl;
//	cout << "AveragePacketLoss: " << o.AveragePacketLoss << endl;
	//WRITE QOS Object in File
	return o;

}

/*
 * Store Measumrent in a file with respect to ClientID(MAC address)
 * This function can be removed, because there is indeed no need to perform that.
 */
void QoSTracker::storeQoSMeasurement(){

	vector<QoSPacketObject> list;
	pthread_mutex_lock(&QoS::Instance()->mutexQoS); //TODO:handling mutex exception
	for(unsigned int i=0;i<QoS::Instance()->qosList.size();i++){
		list.push_back(QoS::Instance()->qosList[i]);
	}
	cout<<"QoS: qos List size " <<list.size()<<endl;
	QoS::Instance()->qosList.clear();
	pthread_mutex_unlock(&QoS::Instance()->mutexQoS);
	string toWrite =" ";
	QoSObject o;
	for(unsigned int i=0;i<list.size();i++){
		toWrite=list[i].time+"\n"+toWrite;
		o=list[i].qosMeasurement;
		toWrite = Utilities::convertDoubleToString(o.AverageDelay) + ","
				+ Utilities::convertDoubleToString(o.AverageBitRate) + ","
				+ Utilities::convertDoubleToString(o.AverageJitter) + ","
				+ Utilities::convertDoubleToString(o.AveragePacketLoss);

		Utilities::fileAppend(list[i].userId, toWrite);
	}
}


std::string QoSTracker::getMessageInSmallChunks(TCPSocket *sock) {
	 const unsigned int RCVBUFSIZE = 32;
	std::string str="";
		try {
			// Establish connection with the echo server
			//	TCPSocket sock(servAddress, echoServPort);
			this->userIP=sock->getForeignAddress();
			cout << sock->getForeignAddress() << ":";
			cout << sock->getForeignPort()<<endl;
			char echoBuffer[RCVBUFSIZE];
			int recvMsgSize;
			while ((recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE)) > 0) { // Zero means// end of transmission

				// Echo message back to client
				//cout<<":received Message Size "<<recvMsgSize<<endl;
				echoBuffer[recvMsgSize] = '\0';
				//cout << ":echo " << echoBuffer << endl;
				str.append(echoBuffer);
				//sock->send(echoBuffer, recvMsgSize);
			}
			//cout << ":all " << str << endl;
			delete sock;
		} catch (SocketException &e) {
			cerr << e.what() << endl;

		}
		return str;
}


