/*
 * UserAuthListObject.h
 *
 *  Created on: 04.08.2012
 *      Author: cem
 */

#ifndef USERAUTHLISTOBJECT_H_
#define USERAUTHLISTOBJECT_H_

#include <string>
#include "UserProfiler.h"

class UserAuthListObject {
private:

public:
	std::string Mac;
	std::string userId;
	std::string ip;
	std::string token;
	//bool active;
	UserProfiler *uprofile;
	UserAuthListObject();
	UserAuthListObject(std::string ip,std::string Mac,std::string token);
	UserAuthListObject(std::string Mac, std::string userId, std::string ip, std::string token);
	~UserAuthListObject();
};


#endif /* USERAUTHLISTOBJECT_H_ */
