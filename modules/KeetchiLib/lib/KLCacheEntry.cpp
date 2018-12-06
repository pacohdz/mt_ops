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
#include "KLCacheEntry.h"

KLCacheEntry::KLCacheEntry(string dName, char *dPayload, int dPayloadSize, int gValue, int dType,
                           double vuTime, int hTravelled, double cTime, int dSimPayloadSize)
{
    dataName = dName;
    dataPayload = (char *) malloc(dPayloadSize + 1);
    memset(dataPayload, '\0', (dPayloadSize + 1));
    memcpy(dataPayload, dPayload, dPayloadSize);
    dataPayloadSize = dPayloadSize;
    goodnessValue = gValue;
    dataType = dType;
    validUntilTime = vuTime;
    hopsTravelled = hTravelled;

    createdTime = cTime;
    updatedTime = cTime;
    lastAccessedTime = cTime;

    simDataPayloadSize = dSimPayloadSize;
}

KLCacheEntry::~KLCacheEntry(void)
{
    free(dataPayload);
}

KLCacheEntry* KLCacheEntry::makeCopy(void)
{
    KLCacheEntry* copiedCacheEntry;

    copiedCacheEntry = new KLCacheEntry(dataName, dataPayload, dataPayloadSize, goodnessValue, dataType,
                                        validUntilTime, hopsTravelled, createdTime, simDataPayloadSize);
    copiedCacheEntry->setCreatedTime(createdTime);
    copiedCacheEntry->setUpdatedTime(updatedTime);
    copiedCacheEntry->setLastAccessedTime(lastAccessedTime);

    return copiedCacheEntry;
}
