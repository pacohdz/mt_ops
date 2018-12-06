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
* Data message contents
*
* @author : Asanga Udugama (adu@comnets.uni-bremen.de)
* @date   : 18-June-2015
*
*/

#ifndef KLDataMsg_H
#define KLDataMsg_H

#include <string>
#include <cstring>
#include <cstdlib>

#include "KLTypes.h"

using namespace std;

class KLDataMsg
{
    public:
        KLDataMsg(void);
        ~KLDataMsg(void);

        // getters
        int getMsgDirection() {return msgDirection; }
        int getFromWhere() {return fromWhere; }
        int getToWhere() {return toWhere; }
        string getSourceAddress() {return sourceAddress; }
        string getDestinationAddress() {return destinationAddress; }
        string getDataName() {return dataName; }
        char *getDataPayload() {return dataPayload; }
        int getDataPayloadSize() {return dataPayloadSize; }
        int getGoodnessValue() {return goodnessValue; }
        int getMsgType() {return msgType; }
        int getValidUntilTime() {return validUntilTime; }
        int getHopsTravelled() {return hopsTravelled; }
        int getSimDataPayloadSize() {return simDataPayloadSize; }

        // setters
        void setMsgDirection(int md) {msgDirection = md; }
        void setFromWhere(int fw) {fromWhere = fw; }
        void setToWhere(int tw) {toWhere = tw; }
        void setSourceAddress(string s) {sourceAddress = s; }
        void setDestinationAddress(string s) {destinationAddress = s; }
        void setDataName(string s) {dataName = s; }
        void setDataPayload(char *d, int ds) {if (d == NULL || ds == 0) {
                                                dataPayload = NULL;
                                                dataPayloadSize = 0;
                                              } else {
                                                dataPayload = (char *)malloc(ds + 1);
                                                memset(dataPayload, '\0', (ds + 1));
                                                memcpy(dataPayload, d, ds);
                                                dataPayloadSize = ds;
                                              }
                                            }
        void setGoodnessValue(int gv) {goodnessValue = gv; }

        void setMsgType(int mt) {msgType = mt; }
        void setValidUntilTime(int vut) {validUntilTime = vut; }
        void setHopsTravelled(int ht) {hopsTravelled = ht; }
        void setSimDataPayloadSize(int ds) {simDataPayloadSize = ds; }


    private:
        // check KLTypes to get values for the variables below
        int msgDirection;
        int fromWhere;
        int toWhere;
        string sourceAddress;
        string destinationAddress;

        string dataName;
        char *dataPayload;
        int dataPayloadSize;

        int goodnessValue;

        int msgType;
        double validUntilTime;

        int hopsTravelled;

        // used when KeetchiLib used for simulations
        int simDataPayloadSize;
};


#endif

