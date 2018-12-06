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
* Holds a single cache entry. A cache entry contains the content of a
* single Data message.
*
* @author : Asanga Udugama (adu@comnets.uni-bremen.de)
* @date   : 18-June-2015
*
*/

#ifndef KLCacheEntry_H
#define KLCacheEntry_H

#include <string>
#include <cstring>
#include <cstdlib>

#include "KLTypes.h"

using namespace std;

class KLCacheEntry
{
    public:
        KLCacheEntry(string dName, char *dPayload, int dPayloadSize, int gValue, int mType,
                     double vuTime, int hTravelled, double cTime, int dSimPayloadSize);
        ~KLCacheEntry(void);

        // getters
        string getDataName() {return dataName; }
        char *getDataPayload() {return dataPayload; }
        int getDataPayloadSize() {return dataPayloadSize; }
        int getGoodnessValue() {return goodnessValue; }
        int getDataType() {return dataType; }
        double getValidUntilTime() {return validUntilTime; }
        int getHopsTravelled() {return hopsTravelled; }

        double getCreatedTime() {return createdTime; }
        double getUpdatedTime() {return updatedTime; }
        double getLastAccessedTime() {return lastAccessedTime; }

        int getSimDataPayloadSize() {return simDataPayloadSize; }

        // setters
        void setDataName(string s) {dataName = s; }
        void setDataPayload(char *d, int ds) {dataPayload = (char *)malloc(ds + 1);
                                                        memset(dataPayload, '\0', (ds + 1));
                                                        memcpy(dataPayload, d, ds);
                                                        dataPayloadSize = ds; }
        void setGoodnessValue(int gv) {goodnessValue = gv; }
        void setDataType(int dt) {dataType = dt; }
        void setValidUntilTime(double vut) {validUntilTime = vut; }
        void setHopsTravelled(int ht) {hopsTravelled = ht; }

        void setCreatedTime(double ct) {createdTime = ct; }
        void setUpdatedTime(double ut) {updatedTime = ut; }
        void setLastAccessedTime(double lat) {lastAccessedTime = lat; }

        void setSimDataPayloadSize(int ds) {simDataPayloadSize = ds; }

        // utility methods
        KLCacheEntry *makeCopy(void);

    private:
        string dataName;
        char *dataPayload;
        int dataPayloadSize;
        int goodnessValue;
        int dataType;
        double validUntilTime;
        int hopsTravelled;

        double createdTime;
        double updatedTime;
        double lastAccessedTime;

        // used when KeetchiLib used for simulations
        int simDataPayloadSize;
    };

#endif
