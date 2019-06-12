//
// The model implementation for the Randomized Rumor Spreading (RRS) layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 25-aug-2015
//
//

#include "KRRSLayer.h"
//#include "inet/mobility/base/MobilityBase.h"
//#include "inet/mobility/base/MovingMobilityBase.h"

Define_Module(KRRSLayer);

void KRRSLayer::initialize(int stage)
{

    if (stage == 0) {
        // get parameters
        ownMACAddress = par("ownMACAddress").stringValue();
        nextAppID = 1;
        maximumCacheSize = par("maximumCacheSize");
        currentCacheSize = 0;
        
        nodeIndex = getParentModule()->getIndex();
        NUMNODES = par("numberOfNodes");
        NUMDATA = par("numberOfData");
        
        logging = par("logging");

        // set other parameters
        broadcastMACAddress = "FF:FF:FF:FF:FF:FF";
        
        // Only the node with index 0 creates the predefined communication table
        if (getParentModule()->getIndex() == 0){
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_int_distribution<int> uni(0, NUMNODES - 1);

            cout <<  "NUM NODES: " << NUMNODES << "\n";
            commTableEntries = NUMNODES * 20; // This is an arbitrary initial number of senders in the commTable
            string strSender;
            for (int i = 0; i <= commTableEntries; ++i){
                auto random_sender = uni(rng);
                strSender = "sender" + to_string(i);
                if (i == 0){
                    // Log start of dissemination
                    disseminationStartEnd.insert({"start", simTime().str()});
                    commTable.insert({strSender, 0}); // Start with the first infected node
                } else {
                    commTable.insert({strSender, random_sender}); // Random node selected
                    //commTable.insert({strSender, 0});  // Center node is always selected to spread the infection
                }
                
            }
                      
            // EV_INFO << "----------- Communication Table -------------\n";
            // for (pair<string, int> element : commTable){        
            //     EV_INFO << element.first << " = " << element.second << "\n";
            // }
            // EV_INFO << "----------- End Communication Table -------------\n";
            
        //     EV_INFO << "----------- Arrivals Table -------------\n";
        //     for (pair<string, double> element : timeTable){        
        //         EV_INFO << element.first << " = " << element.second << "\n";
        //     }
        //     EV_INFO << "----------- End Arrivals Table -------------\n";
        }
        /// End of addition ///

    } else if (stage == 1) {


    } else if (stage == 2) {

        // setup the trigger to age data
        ageDataTimeoutEvent = new cMessage("Age Data Timeout Event");
        ageDataTimeoutEvent->setKind(108);
        scheduleAt(simTime() + 1.0, ageDataTimeoutEvent);

    } else {
        EV_FATAL << KRRSLAYER_SIMMODULEINFO << "Something is radically wrong in initialisation \n";
    }
}

int KRRSLayer::numInitStages() const
{
    return 3;
}

void KRRSLayer::handleMessage(cMessage *msg)
{
    cGate *gate;
    char gateName[64];
    KRegisterAppMsg *regAppMsg;
    KDataMsg *omnetDataMsg;
    KFeedbackMsg *omnetFeedbackMsg;
    KNeighbourListMsg *neighListMsg;

    // self messages
    if (msg->isSelfMessage()) {

        // age data trigger fired
        if (msg->getKind() == 108) {

            // remove expired data items
            int expiredFound = TRUE;
            while (expiredFound) {
                expiredFound = FALSE;

                CacheEntry *cacheEntry;
                list<CacheEntry*>::iterator iteratorCache;
                iteratorCache = cacheList.begin();
                while (iteratorCache != cacheList.end()) {
                    cacheEntry = *iteratorCache;
                    if (cacheEntry->validUntilTime < simTime().dbl()) {
                        expiredFound = TRUE;
                        break;
                    }
                    iteratorCache++;
                }
                if (expiredFound) {
                    currentCacheSize -= cacheEntry->realPacketSize;
                    cacheList.remove(cacheEntry);
                    delete cacheEntry;
                }
            }

            // setup next age data trigger
            scheduleAt(simTime() + 1.0, msg);

        } else {
            EV_INFO << KRRSLAYER_SIMMODULEINFO << "Received unexpected self message" << "\n";
            delete msg;
        }

    // messages from other layers
    } else {

       // get message arrival gate name
        gate = msg->getArrivalGate();
        strcpy(gateName, gate->getName());

        // app registration message arrived from the upper layer (app layer)
        if (strstr(gateName, "upperLayerIn") != NULL && (regAppMsg = dynamic_cast<KRegisterAppMsg*>(msg)) != NULL) {

            AppInfo *appInfo = NULL;
            int found = FALSE;
            list<AppInfo*>::iterator iteratorRegisteredApps = registeredAppList.begin();
            while (iteratorRegisteredApps != registeredAppList.end()) {
                appInfo = *iteratorRegisteredApps;
                if (appInfo->appName == regAppMsg->getAppName()) {
                    found = TRUE;
                    break;
                }
                iteratorRegisteredApps++;
            }

            if (!found) {
                appInfo = new AppInfo;
                appInfo->appID = nextAppID++;
                appInfo->appName = regAppMsg->getAppName();
                registeredAppList.push_back(appInfo);

            }
            appInfo->prefixName = regAppMsg->getPrefixName();
            delete msg;

        // data message arrived from the upper layer (app layer)
        } else if (strstr(gateName, "upperLayerIn") != NULL && (omnetDataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {

            if (logging) {/*EV_INFO << KRRSLAYER_SIMMODULEINFO << ">!<" << ownMACAddress << ">!<UI>!<DM>!<" << omnetDataMsg->getSourceAddress() << ">!<"
                << omnetDataMsg->getDestinationAddress() << ">!<" << omnetDataMsg->getDataName() << ">!<" << omnetDataMsg->getGoodnessValue() << ">!<"
                << omnetDataMsg->getByteLength() << "\n"*/;}

            CacheEntry *cacheEntry;
            list<CacheEntry*>::iterator iteratorCache;
            int found = FALSE;
            iteratorCache = cacheList.begin();
            while (iteratorCache != cacheList.end()) {
                cacheEntry = *iteratorCache;
                if (cacheEntry->dataName == omnetDataMsg->getDataName()) {
                    found = TRUE;
                    break;
                }

                iteratorCache++;
            }

            if (!found) {

                // apply caching policy if limited cache and cache is full
                if (maximumCacheSize != 0
                        && (currentCacheSize + omnetDataMsg->getRealPacketSize()) > maximumCacheSize
                        && cacheList.size() > 0) {
                    iteratorCache = cacheList.begin();
                    advance(iteratorCache, 0);
                    CacheEntry *removingCacheEntry = *iteratorCache;
                    iteratorCache = cacheList.begin();
                    while (iteratorCache != cacheList.end()) {
                        cacheEntry = *iteratorCache;
                        if (cacheEntry->validUntilTime < removingCacheEntry->validUntilTime) {
                            removingCacheEntry = cacheEntry;
                        }
                        iteratorCache++;
                    }
                    currentCacheSize -= removingCacheEntry->realPacketSize;
                    cacheList.remove(removingCacheEntry);
                    delete removingCacheEntry;
                }

                cacheEntry = new CacheEntry;
                cacheEntry->dataName = omnetDataMsg->getDataName();
                cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
                cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
                cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
                cacheEntry->msgType = omnetDataMsg->getMsgType();
                cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();

                cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();

                cacheEntry->finalDestinationNodeName = omnetDataMsg->getFinalDestinationNodeName();

                cacheEntry->createdTime = simTime().dbl();
                cacheEntry->updatedTime = simTime().dbl();

                cacheList.push_back(cacheEntry);

                currentCacheSize += cacheEntry->realPacketSize;
                
                // Save message ID in global cache so all nodes can check the messages other nodes have.
                globalCache.insert({ cacheEntry->dataName.c_str(), to_string(nodeIndex) });
                infectedNodes = 1;
                alpha = 1;
                firstDataCreated = true;
                EV_INFO << "<msg_created>" << simTime() << "</msg_created>";
            }

            cacheEntry->lastAccessedTime = simTime().dbl();
            
            delete msg;

        // feedback message arrived from the upper layer (app layer)
        } else if (strstr(gateName, "upperLayerIn") != NULL && (omnetFeedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {

            // with RRS, no feedback is considered

            delete msg;

        // neighbour list message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && (neighListMsg = dynamic_cast<KNeighbourListMsg*>(msg)) != NULL) {

            /*if (logging) {EV_INFO << KRRSLAYER_SIMMODULEINFO << ">!<NM>!<NC>!<" <<
                                neighListMsg->getNeighbourNameListArraySize() << ">!<CS>!<"
                                    << cacheList.size() << "\n";}*/

           

            if (infectedNodes < NUMNODES) {
                if ( commTable.find("sender" + to_string(x))->second == getParentModule()->getIndex() && (firstDataCreated == true) && go == true ){
                    if (neighListMsg->getNeighbourNameListArraySize() > 0 && cacheList.size() > 0 ) {

                        // if there are nodes in the neighbourhood and entries in the cache
                        // send a randomly selected cached data out

                        int cacheIndex = 0;
                        /*if (cacheList.size() > 1) {
                            cacheIndex = intuniform(0, (cacheList.size() - 1));
                        }*/
                                            
                        list<CacheEntry*>::iterator iteratorCache = cacheList.begin();
                        advance(iteratorCache, cacheIndex);
                        CacheEntry *cacheEntry = *iteratorCache;

                        KDataMsg *dataMsg = new KDataMsg();

                        dataMsg->setSourceAddress(ownMACAddress.c_str());
                        dataMsg->setDestinationAddress(broadcastMACAddress.c_str());
                        dataMsg->setDataName(cacheEntry->dataName.c_str());
                        dataMsg->setGoodnessValue(cacheEntry->goodnessValue);
                        dataMsg->setRealPayloadSize(cacheEntry->realPayloadSize);
                        dataMsg->setDummyPayloadContent(cacheEntry->dummyPayloadContent.c_str());
                        dataMsg->setRealPacketSize(cacheEntry->realPacketSize);
                        dataMsg->setByteLength(cacheEntry->realPacketSize);
                        dataMsg->setByteLength(cacheEntry->realPacketSize);
                        dataMsg->setMsgType(cacheEntry->msgType);
                        dataMsg->setValidUntilTime(cacheEntry->validUntilTime);
                        dataMsg->setFinalDestinationNodeName(cacheEntry->finalDestinationNodeName.c_str());
                        
                        
                        y += 1;
                        EV_INFO << "--------------------------------------------------------------------\n";
                        /* EV_INFO << "NODE " << getParentModule()->getIndex() << " HAS " << numNeigh << " NEIGHBORS\n";*/
                        // EV_INFO << "NODE " << getParentModule()->getIndex() << " HAS DATA AND IS ITS TURN. BROADCASTING TO NEIGHBORS. ROUND "<< (x+1) <<" \n";
                        EV_INFO << "NODE " << getParentModule()->getIndex() << " HAS DATA AND IS ITS TURN. BROADCASTING TO NEIGHBORS. ROUND "<< y <<" \n";
                                                
                        // Check data for XML-style output
                        SimOutData xmlOut = simOutData(neighListMsg, cacheEntry);
                        
                        // For debugging purposes
                        EV_INFO << xmlOut.event_label << " NODE " << getParentModule()->getIndex() << " INFECTED " << xmlOut.new_inf << " NEIGHBORS. " << xmlOut.alr_inf << " NEIGHBORS ALREADY INFECTED\n";
                        EV_INFO <<  "NETWORK STATE (ALPHA) = " << xmlOut.alpha << "\n";
                        
                        // Format output as XML tags for log parsing
                        EV_INFO << "<message>\n"
                                    << "<node_id>" << xmlOut.node_id << "</node_id>\n"
                                    << "<num_neigh>" << xmlOut.num_neigh << "</num_neigh>\n"
                                    << "<success>" << xmlOut.success << "</success>\n"
                                    << "<new_inf>" << xmlOut.new_inf << "</new_inf>\n"
                                    << "<alr_inf>" << xmlOut.alr_inf << "</alr_inf>\n"
                                    << "<alpha>" << xmlOut.alpha << "</alpha>\n"
                                    << "<tries>" << xmlOut.alpha_tries << "</tries>\n"
                                    << "<jumps>" << xmlOut.jumps << "</jumps>\n"
                                    // << "<round>" << xmlOut.round << "</round>\n"
                                    << "<round>" << y << "</round>\n"
                                    << "<timestamp>" << simTime() << "</timestamp>\n"
                                    << "</message>\n";
                                
                        EV_INFO << "--------------------------------------------------------------------\n";
                        
                        // Save data as CSV file
                        string strStats = to_string(xmlOut.node_id) + "," + to_string(xmlOut.num_neigh) + "," + to_string(xmlOut.new_inf) + "," + to_string(xmlOut.alr_inf) + "," + to_string(xmlOut.alpha ) + "," + to_string(xmlOut.alpha_tries) + "," + to_string(xmlOut.jumps) + "," + to_string(xmlOut.round) + "," + xmlOut.success;
                        //saveFile(strStats);
                        saveTimeStamp(simTime().str());
                        
                        // ONLY FOR STATIC SCENARIOS: Stop simulation after one try
                        // if (x == 0){
                        //     delete msg;
                        //     endSimulation();
                        // }
                        // -------------------------- //

                        if (x < commTableEntries) {
                            x++;
                            
                        }
                        
                        send(dataMsg, "lowerLayerOut");
                        go =  false; // Broadcast done
                        
                        
                        /* if (logging) {EV_INFO << KRRSLAYER_SIMMODULEINFO << ">!<" << ownMACAddress << ">!<LO>!<DM>!<" << dataMsg->getSourceAddress() << ">!<"
                            << dataMsg->getDestinationAddress() << ">!<" << dataMsg->getDataName() << ">!<" << dataMsg->getGoodnessValue() << ">!<"
                            << dataMsg->getByteLength() << "\n";} */
                    
                    } else {//if ( commTable.find("sender" + to_string(x))->second == getParentModule()->getIndex() && (firstDataCreated == true)  && go == true) { 
                    
                        // The current sender does not have data to send, so continue with the next one
                        
                        // EV_INFO << "--------------------------------------------------------------------\n";
                        // EV_INFO << "[FAILED EVENT] NODE " << getParentModule()->getIndex() << " DOESN'T HAVE DATA AND IS ITS TURN. ROUND "<< (x+1) <<" \n"; 
                        
                        // Print sim values with XML format
                        //if (cacheList.size() == 0) {
                            // The current node does not have data so there is not cache to look up
                            //CacheEntry *cacheEntry = NULL;
                            
                            // // Check data for XML-style output
                            // SimOutData xmlOut = simOutData(neighListMsg, cacheEntry);
                            
                            // // Format output as XML tags for log parsing
                            // EV_INFO << "<message>\n"
                            //         << "<node_id>" << xmlOut.node_id << "</node_id>\n"
                            //         << "<num_neigh>" << xmlOut.num_neigh << "</num_neigh>\n"
                            //         << "<success>" << xmlOut.success << "</success>\n"
                            //         << "<new_inf>" << xmlOut.new_inf << "</new_inf>\n"
                            //         << "<alr_inf>" << xmlOut.alr_inf << "</alr_inf>\n"
                            //         << "<alpha>" << xmlOut.alpha << "</alpha>\n"
                            //         << "<tries>" << xmlOut.alpha_tries << "</tries>\n"
                            //         << "<jumps>" << xmlOut.jumps << "</jumps>\n"
                            //         << "<rounds>" << xmlOut.round << "</rounds>\n"
                            //         << "<timestamp>" << simTime()<< "</timestamp>\n"
                            //         << "</message>\n";
                                    
                            // EV_INFO << "--------------------------------------------------------------------\n";
                            
                            // // Save data as CSV file
                            // string strStats = to_string(xmlOut.node_id) + "," + to_string(xmlOut.num_neigh) + "," + to_string(xmlOut.new_inf) + "," + to_string(xmlOut.alr_inf) + "," + to_string(xmlOut.alpha ) + "," + to_string(xmlOut.alpha_tries) + "," + to_string(xmlOut.jumps) + "," + to_string(xmlOut.round) + "," + xmlOut.success;
                            // //saveFile(strStats);
                            // saveTimeStamp(simTime().str());
                            
                        //}
                        
                        if (x < commTableEntries) {
                            x++;
                        }

                        go = true; // Next commTable entry
                    }
                }
                
                delete msg;
            } else {
                
                // totalInfectionTries = x; 
                totalInfectionTries = y; // For only infected node selection
                
                EV_INFO << "<total_tries>" <<  totalInfectionTries << "</total_tries>\n";               
                EV_INFO << "ALL NODES INFECTED. IT TOOK " << totalInfectionTries << " TRIES TO INFECT ALL NODES\n";

                delete msg;
                endSimulation();
            }
            
        // data message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && (omnetDataMsg = dynamic_cast<KDataMsg*>(msg)) != NULL) {

            int found;

            if (logging) {/*EV_INFO << KRRSLAYER_SIMMODULEINFO << ">!<" << ownMACAddress << ">!<LI>!<DM>!<" << omnetDataMsg->getSourceAddress() << ">!<"
                << omnetDataMsg->getDestinationAddress() << ">!<" << omnetDataMsg->getDataName() << ">!<" << omnetDataMsg->getGoodnessValue() << ">!<"
                    << omnetDataMsg->getByteLength() << "\n"*/;}

            // if destination oriented data sent around, then cache message only if not destined to self
            // or if no destination in data, cache all messages
            bool cacheData = TRUE;
            if (omnetDataMsg->getDestinationOriented() && strstr(getParentModule()->getFullName(), omnetDataMsg->getFinalDestinationNodeName()) != NULL) {
                cacheData = FALSE;
            }

            if(cacheData) {

                // insert/update cache
                CacheEntry *cacheEntry;
                list<CacheEntry*>::iterator iteratorCache;
                found = FALSE;
                iteratorCache = cacheList.begin();
                while (iteratorCache != cacheList.end()) {
                    cacheEntry = *iteratorCache;
                    if (cacheEntry->dataName == omnetDataMsg->getDataName()) {
                        found = TRUE;
                        break;
                    }

                    iteratorCache++;
                }

                if (!found) {

                    // apply caching policy if limited cache and cache is full
                    if (maximumCacheSize != 0
                        && (currentCacheSize + omnetDataMsg->getRealPacketSize()) > maximumCacheSize
                    && cacheList.size() > 0) {
                        iteratorCache = cacheList.begin();
                        advance(iteratorCache, 0);
                        CacheEntry *removingCacheEntry = *iteratorCache;
                        iteratorCache = cacheList.begin();
                        while (iteratorCache != cacheList.end()) {
                            cacheEntry = *iteratorCache;
                            if (cacheEntry->validUntilTime < removingCacheEntry->validUntilTime) {
                                removingCacheEntry = cacheEntry;
                            }
                            iteratorCache++;
                        }
                        currentCacheSize -= removingCacheEntry->realPacketSize;
                        cacheList.remove(removingCacheEntry);
                        delete removingCacheEntry;

                    }

                    cacheEntry = new CacheEntry;
                    cacheEntry->dataName = omnetDataMsg->getDataName();
                    cacheEntry->goodnessValue = omnetDataMsg->getGoodnessValue();
                    cacheEntry->realPayloadSize = omnetDataMsg->getRealPayloadSize();
                    cacheEntry->dummyPayloadContent = omnetDataMsg->getDummyPayloadContent();
                    cacheEntry->msgType = omnetDataMsg->getMsgType();
                    cacheEntry->validUntilTime = omnetDataMsg->getValidUntilTime();

                    cacheEntry->realPacketSize = omnetDataMsg->getRealPacketSize();

                    cacheEntry->finalDestinationNodeName = omnetDataMsg->getFinalDestinationNodeName();

                    cacheEntry->createdTime = simTime().dbl();
                    cacheEntry->updatedTime = simTime().dbl();

                    cacheList.push_back(cacheEntry);

                    currentCacheSize += cacheEntry->realPacketSize;
                    
                    // Update the global cache
                    string messageID = omnetDataMsg->getDataName();
                    
                    //if (globalCache.find(messageID) != globalCache.end()){
                        string temp = globalCache.find(cacheEntry->dataName)->second;
                        temp = temp + "," + to_string(nodeIndex);
                        globalCache[messageID] = temp;
                        EV_INFO << "GLOBAL CACHE: " << globalCache.find(messageID)->second << "\n";
                        
                        // Count the number of infected nodes
                        infectedNodes = count(temp.begin(), temp.end(), ',');
                        if (infectedNodes == NUMNODES - 1){
                            infectedNodes++;
                            EV_INFO << "<infected_nodes>" << infectedNodes << "</infected_nodes>\n";
                            EV_INFO << "TOTAL INFECTED NODES = " << infectedNodes << "\n";
                        }
                        
                   // }
                }
                
                // After the cache of a node is updated the next broadcasting node can continue. This in order
                // to avoid the following node to see a not updated cache and report wrong infected nodes.
                go = true; 
                
                cacheEntry->lastAccessedTime = simTime().dbl();
            }

            // if registered app exist, send data msg to app
            AppInfo *appInfo = NULL;
            found = FALSE;
            list<AppInfo*>::iterator iteratorRegisteredApps = registeredAppList.begin();
            while (iteratorRegisteredApps != registeredAppList.end()) {
                appInfo = *iteratorRegisteredApps;
                if (strstr(omnetDataMsg->getDataName(), appInfo->prefixName.c_str()) != NULL) {
                    found = TRUE;
                    break;
                }
                iteratorRegisteredApps++;
            }
            if (found) {
                send(msg, "upperLayerOut");

                if (logging) {/*EV_INFO << KRRSLAYER_SIMMODULEINFO << ">!<" << ownMACAddress << ">!<UO>!<DM>!<" << omnetDataMsg->getSourceAddress() << ">!<"
                    << omnetDataMsg->getDestinationAddress() << ">!<" << omnetDataMsg->getDataName() << ">!<" << omnetDataMsg->getGoodnessValue() << ">!<"
                    << omnetDataMsg->getByteLength() << "\n"*/;}

            } else {
                delete msg;
            }

        // feedback message arrived from the lower layer (link layer)
        } else if (strstr(gateName, "lowerLayerIn") != NULL && (omnetFeedbackMsg = dynamic_cast<KFeedbackMsg*>(msg)) != NULL) {

            // with RRS, no feedback is considered
            delete msg;

        // received some unexpected packet
        } else {

            EV_INFO << KRRSLAYER_SIMMODULEINFO << "Received unexpected packet" << "\n";
            delete msg;
        }
    }
}

int KRRSLayer::saveFile(string stats) 
{
    // Save statistics to CSV file
    string resultdir = "./simulations/out/test/";
    
    ofstream myfile;
    myfile.open (resultdir + "sim_data.csv", ofstream::out | ofstream::app);
    myfile << stats << "\n";
    myfile.close();
    return 0;
    
}

int KRRSLayer::saveTimeStamp(string ts){
    // Save Timestamp to CSV file
    string resultdir = "./simulations/out/test/";
    
    ofstream myfile;
    myfile.open (resultdir + "sim_ts.csv", ofstream::out | ofstream::app);
    myfile << ts << "\n";
    myfile.close();
    return 0;
}

string KRRSLayer::fileTimeStamp(){
    //Get timestamp for files
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;

    char buffer [80];
    strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S", localtime(&curTime.tv_sec));
    string fileTS = (string) buffer + "_" + to_string(milli);

    return fileTS;
}


KRRSLayer::SimOutData KRRSLayer::simOutData(KNeighbourListMsg *neighListMsg, CacheEntry *cacheEntry)
{
    // Get current number of neighbors
    int numNeigh = neighListMsg->getNeighbourNameListArraySize();
    
    if(cacheEntry != NULL) {
        // Check current infected neighbors in the global cache
        string strNodeIndices = globalCache.find(cacheEntry->dataName)->second;
    
        // Since the string looks like 1,2,3,4,5 a comma is added at the end to extract all nodes in the next function
        strNodeIndices =  strNodeIndices + ",";

        size_t pos = 0;
        string strNodeIndex;
        string delimiter = ",";

        // Extract node indices from the global cache string
        int j = 0;
        while ((pos = strNodeIndices.find(delimiter)) != std::string::npos) {
            strNodeIndex = strNodeIndices.substr(0, pos);
            infectedList.push_back(stoi(strNodeIndex)); // Save the infected nodes to a list                       
            strNodeIndices.erase(0, pos + delimiter.length());
            j++;
        }
    
    }
    
    // Extract node indices from the neighbor list.
    // For a single digit index the mac address looks like 02:00:00:00:00:05 <-- Node index is 5
    // For a double digit index the mac address looks like 02:00:00:00:03:05 <-- Node index is 35
    // For a triple digit index the mac address looks like 02:00:00:01:00:01 <-- Node index is 101

    string unit;
    string ten;
    string hundred;
    string strAddress;
    int index;
    alreadyInfected = 0;
    newInfected = 0;

    // Check all current neighbors to know if they are infected or not
    for (int i=0; i<numNeigh; i++){
        strAddress = neighListMsg->getNeighbourNameList(i);
        unit = strAddress.substr(15,2);
        ten = strAddress.substr(12,2);
        hundred = strAddress.substr(9,2);
        index = (stoi(hundred) * 100) + (stoi(ten) * 10) + stoi(unit);
        
        // Check if the current neighbor index is in the infected nodes list (global cache)
        if ( find(infectedList.begin(), infectedList.end(), index) != infectedList.end() )
        {
            // The neighbor node is already infected
            alreadyInfected++;
        } else {
            if(cacheEntry != NULL){
                // The neighbor node can be infected
                newInfected++;
            } else {
                // There is no message therefore an infection is not possible
                newInfected = 0;
            }
        }
        //EV_INFO << "NE" << i << ": " << neighListMsg->getNeighbourNameList(i) << " AND NODE INDEX: "<< index << "\n";                        
    } 

    alphaTries++;
    string eventLabel;
    string success;
    int statAlphaTries = 0; // For statistics displaying purposes only
    if (newInfected > 0){
        stateJumps++;
        statAlphaTries = alphaTries;
        alphaTries = 0; // If at least one neighbor was successfully infected the alpha changes and the counter is restarted
        alpha =  alpha + newInfected;
        eventLabel = "[SUCCESSFUL EVENT]";
        success = "1";
    }else{
        statAlphaTries = alphaTries;
        eventLabel = "[FAILED EVENT]";
        success = "0";
    }
        
  
    // Check if the node has a data structure
    SimOutData *outData;
    list<SimOutData*>::iterator dataIterator;
    int found = FALSE;
    dataIterator = simOutDataList.begin();
    while (dataIterator != simOutDataList.end()) {
        outData = *dataIterator;
        if (outData->node_id == getParentModule()->getIndex()) {
            found = TRUE;
            break;
        }

        dataIterator++;
    }
    
    // If the node has no output data structure, create a new one and save current values
    if (!found) {
        outData = new SimOutData;
        outData->node_id = getParentModule()->getIndex();
        outData->num_neigh = numNeigh;
        outData->alr_inf = alreadyInfected;
        outData->new_inf = newInfected;
        outData->alpha = alpha;
        outData->alpha_tries = statAlphaTries;
        outData->jumps = stateJumps;
        outData->round = x + 1;
        outData->event_label = eventLabel;
        outData->success = success;

    } else {
        // Update structure
        outData->num_neigh = numNeigh;
        outData->alr_inf = alreadyInfected;
        outData->new_inf = newInfected;
        outData->alpha = alpha;
        outData->alpha_tries = statAlphaTries;
        outData->jumps = stateJumps;
        outData->round = x + 1;
        outData->event_label = eventLabel;
        outData->success = success;
    }
    
    return *outData;
}

void KRRSLayer::finish()
{
    // Add "-" character at EOF to separate each run results from the others.
    string ts = fileTimeStamp();
    string resultdir = "./simulations/out/test/";
    if (getParentModule()->getIndex() == 0){
        ofstream myfile;
        myfile.open (resultdir + "sim_ts.csv", ofstream::out | ofstream::app);
        myfile << "-\n";
        myfile.close();
    }
    
    // Save to file start/end dissemination and inter-arrival times
    if (getParentModule()->getIndex() == 0){
        //double end = timeTable.find("sender" + to_string(x))->second;

        disseminationStartEnd.insert({"end", simTime().str()});
        
        ofstream file;
        file.open (resultdir + "dissemination.csv", ofstream::out | ofstream::app);
        file << disseminationStartEnd.find("start")->second << "," << disseminationStartEnd.find("end")->second << "\n";
        file.close();
    }  
    
    // remove age data trigger
    cancelEvent(ageDataTimeoutEvent);
    delete ageDataTimeoutEvent;
}
