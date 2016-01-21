/*
 * UserSession.cpp
 *
 *  Created on: 07.08.2012
 *      Author: cem
 */
#include "UserSession.h"
#include "Utilities.h"


UserSession::UserSession() {

}
UserSession::~UserSession() {

}
void UserSession::calculateSessionTime() {

	this->sessionTime=difftime(this->departureTime,this->arrivalTime);
	std::cout<<"User Session:Arrival Time: "<<Utilities::getTimeFromGivenTime(this->arrivalTime)<<std::endl;
	std::cout<<"User Session:Departure Time: "<<Utilities::getTimeFromGivenTime(this->departureTime)<<std::endl;
}
void UserSession::calculateSessionAverage() {
	this->calculateSessionTime();
	this->sessionAverage=(this->sessionAverage*(this->authenticationNumber-1)+this->sessionTime)/this->authenticationNumber;
	std::cout<<"User Session:Session Time: "<<this->sessionAverage<<std::endl;
}
void UserSession::calculateVisitingFrequency( unsigned int place,UserModel* userModel) {

	try{
		for (unsigned int j = 0; j < userModel->sessionList.size(); j++) {

			this->visitingFrequency =userModel->sessionList[j].visitingFrequency;
			if (j == place) {
				this->visitingFrequency = (((this->visitFrequencyNumber-1) * this->visitingFrequency) + 1 )/ this->visitFrequencyNumber;
				userModel->sessionList[j].visitingFrequency=this->visitingFrequency;
			}else{
				this->visitingFrequency = ((this->visitFrequencyNumber-1) * this->visitingFrequency )/ this->visitFrequencyNumber;
				userModel->sessionList[j].visitingFrequency=this->visitingFrequency;

			}
		}
	}
	catch(exception& e){
		cerr<<"User Session: "<<e.what()<<endl;
	}
}/*
Compare the last departure time's tick number with the new tick number. If they are same, return true else false;
*/
bool UserSession::tickChecker(int ticknumber){
	bool state=false;
	std::vector<std::string> startTime;
	startTime = Utilities::split(Utilities::getTimeFromGivenTime(this->lastDepartureTime), ':');
	int previousTickNumber = atoi(startTime[0].c_str()) * 6
				+ (atoi(startTime[1].c_str()) / 10);

	cout << "Previous Tick Number:" << previousTickNumber << endl;
	if(ticknumber==previousTickNumber){
		state=true;//means tick are same
	}else{
		state=false;
	}
	return state;
}

void UserSession::addListToJSON(UserProfiler *upm) {
	try {

		time_t currentTime;
		currentTime=time(NULL);
		this->checkDailyAuthNum(currentTime,this->lastDepartureTime);

		cout<<" addListToJSON List size "<<upm->umodel->sessionList.size()<<endl;

		upm->bandwidth->sessionBandwidthAveragePerMin=upm->umodel->sessionList[upm->tickNumber].bandwidth;
		upm->bandwidth->calculateSessionBandwidthAveragePerMin(upm);

		upm->umodel->sessionList[upm->tickNumber].bandwidth=upm->bandwidth->sessionBandwidthAveragePerMin;

		if(!this->tickChecker(upm->tickNumber)){
			this->visitFrequencyNumber=this->visitFrequencyNumber+1;//increase one at each tick.
			upm->umodel->visitFrequencyNumber=this->visitFrequencyNumber;
			this->calculateVisitingFrequency(upm->tickNumber,upm->umodel);
		}

	} catch (std::exception& e) {
		cerr << "User Session: " << e.what() << endl;
	}
}
void UserSession::setParametersToUserModel(UserProfiler* upm) {
	try {
			std::cout<<"US: Storage Request for user  "<<upm->userid<<std::endl;

			upm->umodel->authenticationNumber=this->authenticationNumber;
			//Utilities::errorCatcher("UserSession setParametersToUserModel",2);

			this->addListToJSON(upm);

			if(this->isTimeAssigned()){
				this->calculateSessionAverage();
				upm->umodel->sessionAverage=Utilities::getTimeFromGivenTime(this->sessionAverage);
				upm->umodel->dailyAuthenticationNumber=this->dailyAuthenticationNumber;
				upm->umodel->lastDepartureTime=Utilities::getCurrentDateAndTimeAsString();

				cout<<"Session Average"<<upm->umodel->sessionAverage<<endl;
				cout<<"Session Average"<<upm->umodel->dailyAuthenticationNumber<<endl;
				cout<<"Session Average"<<upm->umodel->lastDepartureTime<<endl;


			}
		} catch(exception& e){
			cerr<<"User Session: "<<e.what()<<endl;
		}
}

void UserSession::getParametersFromUserModel(UserModel* userModel) {
	try {
		//Utilities::errorCatcher("UserSession.getParametersFromUserModel",1);
		this->authenticationNumber=userModel->authenticationNumber;
		this->visitFrequencyNumber=userModel->visitFrequencyNumber;
		this->sessionAverage=Utilities::convertStringToMilisecond(userModel->sessionAverage);
		this->lastDepartureTime=Utilities::convertDateTimeStringToMilisecond(userModel->lastDepartureTime);//converting should be performed
		this->dailyAuthenticationNumber=userModel->dailyAuthenticationNumber;

		//cout<<"Daily Authentication Number: "<<this->dailyAuthenticationNumber<<endl;

		time_t currentTime;
		currentTime=time(NULL);
		this->checkDailyAuthNum(currentTime,this->lastDepartureTime);
		//cout<<"Daily Authentication Number: "<<this->dailyAuthenticationNumber<<endl;

	}catch(exception& e){
		cerr<<"User Session: "<<e.what()<<endl;
	}

}

void UserSession::setArrivalTime(time_t arrivalTime) {
	this->arrivalTime=arrivalTime;
}
void UserSession::setDepartureTime(time_t departureTime) {

	this->departureTime=departureTime;
}
void UserSession::setTimeisAssigned(bool isTimeAssigned) {
	this->timeAssigned=isTimeAssigned;
}
bool UserSession::isTimeAssigned() {
	return this->timeAssigned;
}
long UserSession::getAuthenticationNumber() {
	return this->authenticationNumber;
}
void UserSession::setAuthenticationNumber(long authenticationNum) {
	this->authenticationNumber=authenticationNum;
}
void UserSession::checkDailyAuthNum(time_t now,time_t lastDepart) {
	//get days according to the given data.

	struct tm * timeinfo1;
	timeinfo1 = localtime(&now);

	struct tm * timeinfo2;
	timeinfo2 = localtime(&lastDepart);
	int day1=timeinfo1->tm_mday;
	int day2=timeinfo2->tm_mday;
    int month1=timeinfo1->tm_mon+1;
    int month2=timeinfo2->tm_mon+1;

   // int year1=timeinfo1->tm_year+1900;//if needed we can add also year but it is not required in normal case.
    //int year2=timeinfo1->tm_year+1900;

	if (this->dailyAuthenticationNumber == 0) {//first meeting
		this->dailyAuthenticationNumber = this->dailyAuthenticationNumber + 1;
		cout<<"Daily Authentication Number is increased to: "<<this->dailyAuthenticationNumber<<endl;
	}


	if (day1 != day2) {
		this->dailyAuthenticationNumber = this->dailyAuthenticationNumber + 1;
	} else if (day1==day2 && month1!=month2){
        this->dailyAuthenticationNumber = this->dailyAuthenticationNumber + 1;
	}
	//    else if (day1==day2 && month1==month2 && year1!=year2){
	//        this->dailyAuthenticationNumber = this->dailyAuthenticationNumber + 1;
	//		this->dailyVFProbability = 0;
	//        this->lastDepartureTime=now;//I added here
	//
	//        //	std::cout << "days are  equal days:" << +day1 << " " << day2<< std::endl;
	//	}

}

