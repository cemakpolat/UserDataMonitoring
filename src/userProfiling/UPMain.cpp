/*
 * UPMain.cpp
 *
 *  Created on: 27.07.2012
 *      Author: cem
 */
#include <stdio.h>
#include <string>

#include <pthread.h>
#include "UserAuthListObject.h"
#include "AuthenticationPlatformHAPD.h"
#include "UPMain.h"
#include <iostream>

#include "Utilities.h"
using namespace std;

//TODO: implement exceptions

//vectors should be defined in order to be active, because they are dependent on upmain object
vector<UserAuthListObject> UPMain::userAuthList;

UPMain::UPMain(){

}
UPMain::~UPMain(){

}
void* UPMain::run(void* object) {
	UPMain *upm=(UPMain*)object;
	//upm->upmain();
	printf("User Profile Block started\n");
	//	upm->mutex1 = PTHREAD_MUTEX_INITIALIZER;
	       //mutex1 = PTHREAD_MUTEX_INITIALIZER;
		upm->mutex1.__m_reserved = 0;
		upm->mutex1.__m_count = 0;
		upm->mutex1.__m_owner = 0;
		upm->mutex1.__m_kind = PTHREAD_MUTEX_ADAPTIVE_NP;
		upm->mutex1.__m_lock.__status = 0;
		upm->mutex1.__m_lock.__spinlock = 0;

		//upm->mutexFile=PTHREAD_MUTEX_INITIALIZER;
		upm->mutexFile.__m_reserved = 0;
		upm->mutexFile.__m_count = 0;
		upm->mutexFile.__m_owner = 0;
		upm->mutexFile.__m_kind = PTHREAD_MUTEX_ADAPTIVE_NP;
		upm->mutexFile.__m_lock.__status = 0;
		upm->mutexFile.__m_lock.__spinlock = 0;





		pthread_t authentication;
		AuthenticationPlatformHAPD *auhtPl=AuthenticationPlatformHAPD::Instance();


		int rc2;
		if ((rc2 = pthread_create(&authentication, NULL,
				&AuthenticationPlatformHAPD::run, auhtPl))) {
			printf("User Profile Block:Thread creation failed: %d\n", rc2);
			int attempt = 0;
			while (attempt < 3) {
				bool success = true;
				Utilities::randomBackOfftime(7);
				if ((rc2 = pthread_create(&authentication, NULL,&AuthenticationPlatformHAPD::run, auhtPl))) {
					success = false;
					printf("User Profile Block:Thread creation failed: %d\n", rc2);
					if (attempt == 2) {
						printf("User Profile Block:Thread creation failed,please re-start the program. \n");
					}
				}
				if (success == true) {
					attempt = 3;
				}
			}
		}
		pthread_join(authentication,NULL);
		pthread_exit(NULL);

	return NULL;
}


//singleton pattern required
UPMain* UPMain::m_pInstance=NULL;
UPMain* UPMain::Instance()
{
   if (!m_pInstance)   // Only allow one instance of class to be generated.
      m_pInstance = new UPMain();
   return m_pInstance;
}

/*------------------|UPMain Functionalities for MP|-------------------*/

/**
 * Get the rest of the bandwidth usage for the current time.
 * @totalBandwidth indicates the maximum bandwidth amount for an AP.
 */
//void UPMain::availableBandwidth(double * available){
//	//double totalBandwidth=totalBW;//should be transported in configParameters
//	double restBW=0;
//	int timeInterval=10;
//	std::vector<std::string> startTime;
//	startTime = Utilities::split(Utilities::getCurrentTimeHM(), ':');
//	int placeOfTic = atoi(startTime[0].c_str()) * 6+ (atoi(startTime[1].c_str()) / timeInterval);
//
//	vector<SessionListObject> list;
//	pthread_mutex_lock(&UPMain::Instance()->mutex1);
//	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
//		list=UPMain::Instance()->userAuthList[i].uprofile->umodel->sessionList;
//		restBW=restBW+list[placeOfTic].bandwidth;
//
//	}
//	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
//	available=&restBW;
//
//}
/**
 * Get the rest of the bandwidth usage for the current time.
 * @totalBandwidth indicates the maximum bandwidth amount for an AP.
 */
void UPMain::availableBandwidth(double * available){
	//double totalBandwidth=totalBW;//should be transported in configParameters
	double restBW=0;
	int timeInterval=10;
	double consumedDownload=0;
	double consumedUpload=0;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		UPMain::Instance()->userAuthList[i].uprofile->bandwidth->calculateTotalBandwidth(UPMain::Instance()->userAuthList[i].ip);
		consumedDownload=consumedDownload+UPMain::Instance()->userAuthList[i].uprofile->bandwidth->totalDownloadBW;
		consumedUpload=consumedUpload+UPMain::Instance()->userAuthList[i].uprofile->bandwidth->totalUploadBW;

	}
	cout<<"UPMain: Total Consumed Download:"<<consumedDownload<<endl;
	cout<<"UPMain: Total Consumed Upload:"<<consumedUpload<<endl;
	restBW=consumedDownload;
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	available=&restBW;

}
/*-----------|Authentication and User related Functionalities|----------*/

/*User Authentication List Functionalities*/
int UPMain::getAuthenticatedUserCount(){
	int i=0;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	i=UPMain::Instance()->userAuthList.size();
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return i;
}

string UPMain::getUserNameByIP(string& IP) {
	std::string userId="";
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].ip==IP) {
			userId=UPMain::Instance()->userAuthList[i].userId;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userId;
}

string UPMain::getUserNameByMAC(string& MAC) {
	std::string userId="";
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].Mac==MAC) {
			userId=UPMain::Instance()->userAuthList[i].userId;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userId;
}

string UPMain::getUserMAC(string& IP) {
	std::string userMac="";
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].ip==IP ) {
			userMac=UPMain::Instance()->userAuthList[i].Mac;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userMac;
}

string UPMain::getUserToken(string& IP) {
	std::string userToken="";
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].ip==IP  ) {
			userToken= UPMain::Instance()->userAuthList[i].token;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userToken;
}

bool UPMain::checkUserAuthenticityByUID(string& userId) {
	bool state=false;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].userId==userId  ) {
			state=true;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	//std::cout<<"UP: User state "<<state<<std::endl;
	return state;
}
bool UPMain::checkUserAuthenticityByIP(string& IP ) {
	bool state=false;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].ip==IP  ) {
			state=true;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return state;
}

bool UPMain::checkUserAuthenticityByMAC(string& mac) {
	bool state=false;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
		if (UPMain::Instance()->userAuthList[i].Mac==mac   ) {
			state=true;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	//std::cout<<"UP: User state "<<state<<std::endl;
	return state;
}

