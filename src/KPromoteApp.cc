//
// The model implementation for the Promote application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 17-nov-2016
//
//

#include "KPromoteApp.h"

Define_Module(KPromoteApp);

void KPromoteApp::initialize(int stage)
{
    if (stage == 0) {
        
        // get parameters
        nodeIndex = par("nodeIndex");
        // Added for Unicast/Broadcast version
        operationMode = par("operationMode");
        dataGeneratingNodeIndex = par("dataGeneratingNodeIndex");
        numberOfData = par("numberOfData");
        numberOfNodes = par("numberOfNodes");
        // End of addition
        notificationGenDistribution = par("notificationGenDistribution");
        notificationGenStartMaxTime = par("notificationGenStartMaxTime");
        notificationGenInterval = par("notificationGenInterval");
        notificationValidDuration = par("notificationValidDuration");
        dataPayloadSizeInBytes = par("dataPayloadSizeInBytes");
        dataPacketSizeInBytes = par("dataPacketSizeInBytes");
        usedRNG = par("usedRNG");
        expectedNodeTypes = par("expectedNodeTypes").stringValue();
        destinationOriented = par("destinationOriented");
        logging = par("logging");
        
        /*
        Generate the random dataGeneratingNodeIndex only once, but submit it
        to every single node to avoid duplicate data generation or no data
        generation
        */
        if(once){

            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_int_distribution<int> uni(0, numberOfNodes - 1);
            auto random_integer = uni(rng);
            rnd_int = random_integer;
            //EV_INFO << "NUMBER OF NODES: " << numberOfNodes << " \n";
            //EV_INFO << "THE RANDOM NUMBER IS: " << rnd_int << " \n";
            once = false;
        }
        if (dataGeneratingNodeIndex == 0){
            //dataGeneratingNodeIndex = rnd_int; // For complete random scenarios
            dataGeneratingNodeIndex = 0; // Node 0 generates the initial data
            //EV_INFO << "Node: " << rnd_int << " generates Information \n";
        }
  
    } else if (stage == 1) {

        // make list of all node names in network
        for (int id = 0; id <= getSimulation()->getLastComponentId(); id++) {
            cModule *unknownModule = getSimulation()->getModule(id);
            if (unknownModule == NULL) {
                continue;
            }

            // has to be a node type module given in expectedNodeTypes parameter
            if(strstr(expectedNodeTypes.c_str(), unknownModule->getModuleType()->getName()) == NULL) {
                continue;
            }

            // don't save own node name
            if(getParentModule() == unknownModule) {
                continue;
            }

            string *nodeName = new string(unknownModule->getFullName());

            // list should only contain nodes that have the KPromoteApp deployed
            int promoteAppDeployed = unknownModule->par("promoteAppDeployed");
            if(promoteAppDeployed != 1) {
                continue;
            }

            // // temp hack to prevent data being generated to fixed nodes
            // if(strstr(nodeName->c_str(), "fixedNode") != NULL) {
            //     continue;
            // }

            nodeNameList.push_back(nodeName);
        }

        // cout << "mobile node size: " <<  nodeNameList.size() << "\n";

        if(nodeNameList.size() == 0) {
            EV_FATAL << KPROMOTEAPP_SIMMODULEINFO << "No other nodes in the network \n";
        }

    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(92);
        scheduleAt(simTime(), appRegistrationEvent);
        
        if (operationMode == 1 || (operationMode == 2 && nodeIndex == dataGeneratingNodeIndex)) {
            // create and setup notification generation trigger
            dataTimeoutEvent = new cMessage("Data Timeout Event");
            dataTimeoutEvent->setKind(93);
            // double firstGenTime = simTime().dbl() + uniform(1.0, notificationGenStartMaxTime, usedRNG);
            //double firstGenTime = simTime().dbl() + uniform(0.1, 1, usedRNG); // Broadcast modification
            //scheduleAt(firstGenTime, dataTimeoutEvent);
            scheduleAt(simTime().dbl() + 0.010, dataTimeoutEvent); // Broadcast modification
        }
        
        

        if (operationMode == 2 && nodeIndex == dataGeneratingNodeIndex) {
            if (logging) {/*EV_INFO << KPROMOTEAPP_SIMMODULEINFO << ">!<SUTG "<< -1 << " DI" << "\n";*/}
        }
        
        generatedDataCount = 0;
        
    } else {
        EV_FATAL << KPROMOTEAPP_SIMMODULEINFO << "Something is radically wrong\n";
    }
}

int KPromoteApp::numInitStages() const
{
    return 3;
}

void KPromoteApp::handleMessage(cMessage *msg)
{
    bool first;
    // check message
    if (msg->isSelfMessage() && msg->getKind() == 92) {
        // send app registration message the forwarding  layer

        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg("Promote App Registration");
        regAppMsg->setAppName("Promote");
        regAppMsg->setPrefixName("/promote");

        send(regAppMsg, "lowerLayerOut");

        if (logging) {/*EV_INFO << KPROMOTEAPP_SIMMODULEINFO << ">!<GAR" << "\n";*/}

    } else if (msg->isSelfMessage() && msg->getKind() == 93) {
        
       // Check whether generatedDataCount is already reached
        if(generatedDataCount < numberOfData){ 
            
            bool first;
            if(generatedDataCount == 0){
                first = true;
            }
            
            // timeout for data (notification) send event occured
            // so, generate a data message
            char dummyPayloadContent[64], dataName[64], msgName[64];

            sprintf(dataName, "/promote/item-%05d-%05d", nodeIndex, lastGeneratedNotificationID);
            sprintf(msgName, "D item-%05d-%05d", nodeIndex, lastGeneratedNotificationID);
            sprintf(dummyPayloadContent, "Details of item-%05d-%05d", nodeIndex, lastGeneratedNotificationID);

            KDataMsg *dataMsg = new KDataMsg(msgName);

            dataMsg->setSourceAddress("");
            dataMsg->setDestinationAddress("");
            dataMsg->setDataName(dataName);
            dataMsg->setGoodnessValue(100);
            dataMsg->setRealPayloadSize(dataPacketSizeInBytes);
            dataMsg->setDummyPayloadContent(dummyPayloadContent);
            dataMsg->setByteLength(dataPacketSizeInBytes);
            dataMsg->setRealPacketSize(dataPacketSizeInBytes);
            dataMsg->setMsgType(0);
            dataMsg->setValidUntilTime(simTime().dbl() + notificationValidDuration);
            dataMsg->setOriginatorNodeName(getParentModule()->getFullName());
            if (destinationOriented) {
                dataMsg->setDestinationOriented(true);
                int randomNodeIndex = intuniform(0, (nodeNameList.size() - 1), usedRNG);
                list<string*>::iterator it = nodeNameList.begin();
                advance(it, randomNodeIndex);
                string *nodeName = *it;
                dataMsg->setFinalDestinationNodeName(nodeName->c_str());
                
            } else {
                dataMsg->setDestinationOriented(false);
                
            }

            send(dataMsg, "lowerLayerOut");           

            if (logging) {/*EV_INFO << KPROMOTEAPP_SIMMODULEINFO << ">!<GD>!<" << dataMsg->getDataName() << ">!<AS>!<"
                << getParentModule()->getFullName() << " \n";*/}
            
            generatedDataCount++;
            
            EV_INFO << "operationMode = " << operationMode << "\n";
            EV_INFO << "nodeIndex = " << nodeIndex << "\n";
            EV_INFO << "dataGeneratingNodeIndex = " <<  dataGeneratingNodeIndex << "\n";
            EV_INFO << "generatedDataCount = " << generatedDataCount << "\n";
            EV_INFO << "numberOfData = " << numberOfData << "\n";
            if (operationMode == 1 || (operationMode == 2 && nodeIndex == dataGeneratingNodeIndex && generatedDataCount < numberOfData)) {
                // setup next data generation trigger
                lastGeneratedNotificationID++;
                double nextNotificationGenTime = 0.0;
                if (notificationGenDistribution == KPROMOTEAPP_DISTR_EXPONENTIAL) {
                    nextNotificationGenTime = simTime().dbl() + exponential(notificationGenInterval, usedRNG);
                } else if (notificationGenDistribution == KPROMOTEAPP_DISTR_UNIFORM) {
                    nextNotificationGenTime = simTime().dbl() + uniform(0.0, (notificationGenInterval * 2), usedRNG);
                } else {
                    nextNotificationGenTime = simTime().dbl() + notificationGenInterval;
                }

                // cout << "curr sim time: " << simTime().dbl() << " - next gen time: " << nextNotificationGenTime
                //               << " - time diff: " << (nextNotificationGenTime - simTime().dbl()) << "\n";

                scheduleAt(nextNotificationGenTime, msg);
            }
        }


    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        if(strstr(getParentModule()->getFullName(), dataMsg->getFinalDestinationNodeName()) != NULL) {
            
            if (logging) {/*EV_INFO << KPROMOTEAPP_SIMMODULEINFO << ">!<RD>!<" << dataMsg->getDataName() << ">!<AD>!<"
                << dataMsg->getFinalDestinationNodeName() << " \n";*/}
        }

        delete msg;

    } else {

        EV_INFO << KPROMOTEAPP_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }
}

void KPromoteApp::finish()
{
    delete appRegistrationEvent;
    if (operationMode == 1 || (operationMode == 2 && nodeIndex == dataGeneratingNodeIndex)) {       
        // remove data generation event
        cancelEvent(dataTimeoutEvent);
        delete dataTimeoutEvent;
    }
}

