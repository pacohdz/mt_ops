//
// The model implementation for the Promote application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 17-nov-2016
//
//

#ifndef KPROMOTEAPP_H_
#define KPROMOTEAPP_H_

#include <omnetpp.h>
#include <cstdlib>
#include <random> // Added for unicast mod

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif


using namespace std;


#define KPROMOTEAPP_SIMMODULEINFO        ">!<" << simTime() << ">!<" << getParentModule()->getFullName()
#define TRUE                            1
#define FALSE                           0
#define KPROMOTEAPP_DISTR_NONE          1 // constant time intervals
#define KPROMOTEAPP_DISTR_EXPONENTIAL   2 // exponential time intervals
#define KPROMOTEAPP_DISTR_UNIFORM       3 // uniformly distributed time intervals


class KPromoteApp : public cSimpleModule
{   
    // Added for Unicast/Broadcast version
    public:
      	static bool once;
      	static int rdCnt;
      	static int rnd_int;
        static int generatedDataCount;
    // End of addition
    
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        char prefixName[128] = "/promote";
        int nodeIndex;
        //Added for Unicast/Broadcast version
        int operationMode; 
        int dataGeneratingNodeIndex; 
        int numberOfData; 
        int numberOfNodes; 
        // End of addition
        int notificationGenDistribution;
        double notificationGenStartMaxTime;
        double notificationGenInterval;
        double notificationValidDuration;
        int dataPayloadSizeInBytes;
        int dataPacketSizeInBytes;
        int usedRNG;
        string expectedNodeTypes;
        bool destinationOriented;
        int logging;
        int lastGeneratedNotificationID = 10000;

        cMessage *appRegistrationEvent;
        cMessage *dataTimeoutEvent;

        list<string*> nodeNameList;
        
       
};
//Added for Unicast/Broadcast version
bool KPromoteApp::once = true;
int KPromoteApp::rnd_int = false;
int KPromoteApp::rdCnt = 0;
int KPromoteApp::generatedDataCount = 0;
//End of addition

#endif /* KPROMOTEAPP_H_ */
