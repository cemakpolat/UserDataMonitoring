/*
 * BlackBoard.h
 *
 *  Created on: 07.08.2012
 *      Author: cem
 */

#ifndef BLACKBOARD_H_
#define BLACKBOARD_H_
#include <string>
class BlackBoard{
public:
	//static const void writeConsole( const std::string& line);
	//static const void writeConsole( std::string& line);
	static void writeConsole(const std::string& line){
		std::cout<<line<<std::endl;
	}
	static void writeConsole(std::string& line){
		std::cout<<line<<std::endl;
	}
};



#endif /* BLACKBOARD_H_ */
