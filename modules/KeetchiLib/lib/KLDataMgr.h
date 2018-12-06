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
* Manages the operations related to managing data including
* the cache.
*
* @author : Asanga Udugama (adu@comnets.uni-bremen.de)
* @date   : 12-Sept-2015
*
*/

#ifndef KLDataMgr_H
#define KLDataMgr_H

#include <string>
#include <list>
#include <iostream>
#include <math.h>

#include "KLTypes.h"
#include "KLCacheEntry.h"

using namespace std;

class KLDataMgr
{
    public:
        KLDataMgr(int cachePolicy, int cacheSize, double coolOffDur, double learningConst, int simKeetchi,
                  double backoffTimerIncFactor);
        ~KLDataMgr(void);

        // getters
        int getCurrentCacheSize() {if (simulatedKeetchi) {
                                       return simulatedCurrentCacheSize;
                                   } else {
                                       return currentCacheSize; }}

        // operations
        KLCacheEntry* getCacheEntry(string dName, double cTime);
        int updateCacheEntry(string dName, char *dPayload, int dPayloadSize, int gValue, int dType,
                             double vuTime, int hTravelled, double cTime, int dSimPayloadSize);
        int recomputeGoodnessValue(int curValue, int rcvdValue, double cTime);
        list<KLCacheEntry*> getCacheEntriesToSend(int changeSignificance, int resourceLimit, double cTime);
        int ageCacheEntries(double cTime);
        int checkCacheEntryPresence(string dName);

    private:
        list<KLCacheEntry*> cacheEntryList;
        int cacheReplacementPolicy; // policy -> LRU, etc
        int maxCacheSize;  // size in bytes
        double coolOffDuration;
        double learningConstant;

        int currentCacheSize;
        int lastFocusIndex;
        double coolOffEndTime;

        int simulatedKeetchi;
        int simulatedCurrentCacheSize;

        double backoffTimerIncrementFactor;
        double backoffTimerExpirationTime;
        double currentBackoffTimerDuration;

        int buildDistributionAndReturnRandomIndex(int cacheEntryCount, int currentFocusIndex);

    };

#endif
