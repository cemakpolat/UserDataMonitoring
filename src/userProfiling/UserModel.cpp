/*
 * UserModel.cpp
 *
 *  Created on: Feb 21, 2013
 *      Author: cem
 */

#include "UserModel.h"
#include <sstream>


pthread_mutex_t UserModel::mutexFile=PTHREAD_MUTEX_INITIALIZER;

UserModel::UserModel(){

}
const string UserModel::userModelFolderName="Users";
const string UserModel::qoEFolderName="QoExperinces";
int UserModel::observationNumber=144;
UserModel::UserModel(std::string& userName) {

	string path=Utilities::pathFinder(UserModel::userModelFolderName,userId);
	//const char *fileName=(path+".txt").c_str();

	if(this->isUserModelExist(userName)){
		cout<<"Incoming user is recognized, user's data is being fetched"<<endl;
		this->readModel(userName);
	}else{
		cout<<"Incoming user is unknown, new profile is being created"<<endl;
		this->createModel(userName);
	}
}

UserModel::~UserModel() {
	// TODO Auto-generated destructor stub
}

UserModel* UserModel::getUserModel() {
	return this;
}
void UserModel::storeUserModel() {
	Utilities::getNetworkInterface();
	std::string file;
	std::string path;
	Utilities::fileWrite(file,path);
}
bool UserModel::isUserModelExist(std::string& userName){
	bool state=false;
	string path=Utilities::pathFinder(UserModel::userModelFolderName,userName);//TODO: generalize that
	const char *fileName=(path+".txt").c_str();//TODO: error can arise here owing to the const value
	//string fileName=userName+".txt";
	std::ifstream file(fileName);
	if(file){
		state=true;
	}else{
		state=false;
	}
	return state;
}
void UserModel::createModel(std::string& userName){

		this->TCPAverage=0;
		this->UDPAverage=0;
		this->lastDepartureTime="2013/5/8-00:00:00";
		this->authenticationNumber=0;
		this->bandwidthAverage=0;
		this->dailyAuthenticationNumber=0;
		this->visitFrequencyNumber=0;
		this->sessionAverage="00:00:00";
		this->userId="";
		SessionListObject o;
		for( int i=0;i<observationNumber;i++){
			o.bandwidth=0;
			o.visitingFrequency=0;
			this->sessionList.push_back(o);
		}

		cout<<"New User Profile is created "<<endl;
}
void UserModel::readModel(std::string& userName){
	//Read file
	string path=Utilities::pathFinder(UserModel::userModelFolderName,userName);
	//const char *fileName=(path+".txt").c_str();//TODO: error can arise here owing to the const value

	std::string file=(path+".txt");

	pthread_mutex_lock(&mutexFile);
	string in=Utilities::readFile2(file);
	pthread_mutex_unlock(&mutexFile);

	vector<string> v1,v2,v3,v4;
	v1=Utilities::split(in,',');

	for (unsigned int i = 0; i < v1.size(); i++) {
		v2 = Utilities::split(v1[i], '=');
		//cout<<"V2 1 "<<v2[0]<<" "<<v2[1]<<endl;
		if (v2[0] == "UDPAverage") {
			this->UDPAverage=Utilities::convertStringDouble(v2[1]);
		} else if (v2[0] == "TCPAverage") {
			this->TCPAverage=Utilities::convertStringDouble(v2[1]);
		} else if (v2[0] == "lastDepartureTime") {
			this->lastDepartureTime=v2[1];
		} else if (v2[0] == "dailyAuthenticationNumber") {
			this->dailyAuthenticationNumber=Utilities::convertStringDouble(v2[1]);
		} else if (v2[0] == "visitFrequencyNumber") {
			this->visitFrequencyNumber=Utilities::convertStringDouble(v2[1]);
		} else if (v2[0] == "authenticationNumber") {
			this->authenticationNumber=Utilities::convertStringDouble(v2[1]);
		} else if (v2[0] == "bandwidthAverage") {
			this->bandwidthAverage=Utilities::convertStringDouble(v2[1]);
		} else if (v2[0] == "userId") {
			this->userId=v2[1];
		} else if (v2[0] == "sessionAverage") {
			this->sessionAverage=v2[1];
		} else if (v2[0] == "sessions") {
			SessionListObject *o=new SessionListObject();
			v3=Utilities::split(v2[1],'|');
			for (unsigned int i = 0; i < v3.size(); i++) {
				v4=Utilities::split(v3[i],';');
				o->visitingFrequency=Utilities::convertStringDouble(Utilities::split(v4[0],':')[1]);
				o->bandwidth=Utilities::convertStringDouble(Utilities::split(v4[1],':')[1]);
				this->sessionList.push_back(*o);
			}
			delete o;
		}
	}
}
void UserModel::writeModelInFile(std::string& userId){

	//this->lastDepartureTime=Utilities::getCurrentDateAndTimeAsString();
	this->userId=userId;
	cout<<"USERMODEL: userId"<<this->userId<<endl;
	cout<<"USERMODEL: dailyAuthenticationNumber"<< Utilities::long_to_String(this->dailyAuthenticationNumber)<<endl;
	cout<<"USERMODEL: sessionAverage"<<this->sessionAverage<<endl;
	cout<<"USERMODEL: authenticationNumber"<<this->authenticationNumber<<endl;
	cout<<"USERMODEL: bandwidthAverage"<<this->bandwidthAverage<<endl;
	//this->userId=userId;

	std::string toWrite = "TCPAverage=" +Utilities::convertDoubleToString( this->TCPAverage) + "," + "UDPAverage="
			+ Utilities::convertDoubleToString(this->UDPAverage) + "," + "lastDepartureTime="
			+ this->lastDepartureTime + "," + "dailyAuthenticationNumber="
			+ Utilities::long_to_String(this->dailyAuthenticationNumber) + ","
			+ "visitFrequencyNumber="
			+ Utilities::long_to_String(
					this->visitFrequencyNumber) + "," + "authenticationNumber="
			+ Utilities::long_to_String(this->authenticationNumber) + "," + "bandwidthAverage="
			+ Utilities::convertDoubleToString(this->bandwidthAverage) + "," + "userId=" + this->userId + ","
			+ "sessionAverage=" + this->sessionAverage + ","+"sessions=";
	string vec="";
	unsigned int listSize=sessionList.size();
	for(unsigned int i=0;i<listSize;i++){
		if(listSize-1>i){
			vec = vec + "VF:"
					+ Utilities::convertDoubleToString(
							sessionList[i].visitingFrequency) + ";" + "BW:"
					+ Utilities::convertDoubleToString(sessionList[i].bandwidth)
					+ "|";
		}
		else {
			vec = vec + "VF:"
					+ Utilities::convertDoubleToString(
							sessionList[i].visitingFrequency) + ";" + "BW:"
					+ Utilities::convertDoubleToString(
							sessionList[i].bandwidth);
		}
	}
	toWrite=toWrite+vec;
	string path=Utilities::pathFinder(UserModel::userModelFolderName,userId);
	std::string file=(path+".txt");
	//string file=userId;
//	cout<<toWrite<<endl;

	pthread_mutex_lock(&mutexFile);
	Utilities::fileWrite(file,toWrite);
	pthread_mutex_unlock(&mutexFile);

}


