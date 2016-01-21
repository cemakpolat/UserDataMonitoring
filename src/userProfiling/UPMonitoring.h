/*
 * UPMonitoring.h
 *
 *  Created on: 04.08.2012
 *      Author: cem
 */

#ifndef UPMONITORING_H_
#define UPMONITORING_H_

#include <string>
#include "UserAuthListObject.h"
#include <pthread.h>
#include "UserProfiler.h"

class UserProfiler;
struct timer {
	unsigned long duration;
	bool state;
	int count;
};

class UPMonitoring{
private:
	std::string userPacket;

	pthread_t timerObj;
	struct timer t;

public:
	std::string Mac;
	std::string IP;
	std::string token;
	std::string userId;
	UserProfiler *uprofile;
	int timeInterval;
	bool timerOut;

	~UPMonitoring(); //TODO:destructor raises a failure in which it is said that it is private!
	UPMonitoring(UserAuthListObject& userObj);//TODO reference could be better than copy
	UPMonitoring(UserAuthListObject* userObj);
	static void* run(void*);
	void composeUserId();
	bool checkUserAuthenticity(std::string& userId);
	//void getUserModelFromStoragePlace(string& userId,Object& usermodel);
	void getUserModelFromStoragePlace(std::string& userId);
	void updateProcessedPacket();
	void storeUserModelInStoragePlace(UPMonitoring* p);

	void timer(UPMonitoring *upm);
	static void* timerFunction(void*);
	void callTimerTask(std::string& userId);
	void insertIPTableCommands(std::string& ip,std::string& mac);
	void removeIPTableCommands(std::string& ip,std::string& mac);

	void setTickNumber();
	void userSessionProfile();
	void getUserModelInfo();
	void transferUserProfileInfoIntoUserModel();


};


#endif /* UPMONITORING_H_ */
