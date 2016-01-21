/*
 * QoSTracker.h
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#ifndef QoSTRACKER_H_
#define QoSTRACKER_H_

#include "PracticalSocket.h"

//TODO: DITG MEssage should be
/**
 * DITG Report
 * FlowResult=FlowNumber:1,
 * 			  FromIP:192.168.1.222,
 * 			  FromPort:36962,
 * 			  ToIP:192.168.1.1,
 * 			  ToPort:9524,
 * 			  TotalTime:     1.036290,
 * 			  TotalPacket:          114 ,
 * 			  MinimumDelay:     0.001762,
 * 			  MaximumDelay:     0.053444,
 * 			  AverageDelay:     0.011616,
 * 			  AverageJitter:     0.006133,
 * 			  DelayStdDeviation:     0.011236,
 * 			  BytesRX:        91697,
 * 			  AverageBitrate:   707.886788,
 * 			  AveragePacketRate:   110.007816,
 * 			  PacketDropped:             20 (0.00 %),
 * 			  AverageLossBurstSize:      0.000000
 * 			  |
 * 			  TotalResult={"Flow Number":"            1","Total Time":"     1.036290","Total Packet":"          114","Minimum Delay":"     0.001762","Maximum Delay":"     0.053444","Average Delay":"     0.011616","Average Jitter":"     0.006133","DelayStd Deviation":"     0.011236","BytesRX":"        91697","Average Bitrate":"   707.886788","Average Packet Rate":"   110.007816","Packet Dropped":"            0 (0.00 %)","Average Loss-Burst Size":"            0","Error lines": "            0"}}
 * ==================
 * As we evaluate only the first flow we do not touch not the Total Result;
 * In file this will be written as an unique string line or we can only store the following values such as delay, jitter, bitrate and packetloss.
 *
 */

struct QoSObject{
	double AverageDelay;
	double AverageJitter;
	double AverageBitRate;
	double AveragePacketLoss;
};
class QoSTracker{
private:
	std::string userIP;
	std::string userMac;
	int bufferingNumber;
	time_t lastQoSTime;
public:
	TCPServerSocket *sock;
	QoSTracker();
	~QoSTracker();
	void randomBackOfftime();
	int getAuthenticatedUserCount();
	QoSObject getMessage();
	void storeQoSMeasurement();
	void shiftQoSFromClientsToLeft(std::string userId, std::string ip,std::string userMAC, std::string time, QoSObject& meas);
	std::string getUsername(std::string ip);
	static void* run(void*);
	static void* qosShifter(void *);
	std::string getMessageInSmallChunks(TCPSocket *sock);

};


#endif /* QoSTRACKER_H_ */
