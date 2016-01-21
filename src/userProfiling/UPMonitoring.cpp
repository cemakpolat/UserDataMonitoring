/*
 * UPMonitoring.cpp
 *
 *  Created on: 27.07.2012
 *      Author: cem
 */

#include "UPMonitoring.h"
#include "UPMain.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include "Utilities.h"
//#include <memory>
using namespace std;
UPMonitoring::UPMonitoring(UserAuthListObject* userObj) {
	this->Mac = userObj->Mac;
	this->IP = userObj->ip;
	//userId is derived from MAC Address
	this->userId=userObj->userId;
	this->token = userObj->token;
	this->insertIPTableCommands(this->IP, this->Mac);

	this->uprofile=new UserProfiler(this->userId,this->Mac,this->IP);
	userObj->uprofile=this->uprofile;//TODO: Check here userObj
	//this->userModel->userId=this->userId;
	cout << "User Profile Monitoring is created " << "IP " << this->IP
			<< " MAC: " << this->Mac << endl;

}
bool UPMonitoring::checkUserAuthenticity(string& userId) {
	bool state=false;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].userId==userId) {
			state=true;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return state;
}
UPMonitoring::~UPMonitoring() {
	delete uprofile;
}

void* UPMonitoring::run(void* object) {
	pthread_detach(pthread_self());
	try {
		cout << "User Profile Monitoring:UPMonitoring started" << endl;
		UPMonitoring *upm = (UPMonitoring*) object;
		upm->timeInterval = 10; //10 minutes//should be transported in ConfigParameters File in the further steps
		bool authenticatedState =upm->checkUserAuthenticity(upm->userId);
		struct timeval arrTime, depTime;
		if (authenticatedState) {
			cout << "User Profile Monitoring:\" " << upm->userId
					<< " \" is authenticated" << endl;

			upm->uprofile->setModelValuesToObserver();
			//set arrival time
			gettimeofday(&arrTime, NULL);
			time_t arrivalTime = arrTime.tv_sec;
			cout << "User Profile Monitoring:Arrival Time: "
					<< Utilities::convertMilisecondToString(arrivalTime)
					<< endl;
			upm->uprofile->session->setArrivalTime(arrivalTime); //TODO:please check the taken time from cpu
			//set tick number for intervals
			upm->setTickNumber();
			//set time assign to false, assigned true means that user has gone
			upm->uprofile->session->timeAssigned = false;
			//set timer for periodically called functions for bandwidth measurements
			upm->timer(upm);
			//set timer to false until it would turn out to true;
			upm->timerOut=false;
			//set the authentication number
			upm->uprofile->session->setAuthenticationNumber(
					upm->uprofile->session->getAuthenticationNumber() + 1);
			cout<<"UPM: authentication number"<<upm->uprofile->session->authenticationNumber<<endl;
		}
		//start here to listen client authentication situation
		while (authenticatedState) {
			cout << "UPM Authentication State: "<<authenticatedState<< endl;

			try {
				if (upm != NULL) {
					sleep(2);
					authenticatedState =upm->checkUserAuthenticity(upm->userId);
				} else {
					cout << "User Profile Monitoring:Client Object was gone!"<< endl;
					authenticatedState = false;
					throw "UPM is null!";
				}
			} catch (exception& e) {
				cout <<"UPM: "<< e.what() << endl;
				delete (UPMonitoring*) object;
				pthread_exit(NULL);
			}
		}
		//set departure time
		gettimeofday(&depTime, NULL);
		time_t departureTime = depTime.tv_sec;
		cout << "User Profile Monitoring:Departure Time: "<< Utilities::convertMilisecondToString(departureTime) << endl;
		upm->uprofile->session->setDepartureTime(departureTime);
		//set time assigned to incated that user has gone
		upm->uprofile->session->setTimeisAssigned(true);
		//store user profile in file
		upm->storeUserModelInStoragePlace(upm);
		//remove already inserted iptables rules
		upm->removeIPTableCommands(upm->IP, upm->Mac);
		cout << "User Profile Monitoring:\" " << upm->userId
				<< " \"'s Thread is going to leave!" << endl;
		//wait for timer pthread
		while(!upm->timerOut);
		//delete the object of upm
		//while(!upm->timerOut);
		delete (UPMonitoring*) object;
		//finalize the thread.
		pthread_exit(NULL);

	} catch (exception& e) {
		cerr << "User Profile Monitoring: Exception occurred: " << e.what()
				<< endl;
		delete (UPMonitoring*) object;
		pthread_exit(NULL);
	}
	//exit from thread, but this object will take place in memory.
	pthread_exit(NULL);
	return NULL;
}

void UPMonitoring::storeUserModelInStoragePlace(UPMonitoring *upm) {
	cout << "Storing Request: " <<endl;
	this->uprofile->setObserverValuesToModel();
}

void UPMonitoring::timer(UPMonitoring *upm) {

	//upm->t.state = true;
	upm->t.duration = Utilities::minute(1); //60 sn
	upm->t.count = 0;
	int rc;

	//CHANGE: thread object was altered.
	if ((rc = pthread_create(&upm->timerObj, NULL,
			&UPMonitoring::timerFunction, (void*) upm))) {
		cout << "User Profile Monitoring:Thread creation failed: " << rc
				<< endl;
	}
}

//TODO:check Timer in terms of time intervalgetBytes
void *UPMonitoring::timerFunction(void* data) {
	pthread_detach(pthread_self());
	UPMonitoring *upm = (UPMonitoring*) data;
	//First meeting
	int restTime = upm->timeInterval- ((upm->uprofile->session->arrivalTime / 60) % 60) % 10;
	if (restTime < 10) {
		//cout<<"!!!restTime:    "<<restTime<<endl;
		usleep(upm->t.duration * restTime);
		upm->t.count = upm->timeInterval;
	}
	string userId=upm->userId;
	while(UPMain::Instance()->checkUserAuthenticityByUID(userId)){

		if(upm->t.count<2){//		if(upm->t.count<upm->timeInterval){
			upm->t.count=upm->t.count+1;
			cout<<"counter"<<upm->t.count<<endl;
		}
		else if (upm->t.count == upm->timeInterval) {
			upm->t.count = 0;
			upm->callTimerTask(upm->Mac);
		}
	}
	upm->timerOut=true;
	pthread_exit(NULL);
}
void UPMonitoring::callTimerTask(string& Mac) {
	printf("User Profile Monitoring:Periodic bandwidth function is invoked.\n");
	if (UPMain::Instance()->checkUserAuthenticityByMAC(Mac)) {
		try {
			if (this->uprofile->tickNumber == 143) {
				cout<<"tick number "<<this->uprofile->tickNumber<<endl;
				this->uprofile->tickNumber = 0;
			}
			this->uprofile->tickNumber++;
			this->uprofile->setObserverValuesToModel();
		} catch (exception& e) {
			cerr << " User Profile Monitoring: " << e.what() << endl;
		}
	}

}
void UPMonitoring::setTickNumber() {
	std::vector<std::string> startTime;
	startTime = Utilities::split(Utilities::getCurrentTimeHM(), ':');
	this->uprofile->tickNumber = atoi(startTime[0].c_str()) * 6
			+ (atoi(startTime[1].c_str()) / this->timeInterval);
	cout << "Tick Number:" <<	this->uprofile->tickNumber << endl;
}


void UPMonitoring::insertIPTableCommands(string& ip, string& mac) {
	removeIPTableCommands(ip, mac);
	cout
			<< "User Profile Monitoring: Iptables rules are being inserted for IP: "
			<< ip << endl;
	std::string in = "iptables -I FORWARD -d " + ip + " -j ACCEPT";
		std::string out = "iptables -I FORWARD -s " + ip + " -j ACCEPT";
	system(in.c_str());
	system(out.c_str());
}
void UPMonitoring::removeIPTableCommands(string& ip, string& mac) {
	cout << "User Profile Monitoring: Iptables rules are being removed for IP: "
			<< ip << endl;

	std::string in = "iptables -D FORWARD -d " + ip +  " -j ACCEPT";
	std::string out = "iptables -D FORWARD -s " + ip + " -j ACCEPT";
	system(in.c_str());
	system(out.c_str());
}

