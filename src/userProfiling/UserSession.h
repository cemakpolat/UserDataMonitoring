/*
 * UserSession.h
 *
 *  Created on: 07.08.2012
 *      Author: cem
 */

#ifndef USERSESSION_H_
#define USERSESSION_H_
#include <string>
#include <vector>
#include "UserModel.h"
#include "UserProfiler.h"

class UserProfiler;
struct sessionObject {
	time_t startTime;
	time_t endTime;
	double bandwidth;
};

class UserSession {
private:

public:
	long int authenticationNumber;
	time_t arrivalTime;
	time_t departureTime;
	bool timeAssigned;
	long sessionTime;
	long sessionAverage;
	std::vector<sessionObject> sessionList;
	double visitingFrequency;
	long int visitFrequencyNumber;
	int dailyAuthenticationNumber; //
	time_t lastDepartureTime;
	UserSession();
	~UserSession();
	void calculateSessionTime();
	void calculateSessionAverage();
	void calculateVisitingFrequency(unsigned int place,UserModel* userModel);
	void addListToJSON(UserProfiler *upm);
	//time_t getAdequateTime( std::string& t,int& timeInterval);
	std::string getDepartureTimeForGivenArrivalTime();
	void setParametersToUserModel(UserProfiler* upm);
	void getParametersFromUserModel(UserModel* userModel);
	//void checkDailyAuthNum(time_t now,time_t lastDepart);
	void setArrivalTime(time_t arrivalTime);
	void setDepartureTime(time_t departureTime);
	void setTimeisAssigned(bool isTimeAssigned);
	bool isTimeAssigned();
	long getAuthenticationNumber();
	void setAuthenticationNumber(long authenticationNum);
	bool tickChecker(int ticknumber);
	void checkDailyAuthNum(time_t now,time_t lastDepart);
};

#endif /* USERSESSION_H_ */
