/*
 * QoS.h
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#ifndef QoS_H_
#define QoS_H_
#include <vector>
#include <pthread.h>
#include "QoSTracker.h"


struct QoSPacketObject {
	std::string userId;
	std::string ip;
	std::string time;
	QoSObject qosMeasurement;
	std::string mac;
};

class QoS {
private:
	static QoS* m_qosInstance;
	//ITGChecker *ditg;
	QoSTracker *qosTracker;

	double averageDelayNormalizationFactor;
		double packetLossRateNormalizationFactor;
		double jitterNormalizationFactor;
		double bitRateNormalizationFactor;


public:
	//pthread_t itg;
	pthread_mutex_t mutexQoS; //TODO: check the necessity of this mutex!
	pthread_mutex_t mutexTimeQoS;
	QoS();
	~QoS();
	void qosMain();
	static std::vector<QoSPacketObject> qosList;
	static QoS* Instance();
	static void* run(void*);
	static void* runITGChecker(void*);

	int getAverageDelay();
	std::string congestedusers();
	std::string worstCongestedUser();
	std::string firstCongestedUser();
	//int averageDelay();
	void qoSMeasurements(double& delay,double& bitrate,double& jitter,double& pktloss);
	double averageBitRate();
	double averageBitRate2(std::vector<QoSPacketObject>& list);

	double averagePacketLoss();
	double averagePacketLoss2(std::vector<QoSPacketObject>& list);

	double averageDelay();
	double averageDelay2(std::vector<QoSPacketObject>& list);
	double averageDelayFirstDelay3(std::vector<QoSPacketObject>& list);

	double averageJitter();
	double averageJitter2(std::vector<QoSPacketObject>& list);

	std::vector<QoSPacketObject> getQoSList();
};

#endif /* QoS_H_ */
