/*
 * UserBandwidth.h
 *
 *  Created on: 07.08.2012
 *      Author: cem
 */

#ifndef USERBANDWIDTH_H_
#define USERBANDWIDTH_H_
#include "UserSession.h"
#include <vector>
#include "UserProfiler.h"
class UserProfiler;
class UserSession;
struct bandwidthObject {
	std::string packetDirection;
	std::string packetTrafficType;
	int packetSize;
};


struct iptObject{
	long int byte;
	std::string direction;
};
class UserBandwidth {

public:
	std::vector<iptObject> iptList;
	long int currentUploadByte;
	long int currentDownloadByte;
	double bandwidthAverage;
	long int totalDownloadBW;
	long int totalUploadBW;
	double sessionBandwidthAveragePerMin;
	std::string ip;
	std::string userId;
	long int totalTCPBW;
	double TCPAverage;
	long int totalUDPBW;
	double UDPAverage;
	UserBandwidth();
	~UserBandwidth();
	void calculateTotalBandwidth(std::string& ip);
	void calculateSessionBandwidthAveragePerMin(UserProfiler *upm);
	//void calculateSessionBandwidthAveragePerMin(UPMonitoring *upm, std::string& ip);
	void calculateBandwidthAverage(UserSession& session);
	void calculateProtocolsBW();
	void setParametersToUserModel(UserProfiler *upm);
	void getParametersFromUserModel(UserModel* userModel);
	void getBytes( const std::string& ip);

};

#endif /* USERBANDWIDTH_H_ */
