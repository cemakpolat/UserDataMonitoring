/*
 * CommLang.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: cem
 */

#include "CommLang.h"
#include <iostream>
#include "Utilities.h"

CommLang::CommLang() {

}


void CommLang::loadMessage(std::string message){
	this->ip="";this->message="";this->port=0;
	std::vector<std::string> v1,v2;
	v1=Utilities::split(message,',');
	for(unsigned int i=0;i<v1.size();i++){
		v2=Utilities::split(v1[i],':');
		if(v2[0]=="ip"){
			this->ip=v2[1];
		}else if(v2[0]=="serverPort"){
			this->port=Utilities::convertStringToIntCPP(v2[1]);
		}else if(v2[0]=="message"){
			this->message=v2[1];
		}
		else if(v2[0]=="request"){
			this->requestNum=Utilities::convertStringToIntCPP(v2[1]);
		}
		else if(v2[0]=="answer"){
			this->message=v2[1];
		}
		else if(v2[0]=="response"){
			this->message=v2[1];//	this->receivedMes.requestNum=Utilities::convertStringToIntCPP(v2[1]);
		}
		else if(v2[0]=="messageType"){
			this->messageType=v2[1];
		}
		else if(v2[0]=="incentive"){
			this->incentive=Utilities::convertStringDouble(v2[1]);
		}
		else if(v2[0]=="numberOfClients"){
			this->numberOfClients=Utilities::convertStringToIntCPP(v2[1]);
		}
	}
}

std::string CommLang::createRequestMesObj(int port,std::string& ip,int request) {
	std::string packets;
	packets="ip:"+ip+","+"serverPort:"+Utilities::convertIntToString(port)+","+"request:"+Utilities::convertIntToString(request);
	return packets;

}

std::string CommLang::createResponseMesObj(
		std::string response) {
	std::string packets;
	packets="response:"+response;//	packet["response"] = json::String(response);

	return packets;

}

std::string CommLang::createOfferMesToAP(std::string ip,std::string messageType,int numberOfClients,double incentive) {
	std::string packets;
	packets="ip:"+ip+","+"messagetype:"+messageType+","+"numberOfClients:"+Utilities::convertIntToString(numberOfClients)+","+
			"incentive:"+Utilities::convertDoubleToString(incentive);
	return packets;

}
std::string CommLang::createOfferMesToAP1(std::string& ip, std::string& offer) {
	std::string packets;
	packets="ip:"+ip+","+"offer:"+offer;//	packet["offer"] = json::String(offer);

	return packets;

}
std::string CommLang::createMessageObjToClient(std::string& ip,
		std::string& message) {
	std::string packets;
	packets="ip:"+ip+","+"message:"+message;
	return packets;

}
std::string CommLang::createGenericMessage(std::string& message) {
	std::string packets;
	packets="message:"+message;
	return packets;

}
