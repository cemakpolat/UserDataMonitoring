/*
 * UserProfiler.h
 *
 *  Created on: May 10, 2013
 *      Author: cemakpolat
 */

#ifndef USERPROFILER_H_
#define USERPROFILER_H_

#include "UserSession.h"
#include "UserBandwidth.h"
#include "UserModel.h"
class UserBandwidth;
class UserSession;
class UserProfiler {
public:
	static int timeInterval;
	std::string userid;
	std::string ip;
	std::string mac;
	int tickNumber;
	UserSession *session;
	UserBandwidth *bandwidth;
	UserModel *umodel;
	UserProfiler(string& username,string& mac,string& ip);
	virtual ~UserProfiler();
	void setModelValuesToObserver();
	void setObserverValuesToModel();

};

#endif /* USERPROFILER_H_ */
