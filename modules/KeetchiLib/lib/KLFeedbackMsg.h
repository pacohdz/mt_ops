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
* Feedback message data
*
* @author : Asanga Udugama (adu@comnets.uni-bremen.de)
* @date   : 18-June-2015
*
*/

#ifndef KLFeedbackMsg_H
#define KLFeedbackMsg_H

#include <string>
#include "KLTypes.h"

using namespace std;

class KLFeedbackMsg
{
    public:
        KLFeedbackMsg(void);
        ~KLFeedbackMsg(void);

        // getters & setters
        int getMsgDirection() {return msgDirection; }
        int getFromWhere() {return fromWhere; }
        int getToWhere() {return toWhere; }
        string getSourceAddress() {return sourceAddress; }
        string getDestinationAddress() {return destinationAddress; }
        string getDataName() {return dataName; }
        int getGoodnessValue() {return goodnessValue; }
        int getFeedbackType() {return feedbackType; }

        void setMsgDirection(int md) {msgDirection = md; }
        void setFromWhere(int fw) {fromWhere = fw; }
        void setToWhere(int tw) {toWhere = tw; }
        void setSourceAddress(string s) {sourceAddress = s; }
        void setDestinationAddress(string s) {destinationAddress = s; }
        void setDataName(string s) {dataName = s; }
        void setGoodnessValue(int gv) {goodnessValue = gv; }
        void setFeedbackType(int ft) {feedbackType = ft; }

    private:
        int msgDirection;
        int fromWhere;
        int toWhere;
        string sourceAddress;
        string destinationAddress;

        string dataName;
        int goodnessValue;

        int feedbackType;

};

#endif

