/*
 * UserModel.h
 *
 *  Created on: Feb 21, 2013
 *      Author: cem
 */
/**
 * The file structure that we want to write/read as follows:
 *
 * UserId=cem,
 * UDPAverage=0,
 * TCPAverage=0,
 * lastDepartureTime=,
 * dailyAuthenticationNumber=0,
 * visitFrequencyNumber=0,
 * authenticationNumber=0,
 * bandwidthAverage=0,
 * sessionAverage=0,
 * sessions=VF:0;BW:0|VF:;BW:|VF:;BW:|VF:;BW:|...
 */

#ifndef USERMODEL_H_
#define USERMODEL_H_
#include <string>
#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Utilities.h"
struct SessionListObject{
	double visitingFrequency;
	double bandwidth;
};
class UserModel {
private:

public:
	static const string userModelFolderName;
	static const string qoEFolderName;
	static  pthread_mutex_t mutexFile;
	static int observationNumber;
	UserModel();
	UserModel(std::string& userName);
	virtual ~UserModel();
	double UDPAverage;
	double TCPAverage;
	std::string lastDepartureTime;
	long int dailyAuthenticationNumber;
	long int visitFrequencyNumber;
	long authenticationNumber;
	double bandwidthAverage;
	std::string userId;
	std::string sessionAverage;
	std::vector<SessionListObject> sessionList;
	UserModel* getUserModel();
	bool isUserModelExist(std::string& userName);
	void storeUserModel();
	//void writeModelInFile(std::string& fileName, std::string& outputString);
	void writeModelInFile(std::string& userId);
	void readModel(std::string& userName);
	void createModel(std::string& userName);
	string pathFinder(const string folderName, const string fileName);



};

#endif /* USERMODEL_H_ */
