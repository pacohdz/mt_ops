//
// The model implementation for the Randomized Rumor Spreading (RRS) layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-aug-2015
//
//

#ifndef KRRSLAYER_H_
#define KRRSLAYER_H_

#define TRUE                            1
#define FALSE                           0

#include <omnetpp.h>
#include <cstdlib>
#include <unordered_map> // Added for Broadcast version
#include <random> // Added for Broadcast version
#include <algorithm> // Added for Broadcast version
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iterator>
#include <ctime>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

using namespace std;


class KRRSLayer: public cSimpleModule
{
    public:
        static unordered_map<string,string> map1(){
            unordered_map<string,string> map1;
            return map1;
        }
        static unordered_map<string,int> map2(){
            unordered_map<string,int> map2;
            return map2;
        }
            static unordered_map<string,double> map3(){
            unordered_map<string,double> map3;
            return map3;
        }
        static unordered_map<string,string> globalCache;
        static unordered_map<string,int> commTable;
        static unordered_map<string,string> disseminationStartEnd;
        static int commTableEntries;
        static int x;
        static int y;
        static bool firstDataCreated;
        static int infectedNodes;
        static int totalInfectionTries;
        static int alpha;
        static int alphaTries;
        static bool go;
        static int stateJumps;
            
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:
        string ownMACAddress;
        int nextAppID;
        int maximumCacheSize;
        int logging;
        int nodeIndex;

        int currentCacheSize;
               
        /// Added for Unicast/Broadcast version ///
        int NUMNODES;
        int NUMDATA;
        vector<int> infectedList;
        int alreadyInfected;
        int newInfected;
        int tries;
        /// End of addition ///

        struct AppInfo {
            int appID;
            string appName;
            string prefixName;
        };

        struct CacheEntry {
            string dataName;
            int goodnessValue;
            int realPayloadSize;
            string dummyPayloadContent;
            int msgType;
            double validUntilTime;

            int realPacketSize;

            string finalDestinationNodeName;

            double createdTime;
            double updatedTime;
            double lastAccessedTime;

        };
                
        struct SimOutData{
            int node_id;
            int num_neigh;
            int alr_inf;
            int new_inf;
            int alpha;
            int alpha_tries;
            int jumps;
            int round;
            string event_label;
            string success;
        };
        
        list<SimOutData*> simOutDataList;
        string broadcastMACAddress;
        
        int saveFile(string stats);
        int saveTimeStamp(string ts);
        string fileTimeStamp();
        SimOutData simOutData(KNeighbourListMsg *neighListMsg, CacheEntry *cacheEntry);
        
        list<AppInfo*> registeredAppList;
        list<CacheEntry*> cacheList;
        cMessage *ageDataTimeoutEvent;

};

unordered_map<string,string> KRRSLayer::globalCache = KRRSLayer::map1();
unordered_map<string,int> KRRSLayer::commTable = KRRSLayer::map2();
int KRRSLayer::commTableEntries = 0;
int KRRSLayer::x = 0;  // Counter used to advance in the comm table
int KRRSLayer::y = 0; // Counter used when only infected nodes are selected.
bool KRRSLayer::firstDataCreated = false;
int KRRSLayer::infectedNodes = 0;
int KRRSLayer::totalInfectionTries = 0;
int KRRSLayer::alpha = 0;
int KRRSLayer::alphaTries = 0;
bool KRRSLayer::go = true;
int KRRSLayer::stateJumps = 0;
unordered_map<string,string> KRRSLayer::disseminationStartEnd = KRRSLayer::map1();

#define KRRSLAYER_SIMMODULEINFO       ">!<" << simTime() << ">!<" << getParentModule()->getFullName()

#endif /* KRRSLAYER_H_ */
