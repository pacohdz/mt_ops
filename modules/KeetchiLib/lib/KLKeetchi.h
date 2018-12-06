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
* Main class that is instantiated by the Keetchi layer of every node to perform
* all Keetchi related activities. There should only be one instance of these
* objects per node.
*
* @author : Asanga Udugama (adu@comnets.uni-bremen.de)
* @date   : 18-June-2015
*
*/

#ifndef KLKeetchi_H
#define KLKeetchi_H

#include <list>
#include <string>
#include <cstring>
#include <iostream>

#include "KLTypes.h"
#include "KLAction.h"
#include "KLNodeInfo.h"
#include "KLFeedbackMsg.h"
#include "KLDataMsg.h"
#include "KLDataMgr.h"
#include "KLCacheEntry.h"
#include "KLAppInfo.h"
#include "KLCommMgr.h"
#include "KLResourceMgr.h"

using namespace std;

class KLKeetchi
{
    public:
        KLKeetchi(int cachePolicy, int cacheSize, string ownAddr, double changeSigThreshold,
                  double coolOffDur, double learningConst, int simKeetchi, double backoffTimerInc);
        ~KLKeetchi(void);

        int registerApplication(string appName, string prefixName, double currentTime);
        int deregisterApplication(string appName, double currentTime);
        KLAction* processFeedbackMsg(int fromWhere, KLFeedbackMsg *feedbackMsg, double currentTime);
        KLAction* processDataMsg(int fromWhere, KLDataMsg *dataMsg, double currentTime);
        list<KLAction*> processNewNeighbourList(list<KLNodeInfo*> nodeInfoList, double currentTime);
        int ageData(double currentTime);
        int getStatus(int statusType, void *inputInfo, void *outputInfo);

    private:
        int maxCacheSize; // maximum size in bytes
        int cacheReplacementPolicy; // policy -> LRU, etc
        string ownAddress; // node's own MAC address
        double neighbourhoodChangeSignificanceThreshold; // 0.0 to 1.0 value indicating change significance
                                                         // e.g., 0.25 means a 25% change is sufficient to consider
                                                         // as a significant change in neighbourhood
        double coolOffDuration; // duration (in sec) the node stays silent without sending data out
                                // after repeated insiginificant changes
        double learningConstant; // the constant that gives the weight assigned to the current
                                 // goodness value in cache when computing the new value
                                 // together with the recently arrived goodness value (in
                                 // a data or feedback) - a value between 0.0 - 1.0
        int simulatedKeetchi; // is KeetchiLib being used for simulations or not - true = simulated
        double backoffTimerIncrementFactor; // increament factor used to incease the sending duration when
                                            // there is a continuous insignificant change in neighbourhood

        KLDataMgr *dataMgr;
        KLCommMgr *commMgr;
        KLResourceMgr *resourceMgr;
        list<KLAppInfo*> registeredAppInfoList;

};



#endif
