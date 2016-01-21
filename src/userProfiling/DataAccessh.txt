/*
 * DataAccess.h
 *
 *  Created on: 06.08.2012
 *      Author: cem
 */

#ifndef DATAACCESS_H_
#define DATAACCESS_H_

#include <string>
#include "writer.h"
#include "elements.h"
#include "reader.h"
#include <pthread.h>
using namespace std;
using namespace json;
class DataAccess{

private:
	static const string userModelFolderName;
	static const string qoEFolderName;
	static  pthread_mutex_t mutexFile;
public:
	string pathFinder(string folderName,string fileName);
	void storeUserModel(Object& userModel,string& userId);
	void storeQoEInfo(string& time,Object& qoe,string& userId);
	Object getUserQoE(string& userId);
	Object getUserModel(string& userId);
	void getUserModel(string& userId,json::Object& up);
	void getUM(string& userId,Object& um);
	void synchronizedStoringData(Object& userModel,string& userId);
	void synchronizedGettingData(string& userId);

};


#endif /* DATAACCESS_H_ */
