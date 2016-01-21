CONN_MODULES_FILES = conn/PracticalSocket conn/TCPConnection
QOS_MODULES_FILES = qos/QoS qos/QoSTracker
USERPROFILLING_MODULES_FILES = userProfiling/AuthenticationPlatformHAPD userProfiling/MeasurementPlane userProfiling/UPMain userProfiling/UPMonitoring \
				userProfiling/UserAuthListObject userProfiling/UserBandwidth \
				userProfiling/UserModel userProfiling/UserSession userProfiling/UserProfiler
UTILS_MODULES_FILES= utils/CommLang

MISSING_HEADERS = -I utils/ -I dme/ -I conn/ -I userProfiling/ -I qos/ -I $(STAGING_DIR)/usr/include

REMOVE = rm -rf

BINARY = MM

all: binary

binary: compile_conn_modules compile_qos_modules compile_userprofilling_modules compile_utils_modules 
	$(CXX) -o $(BINARY) \
	conn/PracticalSocket.o conn/TCPConnection.o \
	userProfiling/AuthenticationPlatformHAPD.o userProfiling/MeasurementPlane.o userProfiling/UPMain.o userProfiling/UPMonitoring.o \
	userProfiling/UserAuthListObject.o userProfiling/UserBandwidth.o userProfiling/UserProfiler.o \
	userProfiling/UserModel.o userProfiling/UserSession.o \
	qos/QoS.o qos/QoSTracker.o \
	utils/CommLang.o \
        -l pthread -Wl,-rpath-link=$(STAGING_DIR)/usr/lib -lssl -lcrypto -lprotobuf-c -lmessageapi -lcryptoid -L $(STAGING_DIR)/usr/lib

compile_conn_modules: 
	$(foreach file, $(CONN_MODULES_FILES), $(CXX) -c $(file).cpp $(MISSING_HEADERS) -o $(file).o ;)

compile_qos_modules:
	$(foreach file, $(QOS_MODULES_FILES), $(CXX) -Wall -c $(file).cpp $(MISSING_HEADERS) -o $(file).o ;)

compile_userprofilling_modules:
	$(foreach file, $(USERPROFILLING_MODULES_FILES), $(CXX) -c $(file).cpp $(MISSING_HEADERS) -o $(file).o ;)

compile_utils_modules:
	$(foreach file, $(UTILS_MODULES_FILES), $(CXX) -c $(file).cpp $(MISSING_HEADERS) -o $(file).o ;)

clean:
	$(foreach file, $(CONN_MODULES_FILES), $(REMOVE) $(file).o ;)
	$(foreach file, $(QOS_MODULES_FILES)$, $(REMOVE) $(file).o ;)
	$(foreach file, $(USERPROFILLING_MODULES_FILES), $(REMOVE) $(file).o ;)
	$(foreach file, $(UTILS_MODULES_FILES), $(REMOVE) $(file).o ;)
	$(REMOVE) $(BINARY)
