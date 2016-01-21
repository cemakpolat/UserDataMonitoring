/*
 * UserAuthListObject.cpp
 *
 *  Created on: 27.07.2012
 *      Author: cem
 */

#include "UserAuthListObject.h"

UserAuthListObject::UserAuthListObject(std::string userId,std::string ip,std::string Mac,std::string token){
	this->Mac=Mac;
	this->userId=userId;
	this->token=token;
	this->ip=ip;
	//this->active=false;
	this->uprofile=NULL;//TODO: that can give error
}
UserAuthListObject::UserAuthListObject(std::string ip,std::string Mac,std::string token){
	this->Mac=Mac;
	this->userId="";
	this->token=token;
	this->ip=ip;
	this->uprofile=NULL;//TODO: that can give error
}
UserAuthListObject::UserAuthListObject(){

}
UserAuthListObject::~UserAuthListObject(){

}
