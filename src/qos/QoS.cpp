/*
 * QoS.cpp
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#include "QoS.h"
#include <stdio.h>
#include "QoSTracker.h"
//#include "BlackBoard.h"
#include "Utilities.h"
//initiate list
std::vector<QoSPacketObject> QoS::qosList;

QoS::QoS(){
	this->averageDelayNormalizationFactor = 0.2;
	this->packetLossRateNormalizationFactor = 10;
	this->jitterNormalizationFactor = 0.2;
	this->bitRateNormalizationFactor = 700;

}
QoS::~QoS(){

}
void* QoS::runITGChecker(void*) {

	printf("ITG: started \n");
	//system("killall iperf");
	//system("iperf -s &");
	system("killall ITGRecv");
	while (true) {
		try {
			//start itg
			//system("/home/cem/uloopSVN/clientLoadBalancing/D-ITG-2.7.0-Beta2/bin/ITGRecv &");//TODO: change to root directory
			system("ITGRecv &");//TODO: change to root directory
			sleep(300);//ITG Refresh time

			system("killall ITGRecv");
			Utilities::writeConsole("\n\n ITG Checker: Restarting the ITG \n\n");
		} catch (SocketException& e) {
			std::cerr << "ITG Checker:" << e.what() << std::endl;
			QoS::runITGChecker(NULL);
		} catch (std::exception& e) {
			std::cerr << "ITG Checker:" << e.what() << std::endl;
			QoS::runITGChecker(NULL);
		}
	}
	pthread_exit(NULL);
return NULL;
}
void* QoS::run(void* object) {
	QoS *qos=(QoS*)object;
	qos->qosMain();
}
//TODO: CHANGE PLEASE MUTEX FILES
void QoS::qosMain(){
	printf("QoS Component started\n");
	//TODO: Change mutex code which is appropriate for GCC.4.3.3
	//mutexQoS = PTHREAD_MUTEX_INITIALIZER;
		mutexQoS.__m_reserved = 0;
		mutexQoS.__m_count = 0;
		mutexQoS.__m_owner = 0;
		mutexQoS.__m_kind = PTHREAD_MUTEX_ADAPTIVE_NP;
		mutexQoS.__m_lock.__status = 0;
		mutexQoS.__m_lock.__spinlock = 0;

		//mutexTimeQoS=PTHREAD_MUTEX_INITIALIZER;
		mutexTimeQoS.__m_reserved = 0;
		mutexTimeQoS.__m_count = 0;
		mutexTimeQoS.__m_owner = 0;
		mutexTimeQoS.__m_kind = PTHREAD_MUTEX_ADAPTIVE_NP;
		mutexTimeQoS.__m_lock.__status = 0;
		mutexTimeQoS.__m_lock.__spinlock = 0;


	pthread_t qosTracker,ditg;
	QoSTracker qT;
	//ITGChecker itg; //TODO : can be pointer

	int rc1;
	//start QoSTracker
	if ((rc1 = pthread_create(&qosTracker, NULL, &QoSTracker::run, &qT))) {
		printf("QoS Block: QoS Tracker thread creation failed: %d\n", rc1);
		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc1 = pthread_create(&qosTracker, NULL, &QoSTracker::run, &qT))) {
				success = false;
				printf("QoS Block: QoS Tracker thread creation failed: %d\n", rc1);
				if (attempt == 2) {
					printf(
							"QoS Block:QoS Tracker thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}
		//start ITG
	if ((rc1 = pthread_create(&ditg, NULL, &QoS::runITGChecker, NULL))) {
		printf("ITG Checker thread creation failed: %d\n", rc1);
		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc1 = pthread_create(&ditg, NULL, &QoS::runITGChecker, NULL))) {
				success = false;
				printf("QoS Block:ITG Checker thread creation failed: %d\n",
						rc1);
				if (attempt == 2) {
					printf(
							"QoS Block: ITG Checker thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}
	pthread_join(qosTracker,NULL);
	pthread_join(ditg,NULL);

	printf("QoS Block ended");
}

//singleton pattern required
QoS* QoS::m_qosInstance=NULL;
QoS* QoS::Instance()
{
   if (!m_qosInstance)   // Only allow one instance of class to be generated.
	   m_qosInstance = new QoS();
   return m_qosInstance;
}

//----------------------Functionalities for QoS----------------------//

int QoS::getAverageDelay(){
	std::string result = "NULL"; //in case of nothing it will return NULL or may be ""
	double maximumAverageDelay = 0;
	pthread_mutex_lock(&QoS::Instance()->mutexQoS);
	if(	QoS::Instance()->qosList.size()>8){
		int listSize=QoS::Instance()->qosList.size();
		for(int i=listSize-1;i>=(listSize-8);i--){
			QoSObject o=QoS::Instance()->qosList[i].qosMeasurement;
			if(o.AverageDelay!=0){
				double temp=o.AverageDelay*1000;
				if( temp > maximumAverageDelay){
					Utilities::writeConsole("QoS Block: Congestion Occured" );
							maximumAverageDelay = temp;
				}
			}

		}
	}
	pthread_mutex_unlock(&QoS::Instance()->mutexQoS);
	int tempInt = (int) maximumAverageDelay;
	return tempInt;
}
//getCurrentUserMACs(){} this function should be called by UPMain

/*multipe or one congested user*/
/**
 * Returned message "congestedUserIP=192.168.1.1"
 */
string QoS::firstCongestedUser() {
	double averaeDelay = 20;
	string congestedUser;
	pthread_mutex_lock(&QoS::Instance()->mutexQoS);
	for (unsigned int i = 0; i < QoS::Instance()->qosList.size(); i++) {
		QoSObject o = QoS::Instance()->qosList[i].qosMeasurement;
		if (o.AverageDelay != 0) {
			double temp = o.AverageDelay * 1000;
			if (temp > averaeDelay) {
				congestedUser = "congestedUserIP="+ QoS::Instance()->qosList[i].ip;
				i = QoS::Instance()->qosList.size();
			}
		}
	}
	pthread_mutex_unlock(&QoS::Instance()->mutexQoS);
	return congestedUser;
}
/**
 * Returned message "congestedUserIP=192.168.1.1"
 */
string QoS::worstCongestedUser(){
	double temp1=0;
	string congestedUser;
	pthread_mutex_lock(&QoS::Instance()->mutexQoS);
	int placeOfWorstAvDelay=0;
	for(unsigned int i=0;i<QoS::Instance()->qosList.size();i++){
		QoSObject o = QoS::Instance()->qosList[i].qosMeasurement;
		if (o.AverageDelay != 0) {
					double temp = o.AverageDelay * 1000;
					if (temp > temp1) {
						temp1=temp;
						placeOfWorstAvDelay=i;
					}
				}
		}
	if(QoS::Instance()->qosList.size()>0){
		congestedUser="congestedUserIP="+QoS::Instance()->qosList[placeOfWorstAvDelay].ip;
	}
	pthread_mutex_unlock(&QoS::Instance()->mutexQoS);
	return congestedUser;
}
/**
 * Returned message "congestedUserIP=192.168.1.1" TODO: ask Mursel congestedUsers
 */
string QoS::congestedusers() {
	double averaeDelay = 20;
	string congestedUser;
	pthread_mutex_lock(&QoS::Instance()->mutexQoS);
	for (unsigned int i = 0; i < QoS::Instance()->qosList.size(); i++) {
		QoSObject o = QoS::Instance()->qosList[i].qosMeasurement;
		if (o.AverageDelay != 0) {
			double temp = o.AverageDelay * 1000;
			if (temp > averaeDelay) {
				congestedUser = congestedUser + QoS::Instance()->qosList[i].ip;
			}
		}
	}
	pthread_mutex_unlock(&QoS::Instance()->mutexQoS);

	return congestedUser;
}
//CreditTRACK
vector<QoSPacketObject> QoS::getQoSList(){
	vector<QoSPacketObject> list;
	pthread_mutex_lock(&QoS::Instance()->mutexQoS);
	list=QoS::Instance()->qosList;
	pthread_mutex_unlock(&QoS::Instance()->mutexQoS);
	return list;
}
void  QoS::qoSMeasurements(double& delay,double& bitrate,double& jitter,double& pktloss){
	vector<QoSPacketObject> list;
	pthread_mutex_lock(&QoS::Instance()->mutexQoS);
	list=QoS::Instance()->qosList;
	pthread_mutex_unlock(&QoS::Instance()->mutexQoS);
	//vector<QoSPacketObject> list=this->getQoSList();

	delay=this->averageDelayFirstDelay3(list);
	bitrate=this->averageBitRate2(list);
	jitter=this->averageJitter2(list);
	pktloss=this->averagePacketLoss2(list);

}
//double QoS::averageBitRate() {
//	return this->averageBitRate2(this->getQoSList());
//}
double QoS::averageBitRate2(std::vector<QoSPacketObject>& list) { //TODO:receive please reference
	double averageBitRate = 0;
	try {
		if (list.size() > 8) {
			for (int i = list.size() - 1; i >= (list.size() - 8); i--) {
				QoSObject o =list[i].qosMeasurement;
				if (o.AverageBitRate != 0) {
					double temp = o.AverageBitRate;
					if (temp > averageBitRate) {
						averageBitRate = temp;
					}
				}
			}
		}

	} catch (exception& e) {
		cerr << "QoS:  " << e.what() << endl;
	}
	cout <<"QoS::Average Bitrate:"<< ((10 * averageBitRate) / this->bitRateNormalizationFactor) << endl;
	return ((10 * averageBitRate) / this->bitRateNormalizationFactor);
}

//double QoS::averagePacketLoss() {
//	return this->averagePacketLoss2(this->getQoSList());
//}
double QoS::averagePacketLoss2(std::vector<QoSPacketObject>& list) {
	double maximumAveragePacketLoss = 0;
	try {
		if (list.size() > 8) {
			for (int i = list.size() - 1; i >= (list.size() - 8); i--) {
				//QoSObject o = QoS::Instance()->qosList[i].qosMeasurement;
				QoSObject o =list[i].qosMeasurement;

				if (o.AverageBitRate != 0) {
					double temp = o.AveragePacketLoss;
					if (temp > maximumAveragePacketLoss) {
						maximumAveragePacketLoss = temp;
					}
				}
			}
		}

	} catch (exception& e) {
		cerr << "QoS: PL2: " << e.what() << endl;
	}
	cout << "QoS:AveragePacketLoss: "
			<< ((10 * maximumAveragePacketLoss)
					/ this->packetLossRateNormalizationFactor) << endl;
	return ((10 * maximumAveragePacketLoss)
			/ this->packetLossRateNormalizationFactor);

}
//double QoS::averageDelay(){
//	//return this->averageDelay2(this->getQoSList());
//	return this->averageDelayFirstDelay3(this->getQoSList());
//}
double QoS::averageDelay2(std::vector<QoSPacketObject>& list) {
	double maximumAverageDelay = 0;
	try {
		if (list.size() > 8) {
			for (int i = list.size() - 1; i >= (list.size() - 8); i--) {
				//QoSObject o = QoS::Instance()->QoSList[i].qosMeasurement;
				QoSObject o =list[i].qosMeasurement;

				if (o.AverageDelay != 0) {
					double temp = o.AverageDelay;
					if (temp > maximumAverageDelay) {
						maximumAverageDelay = temp;
					}
				}
			}
		}

	} catch (exception& e) {
		cerr << "QoS AD2: " << e.what() << endl;
	}
	return ((10 * maximumAverageDelay) / this->averageDelayNormalizationFactor);
}
double QoS::averageDelayFirstDelay3(std::vector<QoSPacketObject>& list) {
	double maximumAverageDelay = 0;
	try {
		if (list.size() > 8) {
			for (int i = list.size() - 1; i >= (list.size() - 8); i--) {
				//QoSObject o = QoS::Instance()->qosList[i].qosMeasurement;
				QoSObject o =list[i].qosMeasurement;

				if (o.AverageDelay != 0) {
					double temp = o.AverageDelay;
					if (temp > maximumAverageDelay) {
						maximumAverageDelay = temp;
					}
				}
			}
		}

	} catch (exception& e) {
		cerr << "QoS: AD3: " << e.what() << endl;
	}
	cout << "QoS:Average Delay : "
			<< ((10 * maximumAverageDelay)
					/ this->averageDelayNormalizationFactor) << endl;
	return ((10 * maximumAverageDelay) / this->averageDelayNormalizationFactor);
}
//double QoS::averageJitter(){
//	return this->averageJitter2(this->getQoSList());
//}
double QoS::averageJitter2(std::vector<QoSPacketObject>& list) {
	double maximumAverageJitter = 0;
	try {
		if (list.size() > 8) {
			for (int i = list.size() - 1; i >= (list.size() - 8); i--) {
				//QoSObject o = QoS::Instance()->qosList[i].qosMeasurement;
				QoSObject o =list[i].qosMeasurement;

				if (o.AverageJitter != 0) {
					double temp = o.AverageJitter;
					if (temp > maximumAverageJitter) {
						maximumAverageJitter = temp;
					}
				}

			}
		}
	} catch (exception& e) {
		cerr << "QoS AJ: " << e.what() << endl;
	}
	cout << "QoS:Average Jitter: "
			<< ((10 * maximumAverageJitter) / this->jitterNormalizationFactor)
			<< endl;
	return ((10 * maximumAverageJitter) / this->jitterNormalizationFactor);

}


//----------------------End of Functionalities for QoS----------------------//











