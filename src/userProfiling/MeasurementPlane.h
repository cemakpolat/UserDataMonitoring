/*
 * MeasurementPlane.h
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#ifndef MEASUREMENTPLANE_H_
#define MEASUREMENTPLANE_H_

#include <string>
#include "TCPConnection.h"

class MeasurementPlane {
private:
	static MeasurementPlane* m_mpInstance;
protected:
public:

	static int userNumber;

	//TCPConnection *con;

	static MeasurementPlane* Instance();
	static void* run(void* object);
	static void* listenRM(void*);
	static void* runQoSResponder(void*);
	void sendToRMMessage(long& uplink_bandwidth,long& available_bandwidth);

	void recv_mm_messages();
	void do_network_status_work(double * uplink, double * available);

	MeasurementPlane();
	~MeasurementPlane();
};


#endif /* MEASUREMENTPLANE_H_ */

