/*
 * MeasurementPlane.cpp
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */
#include "PracticalSocket.h"
#include "MeasurementPlane.h"
#include "UPMain.h"
#include "QoS.h"
#include <stdio.h>
#include "Utilities.h"
#include "CommLang.h"
#include "Definitions.h"
#include "AuthenticationPlatformHAPD.h"
extern "C"{
	#include "uloop_message_api.h"
}

////////// SET TIME ON OWRT  by using the following command line ->date -s 2012.09.24-15:53

MeasurementPlane* MeasurementPlane::m_mpInstance = NULL;
MeasurementPlane* MeasurementPlane::Instance() {
	if (!m_mpInstance) // Only allow one instance of class to be generated.
		m_mpInstance = new MeasurementPlane();
	return m_mpInstance;
}
int MeasurementPlane::userNumber=0;

MeasurementPlane::MeasurementPlane() {

}
MeasurementPlane::~MeasurementPlane() {

}
/**
 * Main Component which starts every sub elements
 */
void* MeasurementPlane::run(void* object){
	pthread_detach(pthread_self());

	pthread_t listenRM,messageRequester;
	MeasurementPlane *mp = (MeasurementPlane*) object;

	int rc1;
	if ((rc1 = pthread_create(&listenRM, NULL,&mp->listenRM, NULL))) {
		printf("Measurement Plane:Thread creation failed: %d\n", rc1);
	}

	if ((rc1 = pthread_create(&messageRequester, NULL,&mp->runQoSResponder, NULL))) {
			printf("Measurement Plane:Thread creation failed: %d\n", rc1);
		}

	pthread_join(listenRM,NULL);
	pthread_join(messageRequester,NULL);

	pthread_exit(NULL);
}
/*
 *Thread: Listen Messages being transmitted by other Modules
 */
void* MeasurementPlane::listenRM(void*)
{
	pthread_detach(pthread_self());
	try{
	//TODO: Can raise an issue
		cout<<"MeasurementPlane : MEM Module is being started..."<<endl;
		low_init_module(MEM_MODULE);
		MeasurementPlane::Instance()->recv_mm_messages();
		close_module(MEM_MODULE);
	}catch(exception &e){
		cerr<<"MeasurementPlane:"<<e.what()<<endl;
		close_module(MEM_MODULE);
		MeasurementPlane::listenRM(NULL);
	}

	pthread_exit(NULL);
}

/*
 * Receive multiple messages example
 */
void MeasurementPlane::recv_mm_messages()
{

	UloopMessage *ulm1 = NULL;
	int conn=-1;
   while(true){

		 conn = low_recv_uloop_unix_msg(MEM_MODULE,&ulm1);

		double token = 0.0;
		struct cryptoid	cid;

		double uplink;
		double available;
		string pktString;
		string obj;
		double val=0;
		double averageDelay=0,averageBitrate=0,averageJitter=0,averagePacketLoss=0;
		char measure_what[50];
		cout<<"=========== MEM Module ============="<<endl;
		if (ulm1 != NULL) {

				switch (uloop_message_type(ulm1)) {
				case ULOOP_MESSAGE_TYPE__ULOOP_QOSMESSAGE:

					/* parse received message as a measurement request */
					parse_measurement_request(ulm1, measure_what);
					//fprintf(stderr, "MEM==Request: %s\n", measure_what);

					//receive message
					pktString = measure_what;
					if (pktString == "") {
						cout << "Measurement Plane: Request Content is empty"
								<< endl;

					} else {

						const unsigned int reqNum = Utilities::convertStringToInt(
								pktString);
						cout<<"Request:";
						switch (reqNum) {
						case CommLang::getAuthenticatedUserCount:
							cout << "Authenticated User Numer:" << reqNum << endl;
							val = UPMain::Instance()->getAuthenticatedUserCount();
							//cout<<"MP: val"<<Utilities::convertDoubleToString(val)<<endl;
							send_measurement_request(conn,Utilities::convertDoubleToString(val).c_str());
							//send_measurement_request(conn,"val");

							break;
						case CommLang::getCongestedUsers:
							cout << "Congested Users:" << reqNum << endl;
							obj = QoS::Instance()->congestedusers();
							send_measurement_request(conn,
									Utilities::convertDoubleToString(val).c_str());
							break;
						case CommLang::getWorstCongestedUser:
							cout << "Worst Congested User:" << reqNum << endl;

							obj = QoS::Instance()->worstCongestedUser();
							send_measurement_request(conn,
									Utilities::convertDoubleToString(val).c_str());
							break;
						case CommLang::getAllQoSMeasurements:
							cout << "QoS Measurements:" << reqNum << endl;

							 QoS::Instance()->qoSMeasurements(averageDelay,averageBitrate,averageJitter,averagePacketLoss);
							send_average_qos_measurements(conn, "IP",
									Utilities::convertDoubleToString(averageDelay).c_str(),
									Utilities::convertDoubleToString(averageBitrate).c_str(),
									Utilities::convertDoubleToString(averageJitter).c_str(),
									Utilities::convertDoubleToString(
											averagePacketLoss).c_str());
							break;
						default:
							cout << "Undefined rule Request Number!" << endl;
							break;
						}
					}
					break;
				case ULOOP_MESSAGE_TYPE__ULOOP_NEWUSERDETAILS:
					parse_new_user_details(ulm1, &cid, &token);
					//do_user_details_work(); /* Fake function */
					AuthenticationPlatformHAPD::Instance()->receiveRMMessage(cid,token);
					break;
				case ULOOP_MESSAGE_TYPE__ULOOP_NETWORKSTATUS:
					parse_network_status_mem_request(ulm1);
					do_network_status_work(&uplink, &available); /* Fake function */
					send_network_status_mem_reply(conn, uplink, available);
					break;
				default:
					printf("MEM=MeasurementPlane: Undefined Message");
					break;
			}
		}

		if(ulm1){
				free_uloop_message(ulm1);
		}
		close_connection(conn);
		cout<<"===================================="<<endl;

	}

}
//TODO: pointer error !!!
void MeasurementPlane::do_network_status_work(double * uplink, double * available)
{
	*uplink = 100000;
	//*available = 50000;
	UPMain::Instance()->availableBandwidth(available);
}

/*
 * Run QoS Responder Tthread. The QoS Rquest will be received from LB Uloop-Component.
 */
void* MeasurementPlane::runQoSResponder(void* ) {
	pthread_detach(pthread_self());

	sleep(2);
	cout<<"MeasurementPlane QoS Module is being started"<<endl;

		try {
			UloopMessage *ulm = NULL;
			int conn = -1;

			/* start the ULOOP message module */
			low_init_module(QOS_MODULE);
			//init_module(QOS_MODULE);

			double token = 0.0;
			struct cryptoid	cid;

			double uplink;
			double available;
			string pktString;
			double averageDelay=0,averageBitrate=0,averageJitter=0,averagePacketLoss=0;
			char measure_what[50];
			 unsigned int reqNum=0;
			 string obj;
			double val=0;
			while (true) {

				cout<<"=========== QoS Module ============="<<endl;
				/* recv uloop message on the pointer, and return the open socket connection id */

				conn = low_recv_uloop_unix_msg(QOS_MODULE,&ulm);
			//	conn = recv_uloop_unix_msg(&ulm);

				if (ulm != NULL) {
					switch (uloop_message_type(ulm)) {
					case ULOOP_MESSAGE_TYPE__ULOOP_QOSMESSAGE:
						/* parse received message as a measurement request */
							parse_measurement_request(ulm, measure_what);
							//fprintf(stderr, "QOS==Request: %s\n", measure_what);

							//receive message
							 pktString=measure_what;

							 if (pktString=="" ) {
								 cout<<"Measurement Plane: Request Content is empty"<<endl;

							 } else {

								 //WE RECEIVE ONLY 'REQUEST MESSAGE OBJECT' AND SENDING ONLY 'RESPONSE MESSAGE OBJECT'

								reqNum=Utilities::convertStringToInt(pktString);
								//double averageDelay=0,averageBitrate=0,averageJitter=0,averagePacketLoss=0;
								cout<<"Request:";
								switch (reqNum) {
									case CommLang::getAuthenticatedUserCount:
										cout<<"Authenticated User Number"<<endl;
										val = UPMain::Instance()->getAuthenticatedUserCount();
										send_measurement_request(conn,Utilities::convertDoubleToString(val).c_str());
										break;
									case CommLang::getCongestedUsers:
										cout << "Congested Users:" << reqNum << endl;
										obj = QoS::Instance()->congestedusers();
										send_measurement_request(conn, Utilities::convertDoubleToString(val).c_str());
										break;
									case CommLang::getWorstCongestedUser:
										cout << "Worst Congested User:" << reqNum << endl;
										obj = QoS::Instance()->worstCongestedUser();
										send_measurement_request(conn, obj.c_str());
										cout<<"Worst congested User: "<<obj.c_str()<<endl;
										break;

									case CommLang::getAllQoSMeasurements:
										  cout<<"QoS Measurements"<<endl;
										  QoS::Instance()->qoSMeasurements(averageDelay,averageBitrate,averageJitter,averagePacketLoss);

										send_average_qos_measurements(conn, "IP",
											Utilities::convertDoubleToString(averageDelay).c_str(),
											Utilities::convertDoubleToString(averageBitrate).c_str(),
											Utilities::convertDoubleToString(averageJitter).c_str(),
											Utilities::convertDoubleToString(
													averagePacketLoss).c_str());
										break;
									default:
										cout << "Undefined rule Request Number!" << endl;
										break;
								}
							 }
								break;
							case ULOOP_MESSAGE_TYPE__ULOOP_NEWUSERDETAILS:
								parse_new_user_details(ulm, &cid, &token);
								AuthenticationPlatformHAPD::Instance()->receiveRMMessage(cid,token);
								break;
							case ULOOP_MESSAGE_TYPE__ULOOP_NETWORKSTATUS:
								parse_network_status_mem_request(ulm);
								MeasurementPlane::Instance()->do_network_status_work(&uplink, &available); /* Fake function */
								send_network_status_mem_reply(conn, uplink, available);
								break;
							default:
								printf("MeasurementPlane: Undefined Message");
								break;
						}
				}

			/* release the received message */
			if (ulm){
				free_uloop_message(ulm);
			}
			/* close the received connection */
			close_connection(conn);
			cout<<"===================================="<<endl;
		}
	}
	catch (exception& e) {
		cerr<<"Measurement Plane: "<<e.what()<<endl;
		close_module(QOS_MODULE);
		MeasurementPlane::run(NULL);

	}

	/* close the ULOOP message module */
	close_module(QOS_MODULE);
	pthread_exit(NULL);
}

int main() {

	pthread_t qoeThread, upmainThread, mpThread;
	MeasurementPlane *mp = MeasurementPlane::Instance();
	QoS *qos = QoS::Instance();
	UPMain *upmain = UPMain::Instance();
	int rc1, rc2, rc3;

	//start upmain
	if ((rc1 = pthread_create(&upmainThread, NULL, &UPMain::run, upmain))) {
		printf("Measurement Plane:Thread creation failed: %d\n", rc1);
		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc1 = pthread_create(&upmainThread, NULL, &UPMain::run, upmain))) {

				success = false;
				printf("Measurement Plane:Thread creation failed: %d\n", rc1);
				if (attempt == 2) {
					printf(
							"Measurement Plane:Thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}
	//start QoETracker
	if ((rc2 = pthread_create(&qoeThread, NULL, &QoS::run, qos))) {
		printf("QoETracker Plane:Thread creation failed: %d\n", rc2);
		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc2 = pthread_create(&qoeThread, NULL, &QoS::run, qos))) {
				success = false;
				printf("Measurement Plane:Thread creation failed: %d\n", rc2);
				if (attempt == 2) {
					printf(
							"Measurement Plane:Thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}
	//start Listener
	if ((rc3 = pthread_create(&mpThread, NULL, &MeasurementPlane::run, mp))) {
		printf("Measurement Plane:Thread creation failed: %d\n", rc3);

		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc3 = pthread_create(&mpThread, NULL, &MeasurementPlane::run,
					mp))) {

				success = false;
				printf("Measurement Plane:Thread creation failed: %d\n", rc3);
				if (attempt == 2) {
					printf(
							"Measurement Plane:Thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}


	pthread_join(upmainThread, NULL);
	pthread_join(qoeThread, NULL);
	pthread_join(mpThread, NULL);
	return 0;
}

