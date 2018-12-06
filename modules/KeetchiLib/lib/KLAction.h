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
* Information returned after processing is done
*
* @author : Asanga Udugama (adu@comnets.uni-bremen.de)
* @date   : 18-June-2015
*
*/
#ifndef KLAction_H
#define KLAction_H

#include <list>
// using std::list;

#include "KLTypes.h"
#include "KLFeedbackMsg.h"
#include "KLDataMsg.h"

using namespace std;
	
class KLAction
{
	public:
		KLAction(void);
		~KLAction(void);
		
		// getters
		int getActionType() {return actionType; }
		KLDataMsg *getDataMsg() {return dataMsg; }
		KLFeedbackMsg *getFeedbackMsg() {return feedbackMsg; }
		int getProcessingStatus() {return processingStatus; }
		
		// setters
		void setActionType(int acType) {actionType = acType; }  	
		void setDataMsg(KLDataMsg *dMsg) {dataMsg = dMsg; }
		void setFeedbackMsg(KLFeedbackMsg *fMsg) {feedbackMsg = fMsg; }
		void setProcessingStatus(int pStatus) {processingStatus = pStatus; }  	
		
	private:
		int actionType; // KLACTION_ACTION_TYPE_... action with data, feedback or nothing
		KLDataMsg *dataMsg;
		KLFeedbackMsg *feedbackMsg;
		
		int processingStatus; // KLACTION_MSG_PROCESSING_...

};

#endif
