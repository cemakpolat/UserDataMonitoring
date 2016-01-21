/*
 * ITGChecker.h
 *
 *  Created on: 10.08.2012
 *      Author: cem
 */

#ifndef ITGCHECKER_H_
#define ITGCHECKER_H_
class ITGChecker{
private:
	bool continueHandling;
public:
	ITGChecker();
	~ITGChecker();
	static void* run(void*);
	//void reStartITGRecv();
	//void itgMain();
	void randomBackOfftime();
	//void handleITGRecv();
	void runITG();
	void killITG();
};



#endif /* ITGCHECKER_H_ */
