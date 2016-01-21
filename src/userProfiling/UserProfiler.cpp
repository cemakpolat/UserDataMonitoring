/*
 * UserProfiler.cpp
 *
 *  Created on: May 10, 2013
 *      Author: cemakpolat
 */

#include "UserProfiler.h"
#include <string>
int UserProfiler::timeInterval=10;
UserProfiler::UserProfiler(string& username,string& mac,string& ip) {
	// TODO Auto-generated constructor stub
	this->tickNumber=0;//just in case
	this->userid=username;
	this->mac=mac;
	this->ip=ip;
	std::cout<<"IP:"<<ip<<std::endl;
	umodel=new UserModel(username);
	session=new UserSession();
	bandwidth=new UserBandwidth();
	//this->setModelValuesToObserver();
}

UserProfiler::~UserProfiler() {
	// TODO Auto-generated destructor stub
	delete umodel,session,bandwidth;
}
void UserProfiler::setModelValuesToObserver(){
	session->getParametersFromUserModel(umodel);
	bandwidth->getParametersFromUserModel(umodel);
}
void UserProfiler::setObserverValuesToModel(){
	session->setParametersToUserModel(this);
	bandwidth->setParametersToUserModel(this);
	umodel->writeModelInFile(this->userid);
}




