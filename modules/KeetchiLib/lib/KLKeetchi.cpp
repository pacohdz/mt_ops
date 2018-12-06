/******************************************************************************
 * Keetchi - A library for opportunistic communications.
 *
 * Copyright (C) 2016, Communication Networks, University of Bremen, Germany
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 * This file is part of the Keetchi library
 *
 ******************************************************************************/

/**
*
* @author : Asanga Udugama (adu@comnets.uni-bremen.de)
* @date   : 18-June-2015
*
*/
#include "KLKeetchi.h"

KLKeetchi::KLKeetchi(int cachePolicy, int cacheSize, string ownAddr, double changeSigThreshold,
                     double coolOffDur, double learningConst, int simKeetchi, double backoffTimerIncFactor)
{
    maxCacheSize = cacheSize;
    cacheReplacementPolicy = cachePolicy;
    ownAddress = ownAddr;
    neighbourhoodChangeSignificanceThreshold = changeSigThreshold; // between 0.0 and 1.0
    coolOffDuration = coolOffDur;
    learningConstant = learningConst;
    simulatedKeetchi = simKeetchi;
    backoffTimerIncrementFactor = backoffTimerIncFactor;

    dataMgr = new KLDataMgr(cachePolicy, cacheSize, coolOffDuration, learningConstant, simulatedKeetchi,
                            backoffTimerIncrementFactor);
    commMgr = new KLCommMgr(neighbourhoodChangeSignificanceThreshold);
    resourceMgr = new KLResourceMgr();
}

KLKeetchi::~KLKeetchi(void)
{
    delete dataMgr;
    delete commMgr;
    delete resourceMgr;
}

// register app with the prefix
int KLKeetchi::registerApplication(string appName, string prefixName, double currentTime)
{
    // search in registeredAppInfoList to see if already registered
    int found = FALSE;
    list<KLAppInfo*>::iterator iteratorAppInfo = registeredAppInfoList.begin();
    while (iteratorAppInfo != registeredAppInfoList.end()) {
        if ((*iteratorAppInfo)->getAppName() == appName) {
            found = TRUE;
            break;
        }
        iteratorAppInfo++;
    }
    if (found) {
        return KLKEETCHI_APP_REGISTRATION_FAILED;
    }

    // app is not registered, so register
    KLAppInfo *newAppInfo = new KLAppInfo(appName, prefixName);
    registeredAppInfoList.push_back(newAppInfo);

    return KLKEETCHI_APP_REGISTRATION_SUCCEEDED;

}

// deregister an app and the related prefixes
int KLKeetchi::deregisterApplication(string appName, double currentTime)
{
    KLAppInfo *appInfo = NULL;

    // search in registeredAppInfoList to see if already registered
    int found = FALSE;
    list<KLAppInfo*>::iterator iteratorAppInfo = registeredAppInfoList.begin();
    while (iteratorAppInfo != registeredAppInfoList.end()) {
        appInfo = *iteratorAppInfo;
        if (appInfo->getAppName() == appName) {
            found = TRUE;
            break;
        }
        iteratorAppInfo++;
    }
    if (!found) {
        return KLKEETCHI_APP_DEREGISTRATION_FAILED;
    }

    // remove app
    delete appInfo;
    registeredAppInfoList.remove(appInfo);

    return KLKEETCHI_APP_DEREGISTRATION_SUCCEEDED;
}

// called to process a received Data msg from below or above layers
KLAction* KLKeetchi::processDataMsg(int fromWhere, KLDataMsg *dataMsg, double currentTime)
{
    KLAction *returnAction;
    KLCacheEntry *existingCacheEntry;
    int newGoodnessValue;
    int found;
    KLAppInfo *appInfo;
    KLDataMsg *returnDataMsg;

    // setup an empty action object to return
    returnAction = new KLAction();

    // check if data in cache
    existingCacheEntry = dataMgr->getCacheEntry(dataMsg->getDataName(), currentTime);
    if (existingCacheEntry == NULL) {

        // data seen for the first time
        dataMgr->updateCacheEntry(dataMsg->getDataName(), dataMsg->getDataPayload(),
                                    dataMsg->getDataPayloadSize(), dataMsg->getGoodnessValue(),
                                    dataMsg->getMsgType(), dataMsg->getValidUntilTime(),
                                    dataMsg->getHopsTravelled(),
                                    currentTime, dataMsg->getSimDataPayloadSize());

        // save goodness value for later use
        newGoodnessValue = dataMsg->getGoodnessValue();

    } else {

        // data already available, so check if anything has changed
        if (existingCacheEntry->getDataPayloadSize() == dataMsg->getDataPayloadSize()
            && memcmp(existingCacheEntry->getDataPayload(), dataMsg->getDataPayload(), dataMsg->getDataPayloadSize()) == 0
            && existingCacheEntry->getGoodnessValue() == dataMsg->getGoodnessValue()
            && existingCacheEntry->getDataType() == dataMsg->getMsgType()
            && existingCacheEntry->getValidUntilTime() == dataMsg->getValidUntilTime()
            && existingCacheEntry->getHopsTravelled() == dataMsg->getHopsTravelled()
            && existingCacheEntry->getSimDataPayloadSize() == dataMsg->getSimDataPayloadSize()) {

            // nothing has changed
            returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);
            returnAction->setActionType(KLACTION_ACTION_TYPE_EMPTY);

            delete existingCacheEntry;
            delete dataMsg;
            return returnAction;

        } else {

            // recompute goodness value
            newGoodnessValue = dataMgr->recomputeGoodnessValue(existingCacheEntry->getGoodnessValue(),
                                                    dataMsg->getGoodnessValue(), currentTime);

            dataMgr->updateCacheEntry(dataMsg->getDataName(), dataMsg->getDataPayload(),
                                        dataMsg->getDataPayloadSize(), newGoodnessValue,
                                        dataMsg->getMsgType(), dataMsg->getValidUntilTime(),
                                        dataMsg->getHopsTravelled(),
                                        currentTime, dataMsg->getSimDataPayloadSize());
            delete existingCacheEntry;
        }
    }

    // is the data arriving from an app running in the node
    if (fromWhere == KLKEETCHI_FROM_APP_LAYER) {

        // data from application layer is only used to update
        // the cache
        returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);
        returnAction->setActionType(KLACTION_ACTION_TYPE_EMPTY);

        delete dataMsg;
        return returnAction;

    }

    // if data comes from outside, check if there are any apps
    // registered to receive this data

    found = FALSE;
    appInfo = NULL;
    list<KLAppInfo*>::iterator iteratorAppInfo = registeredAppInfoList.begin();
    while (iteratorAppInfo != registeredAppInfoList.end()) {
        appInfo = *iteratorAppInfo;
        if (dataMsg->getDataName().find(appInfo->getPrefixName()) != string::npos) {
            found = TRUE;
            break;
        }
        iteratorAppInfo++;
    }

    if (found) {

        // if there are any apps registered, send these apps a
        // copy of the data meessage

        returnDataMsg = new KLDataMsg();

        returnDataMsg->setMsgDirection(KLDATAMSG_MSG_DIRECTION_OUT);
        returnDataMsg->setFromWhere(KLDATAMSG_FROM_TO_NOT_SET);
        returnDataMsg->setToWhere(KLDATAMSG_TO_APP_LAYER);
        returnDataMsg->setSourceAddress(ownAddress);
        returnDataMsg->setDestinationAddress(appInfo->getAppName());
        returnDataMsg->setDataName(dataMsg->getDataName());
        returnDataMsg->setDataPayload(dataMsg->getDataPayload(), dataMsg->getDataPayloadSize());
        returnDataMsg->setGoodnessValue(newGoodnessValue);
        returnDataMsg->setMsgType(dataMsg->getMsgType());
        returnDataMsg->setValidUntilTime(dataMsg->getValidUntilTime());
        returnDataMsg->setHopsTravelled(dataMsg->getHopsTravelled());
        returnDataMsg->setSimDataPayloadSize(dataMsg->getSimDataPayloadSize());

        returnAction->setActionType(KLACTION_ACTION_TYPE_DATAMSG);
        returnAction->setDataMsg(returnDataMsg);
        returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);

        delete dataMsg;
        return returnAction;
    }

    returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);
    returnAction->setActionType(KLACTION_ACTION_TYPE_EMPTY);

    delete dataMsg;
    return returnAction;
}

// called to process a received Feedback msg from below or above layers
KLAction* KLKeetchi::processFeedbackMsg(int fromWhere, KLFeedbackMsg *feedbackMsg, double currentTime)
{
    KLAction *returnAction;
    int newGoodnessValue;
    KLFeedbackMsg *returnFeedbackMsg;
    KLCacheEntry *existingCacheEntry;

    // setup an empty action object to return
    returnAction = new KLAction();

    // check if data in cache
    existingCacheEntry = dataMgr->getCacheEntry(feedbackMsg->getDataName(), currentTime);
    if (existingCacheEntry == NULL) {

        // no cache entry
        returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);
        returnAction->setActionType(KLACTION_ACTION_TYPE_EMPTY);

        delete feedbackMsg;
        return returnAction;

    }

    // is feedback from network?
    if (fromWhere == KLKEETCHI_FROM_LINK_LAYER) {

        // recompute goodness value
        newGoodnessValue = dataMgr->recomputeGoodnessValue(existingCacheEntry->getGoodnessValue(),
                                                feedbackMsg->getGoodnessValue(), currentTime);

        // update cache entry with new goodness value
        dataMgr->updateCacheEntry(feedbackMsg->getDataName(), existingCacheEntry->getDataPayload(),
                                    existingCacheEntry->getDataPayloadSize(), newGoodnessValue,
                                    existingCacheEntry->getDataType(), existingCacheEntry->getValidUntilTime(),
                                    existingCacheEntry->getHopsTravelled(),
                                    currentTime, existingCacheEntry->getSimDataPayloadSize());

        returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);
        returnAction->setActionType(KLACTION_ACTION_TYPE_EMPTY);

        delete existingCacheEntry;
        delete feedbackMsg;
        return returnAction;

    }

    // feedback is from application

    // is feedback type where the application indicates its preferences using
    // feedback?
    if (feedbackMsg->getFeedbackType() == KLFEEDBACKMSG_TYPE_PREFERENCE) {

        // recompute goodness value
        newGoodnessValue = dataMgr->recomputeGoodnessValue(existingCacheEntry->getGoodnessValue(),
                                                feedbackMsg->getGoodnessValue(), currentTime);

        // update cache entry with new goodness value
        dataMgr->updateCacheEntry(feedbackMsg->getDataName(), existingCacheEntry->getDataPayload(),
                                    existingCacheEntry->getDataPayloadSize(), newGoodnessValue,
                                    existingCacheEntry->getDataType(), existingCacheEntry->getValidUntilTime(),
                                    existingCacheEntry->getHopsTravelled(),
                                    currentTime, existingCacheEntry->getSimDataPayloadSize());

        returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);
        returnAction->setActionType(KLACTION_ACTION_TYPE_EMPTY);

        delete existingCacheEntry;
        delete feedbackMsg;
        return returnAction;
    }


    // is feedback type where the application requires the feedback to be sent
    // immediately?
    if (feedbackMsg->getFeedbackType() == KLFEEDBACKMSG_TYPE_IMMEDIATE) {

        // send feedback out with existing goodness value
        returnFeedbackMsg = new KLFeedbackMsg();

        returnFeedbackMsg->setMsgDirection(KLFEEDBACKMSG_MSG_DIRECTION_OUT);
        returnFeedbackMsg->setFromWhere(KLFEEDBACKMSG_FROM_TO_NOT_SET);
        returnFeedbackMsg->setToWhere(KLFEEDBACKMSG_TO_LINK_LAYER);
        returnFeedbackMsg->setSourceAddress(ownAddress);
        returnFeedbackMsg->setDestinationAddress(KLFEEDBACKMSG_ADDRESS_ALL);
        returnFeedbackMsg->setDataName(feedbackMsg->getDataName());
        returnFeedbackMsg->setGoodnessValue(existingCacheEntry->getGoodnessValue());
        returnFeedbackMsg->setFeedbackType(KLFEEDBACKMSG_TYPE_IMMEDIATE);

        returnAction->setActionType(KLACTION_ACTION_TYPE_FEEDBACKMSG);
        returnAction->setFeedbackMsg(returnFeedbackMsg);
        returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);

        delete existingCacheEntry;
        delete feedbackMsg;
        return returnAction;
    }

    returnAction->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);
    returnAction->setActionType(KLACTION_ACTION_TYPE_EMPTY);

    delete existingCacheEntry;
    delete feedbackMsg;
    return returnAction;
}

// called regularly to notfy the list of currently connected nodes
list<KLAction*> KLKeetchi::processNewNeighbourList(list<KLNodeInfo*> nodeInfoList, double currentTime)
{
    list<KLAction*> returnActionList;
    int changeSignificance;
    int resourceLimit;
    list<KLCacheEntry*> sendCacheEntryList;

    // determine significance of change
    // cout << "calling to determine significance of change \n";
    changeSignificance = commMgr->determineChangeSignificance(nodeInfoList, currentTime);

    // determine communication limitations
    resourceLimit = resourceMgr->determineResourceLimitationStatus(currentTime);

    // Determine what Data to send out
    sendCacheEntryList = dataMgr->getCacheEntriesToSend(changeSignificance, resourceLimit, currentTime);

    // build the action objects to return
    list<KLCacheEntry*>::iterator iteratorCacheEntry = sendCacheEntryList.begin();
    while (iteratorCacheEntry != sendCacheEntryList.end()) {
        KLCacheEntry *cacheEntry = *iteratorCacheEntry;

        // create a data msg to associate with the action
        KLDataMsg *dataMsg = new KLDataMsg();
        dataMsg->setMsgDirection(KLDATAMSG_MSG_DIRECTION_OUT);
        dataMsg->setFromWhere(KLDATAMSG_FROM_TO_NOT_SET);
        dataMsg->setToWhere(KLDATAMSG_TO_LINK_LAYER);
        dataMsg->setSourceAddress(ownAddress);
        dataMsg->setDestinationAddress(KLDATAMSG_ADDRESS_ALL);
        dataMsg->setDataName(cacheEntry->getDataName());
        dataMsg->setDataPayload(cacheEntry->getDataPayload(), cacheEntry->getDataPayloadSize());
        dataMsg->setGoodnessValue(cacheEntry->getGoodnessValue());
        dataMsg->setMsgType(cacheEntry->getDataType());
        dataMsg->setValidUntilTime(cacheEntry->getValidUntilTime());
        dataMsg->setHopsTravelled(cacheEntry->getHopsTravelled());
        dataMsg->setSimDataPayloadSize(cacheEntry->getSimDataPayloadSize());

        // create the action to return with the data msg
        KLAction *action = new KLAction();
        action->setActionType(KLACTION_ACTION_TYPE_DATAMSG);
        action->setDataMsg(dataMsg);
        action->setProcessingStatus(KLACTION_MSG_PROCESSING_SUCCESSFUL);

        returnActionList.push_front(action);

        iteratorCacheEntry++;

        // if (ownAddress == "02:00:00:00:00:00") {
        //     cout << ownAddress << " " << currentTime << (changeSignificance == KLKEETCHI_SIGNIFICANT_CHANGE ? " -Sig- " : " -Non- ") << " cache = " << dataMgr->getCurrentCacheSize() << "\n";
        //     cout << " - sending " << dataMsg->getDataName() << " with " << dataMsg->getGoodnessValue() << "\n";
        // }
    }

    // update new neighbour list
    commMgr->updateNeighbours(nodeInfoList, currentTime);

    // remove temporary cache enntries in sendCacheEntryList
    while (sendCacheEntryList.size() > 0) {
        list<KLCacheEntry*>::iterator iteratorCacheEntry = sendCacheEntryList.begin();
        KLCacheEntry *cacheEntry = *iteratorCacheEntry;
        sendCacheEntryList.remove(cacheEntry);
        delete cacheEntry;
    }

    // remove received neighbour list
    while (nodeInfoList.size() > 0) {
        list<KLNodeInfo*>::iterator iteratorNodeInfo = nodeInfoList.begin();
        KLNodeInfo *nodeInfo = *iteratorNodeInfo;
        nodeInfoList.remove(nodeInfo);
        delete nodeInfo;
    }

    return returnActionList;
}

int KLKeetchi::ageData(double currentTime)
{
    dataMgr->ageCacheEntries(currentTime);

    return 0;
}

// storage for inputInfo and outputInfo must be allocated by the caller
int KLKeetchi::getStatus(int statusType, void *inputInfo, void *outputInfo)
{
    if (statusType == KLKEETCHI_DATA_ITEM_PRESENCE) {
        string dName = *((string *) inputInfo);
        int rtn = dataMgr->checkCacheEntryPresence(dName);
        *((int *) outputInfo) = rtn;

        return 1;
    }

    return 0;
}
