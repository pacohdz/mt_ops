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
* @date   : 12-Sept-2015
*
*/
#include "KLDataMgr.h"

KLDataMgr::KLDataMgr(int cachePolicy, int cacheSize, double coolOffDur, double learningConst,
                     int simKeetchi, double backoffTimerIncFactor)
{
    cacheReplacementPolicy = cachePolicy;
    maxCacheSize = cacheSize;
    coolOffDuration = coolOffDur;
    learningConstant = learningConst;
    currentCacheSize = 0;
    lastFocusIndex = -1;
    coolOffEndTime = 0.0;

    simulatedKeetchi = simKeetchi;
    simulatedCurrentCacheSize = 0;

    backoffTimerIncrementFactor = backoffTimerIncFactor;
    currentBackoffTimerDuration = KLDATAMGR_INITIAL_BACKOFF_DURATION;
    backoffTimerExpirationTime = 0.0;

    srand(128);
}

KLDataMgr::~KLDataMgr(void)
{
    KLCacheEntry *foundCacheEntry;

    // remove all cache entries
    list<KLCacheEntry*>::iterator iteratorCacheEntry = cacheEntryList.begin();
    while (iteratorCacheEntry != cacheEntryList.end()) {
        foundCacheEntry = *iteratorCacheEntry;
        delete foundCacheEntry;
        iteratorCacheEntry++;
    }

    cacheEntryList.clear();

}

KLCacheEntry* KLDataMgr::getCacheEntry(string dName, double cTime)
{
    KLCacheEntry *foundCacheEntry = NULL, *copiedCacheEntry = NULL;

    // look for entry
    int found = FALSE;
    list<KLCacheEntry*>::iterator iteratorCacheEntry = cacheEntryList.begin();
    while (iteratorCacheEntry != cacheEntryList.end()) {
        if ((*iteratorCacheEntry)->getDataName() == dName) {
            foundCacheEntry = *iteratorCacheEntry;
            found = TRUE;
            break;
        }
        iteratorCacheEntry++;
    }

    // make a copy, if found, to return and update access time
    if (found) {
        copiedCacheEntry = new KLCacheEntry(foundCacheEntry->getDataName(), foundCacheEntry->getDataPayload(),
                                            foundCacheEntry->getDataPayloadSize(),
                                            foundCacheEntry->getGoodnessValue(),
                                            foundCacheEntry->getDataType(), foundCacheEntry->getValidUntilTime(),
                                            foundCacheEntry->getHopsTravelled(),
                                            cTime, foundCacheEntry->getSimDataPayloadSize());
        foundCacheEntry->setLastAccessedTime(cTime);
    }

    return copiedCacheEntry;
}

int KLDataMgr::updateCacheEntry(string dName, char *dPayload, int dPayloadSize, int gValue, int dType,
                                double vuTime, int hTravelled, double cTime, int dSimPayloadSize)
{
    KLCacheEntry *foundCacheEntry = NULL, *updatedCacheEntry = NULL;

    // look for entry
    int found = FALSE;
    list<KLCacheEntry*>::iterator iteratorCacheEntry = cacheEntryList.begin();
    while (iteratorCacheEntry != cacheEntryList.end()) {
        if ((*iteratorCacheEntry)->getDataName() == dName) {
            foundCacheEntry = *iteratorCacheEntry;
            found = TRUE;
            break;
        }
        iteratorCacheEntry++;
    }

    // create a cache entry with the new values
    updatedCacheEntry = new KLCacheEntry(dName, dPayload,
                                        dPayloadSize,
                                        gValue, dType, vuTime, hTravelled,
                                        cTime, dSimPayloadSize);

    // if the cache had a previous entry, remove the existing entry and set old time stamps
    if (found) {
        currentCacheSize -= foundCacheEntry->getDataPayloadSize();
        simulatedCurrentCacheSize -= foundCacheEntry->getSimDataPayloadSize();
        cacheEntryList.remove(foundCacheEntry);
        updatedCacheEntry->setCreatedTime(foundCacheEntry->getCreatedTime());
        updatedCacheEntry->setLastAccessedTime(foundCacheEntry->getLastAccessedTime());

        delete foundCacheEntry;
    }

    // search the cache to find where to insert the entry
    // i.e., ordered by goodness value
    KLCacheEntry *nextCacheEntry;
    iteratorCacheEntry = cacheEntryList.begin();
    while (iteratorCacheEntry != cacheEntryList.end()) {
        nextCacheEntry = *iteratorCacheEntry;
        if (updatedCacheEntry->getGoodnessValue() > nextCacheEntry->getGoodnessValue()) {
            break;
        }
        iteratorCacheEntry++;

    }

    // position found, so insert the entry at the position before
    cacheEntryList.insert(iteratorCacheEntry, updatedCacheEntry);

    // increment cache size
    currentCacheSize += dPayloadSize;
    simulatedCurrentCacheSize += dSimPayloadSize;

    if (simulatedKeetchi) {
        // remove entries if cache exceeded limit (maxCacheSize == 0 means unlimited cache)
        if (maxCacheSize != 0 && simulatedCurrentCacheSize > maxCacheSize) {
            while (simulatedCurrentCacheSize > maxCacheSize) {
                iteratorCacheEntry = cacheEntryList.end();
                iteratorCacheEntry--;
                KLCacheEntry *removalCacheEntry = *iteratorCacheEntry;
                cacheEntryList.remove(removalCacheEntry);
                simulatedCurrentCacheSize -= removalCacheEntry->getSimDataPayloadSize();
                currentCacheSize -= removalCacheEntry->getDataPayloadSize();
                delete removalCacheEntry;
            }
        }

    } else {
        // remove entries if cache exceeded limit (maxCacheSize == 0 means unlimited cache)
        if (maxCacheSize != 0 && currentCacheSize > maxCacheSize) {
            while (currentCacheSize > maxCacheSize) {
                iteratorCacheEntry = cacheEntryList.end();
                iteratorCacheEntry--;
                KLCacheEntry *removalCacheEntry = *iteratorCacheEntry;
                cacheEntryList.remove(removalCacheEntry);
                currentCacheSize -= removalCacheEntry->getDataPayloadSize();
                simulatedCurrentCacheSize -= removalCacheEntry->getSimDataPayloadSize();
                delete removalCacheEntry;
            }
        }
    }

    // // print cache -> temp code
    // if (cacheEntryList.size() == 10) {
    //     cout << "---- begin ------\n";
    //     iteratorCacheEntry = cacheEntryList.begin();
    //     while (iteratorCacheEntry != cacheEntryList.end()) {
    //         nextCacheEntry = *iteratorCacheEntry;
    //         cout << "data: " << nextCacheEntry->getDataName() << ", gval:" << nextCacheEntry->getGoodnessValue() << "\n";
    //         iteratorCacheEntry++;
    //     }
    //     cout << "---- end ------\n";
    // }

    return 0;
}

int KLDataMgr::recomputeGoodnessValue(int curValue, int rcvdValue, double cTime)
{
    double newValue;
    int retValue;

    // // get simple average
    // newValue = (curValue + rcvdValue) / 2;

    // compute new goodness value using learning constant (gamma)
    newValue = (curValue * learningConstant) + (rcvdValue * (1.0 - learningConstant));
    newValue += 0.5;
    retValue = floor(newValue);

    return retValue;
}

list<KLCacheEntry*> KLDataMgr::getCacheEntriesToSend(int changeSignificance, int resourceLimit,
                                                     double cTime)
{
    list<KLCacheEntry*> returnCacheEntryList;
    KLCacheEntry *selecedCacheEntry, *copiedCacheEntry;
    list<double> weightList;
    list<double>::iterator iteratorDouble;

    // cache is empty
    if (cacheEntryList.size() == 0) {
        lastFocusIndex = -1;
        return returnCacheEntryList;
    }

    if (changeSignificance == KLKEETCHI_SIGNIFICANT_CHANGE) {

        // is cool off period active?
        if (cTime < coolOffEndTime) {

            // terminate the cool off period
            coolOffEndTime = cTime;
        }

        currentBackoffTimerDuration = KLDATAMGR_INITIAL_BACKOFF_DURATION;
        backoffTimerExpirationTime = cTime + currentBackoffTimerDuration;

        // set focus index to the begining of cache
        lastFocusIndex = 0;

        // build distribution around the focus index and get a random index
        int sendDataIndex = buildDistributionAndReturnRandomIndex(cacheEntryList.size(), lastFocusIndex);

        // retrieve cache entry
        list<KLCacheEntry*>::iterator iteratorCacheEntry = cacheEntryList.begin();
        advance(iteratorCacheEntry, sendDataIndex);
        selecedCacheEntry = *iteratorCacheEntry;

        // make a copy and set return variable
        copiedCacheEntry = selecedCacheEntry->makeCopy();
        returnCacheEntryList.push_back(copiedCacheEntry);



    } else {

        // KLKEETCHI_INSIGNIFICANT_CHANGE


        // is cool off period active?
        if (cTime < coolOffEndTime) {

            // if so, dont send any data
            return returnCacheEntryList;
        }

        // is backoff timer active?
        if (cTime < backoffTimerExpirationTime) {

            // if so, dont send any data
            return returnCacheEntryList;
        }

        // move the focus index to the next entry
        lastFocusIndex += 1;

        // is valid focus index
        if (lastFocusIndex < cacheEntryList.size()) {

            // yes, it is a valid focus index

            // build distribution around the focus index and get a random index
            int sendDataIndex = buildDistributionAndReturnRandomIndex(cacheEntryList.size(), lastFocusIndex);

            // retrieve cache entry
            list<KLCacheEntry*>::iterator iteratorCacheEntry = cacheEntryList.begin();
            advance(iteratorCacheEntry, sendDataIndex);
            selecedCacheEntry = *iteratorCacheEntry;

            // make a copy and set return variable
            copiedCacheEntry = selecedCacheEntry->makeCopy();
            returnCacheEntryList.push_back(copiedCacheEntry);

            // set backoff timer for next send
            currentBackoffTimerDuration = currentBackoffTimerDuration * backoffTimerIncrementFactor;
            backoffTimerExpirationTime = cTime + currentBackoffTimerDuration;


        } else {
            // no, it is an invalid focus index

            // if the focus index reached beyond cache size,
            // stop sending any more data until a significant
            // change occurs or the cool off period is reached
            lastFocusIndex = -1;
            coolOffEndTime = cTime + coolOffDuration;

            // clear the backoff timer
            currentBackoffTimerDuration = KLDATAMGR_INITIAL_BACKOFF_DURATION;
            backoffTimerExpirationTime = 0.0;

        }
    }

    return returnCacheEntryList;
}

int KLDataMgr::ageCacheEntries(double cTime)
{
    list<KLCacheEntry*>::iterator iteratorCacheEntry = cacheEntryList.begin();
    while (iteratorCacheEntry != cacheEntryList.end()) {
        int goodnessValue = (*iteratorCacheEntry)->getGoodnessValue();
        if (goodnessValue > 0) {
            goodnessValue -= 1;
            (*iteratorCacheEntry)->setGoodnessValue(goodnessValue);
        }
        iteratorCacheEntry++;
    }
    return 0;
}

int KLDataMgr::checkCacheEntryPresence(string dName)
{
    int found = FALSE;
    list<KLCacheEntry*>::iterator iteratorCacheEntry = cacheEntryList.begin();
    while (iteratorCacheEntry != cacheEntryList.end()) {
        if ((*iteratorCacheEntry)->getDataName() == dName) {
            return TRUE;
        }
        iteratorCacheEntry++;
    }
    return 0;
}

int KLDataMgr::buildDistributionAndReturnRandomIndex(int cacheEntryCount, int currentFocusIndex)
{
    list<double> weightList;
    int i;
    double totalWeights;
    double cdfAccumulator;
    int returnIndex;

    // initialize arrays
    for (i = 0; i < cacheEntryCount; i++) {
        weightList.push_back(0.0);
    }

    // set value of focus index (i.e., peak of distribution)
    list<double>::iterator iteratorFocusIndex = weightList.begin();
    advance(iteratorFocusIndex, currentFocusIndex);
    *iteratorFocusIndex = 1000.0;
    totalWeights = 1000.0;

    // fill left half
    i = currentFocusIndex - 1;
    while (i >= 0) {

        // get previous weight
        list<double>::iterator iteratorPreviousIndex = weightList.begin();
        advance(iteratorPreviousIndex, (i + 1));

        // get current weight (which is 0)
        list<double>::iterator iteratorCurrentIndex = weightList.begin();
        advance(iteratorCurrentIndex, i);

        // compute current weight
        *iteratorCurrentIndex = *iteratorPreviousIndex / 2.0;

        // accumulate sum
        totalWeights += *iteratorCurrentIndex;

        i--;
    }

    // fill right half
    i = currentFocusIndex + 1;
    while (i < cacheEntryCount) {

        // get previous weight
        list<double>::iterator iteratorPreviousIndex = weightList.begin();
        advance(iteratorPreviousIndex, (i - 1));

        // get current weight (which is 0)
        list<double>::iterator iteratorCurrentIndex = weightList.begin();
        advance(iteratorCurrentIndex, i);

        // compute current weight
        *iteratorCurrentIndex = *iteratorPreviousIndex / 2.0;

        // accumulate sum
        totalWeights += *iteratorCurrentIndex;

        i++;
    }

    // get random number between 0.0 and 1.0
    double randomDouble = ((double) rand() / (double) (RAND_MAX));

    // select the index relevant for the random number using the
    // CDF of the weights
    i = 0;
    cdfAccumulator = 0.0;
    returnIndex = -1;
    while (i < cacheEntryCount) {

        // get weight
        list<double>::iterator iteratorWeightIndex = weightList.begin();
        advance(iteratorWeightIndex, i);

        double probablityForIndex = *iteratorWeightIndex / totalWeights;

        cdfAccumulator += probablityForIndex;

        if (randomDouble <= cdfAccumulator) {
            returnIndex = i;
            break;
        }

        i++;
    }

    return returnIndex;
}

