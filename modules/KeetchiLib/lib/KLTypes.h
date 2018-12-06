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
* @date   : 22-Sept-2015
*
*/
#ifndef KLTypes_H
#define KLTypes_H

#define TRUE                            1
#define FALSE                           0
#define YES                             1
#define NO                              0
// #define NULL                         0

//
// KLKeetchi class related types
//
#define KLKEETCHI_APP_REGISTRATION_FAILED           0
#define KLKEETCHI_APP_REGISTRATION_SUCCEEDED        1
#define KLKEETCHI_APP_DEREGISTRATION_FAILED         0
#define KLKEETCHI_APP_DEREGISTRATION_SUCCEEDED      1

// caching related flags
#define KLKEETCHI_CACHE_REPLACEMENT_POLICY_LRU      1
#define KLKEETCHI_CACHE_REPLACEMENT_POLICY_Q        2

// From where messages arrive
#define KLKEETCHI_FROM_APP_LAYER                    1
#define KLKEETCHI_FROM_LINK_LAYER                   2

// Neighbourhood change classifications
#define KLKEETCHI_SIGNIFICANT_CHANGE                1
#define KLKEETCHI_INSIGNIFICANT_CHANGE              2

// Resource limitation status
#define KLKEETCHI_RESOURCE_CAPACITY_LEVEL_01        1
#define KLKEETCHI_RESOURCE_CAPACITY_LEVEL_99        99

// Status types
#define KLKEETCHI_DATA_ITEM_PRESENCE                1   // input: data name as a string
                                                        // output: int 1 = yes, 0 = no

//
// KLDataMsg class related types
//
#define KLDATAMSG_MAX_DATA_PAYLOAD_SIZE             1024

// used to indicate whether a message has to be sent
// to someone else or is being received
#define KLDATAMSG_MSG_DIRECTION_NOT_SET             0
#define KLDATAMSG_MSG_DIRECTION_IN                  1
#define KLDATAMSG_MSG_DIRECTION_OUT                 2

// From where or to where the message arrived/to send
#define KLDATAMSG_FROM_TO_NOT_SET                   0
#define KLDATAMSG_FROM_APP_LAYER                    1
#define KLDATAMSG_FROM_LINK_LAYER                   2
#define KLDATAMSG_TO_APP_LAYER                      3
#define KLDATAMSG_TO_LINK_LAYER                     4

#define KLDATAMSG_ADDRESS_ALL                       "all"

#define KLDATAMSG_MSG_TYPE_NONE                     0
#define KLDATAMSG_MSG_TYPE_EMERGENCY                1

//
// KLFeedbackMsg class related types
//

// used to indicate whether a message hast to be sent
// to someone else or is being received
#define KLFEEDBACKMSG_MSG_DIRECTION_NOT_SET             0
#define KLFEEDBACKMSG_MSG_DIRECTION_IN                  1
#define KLFEEDBACKMSG_MSG_DIRECTION_OUT                 2

// From where or to where the message arrived/to send
#define KLFEEDBACKMSG_FROM_TO_NOT_SET                   0
#define KLFEEDBACKMSG_FROM_APP_LAYER                    1
#define KLFEEDBACKMSG_FROM_LINK_LAYER                   2
#define KLFEEDBACKMSG_TO_APP_LAYER                      3
#define KLFEEDBACKMSG_TO_LINK_LAYER                     4

// all neighbours
#define KLFEEDBACKMSG_ADDRESS_ALL                       "FF:FF:FF:FF:FF:FF"

// feedback type
#define KLFEEDBACKMSG_TYPE_NONE                         0
#define KLFEEDBACKMSG_TYPE_IMMEDIATE                    1
#define KLFEEDBACKMSG_TYPE_PREFERENCE                   2


//
// KLCacheEntry class related types
//
#define KLCACHEENTRY_MAX_DATA_PAYLOAD_SIZE          1024


//
// KLAction class related types
//
#define KLACTION_ACTION_TYPE_EMPTY                  0
#define KLACTION_ACTION_TYPE_DATAMSG                1
#define KLACTION_ACTION_TYPE_FEEDBACKMSG            2

#define KLACTION_MSG_PROCESSING_SUCCESSFUL          0
#define KLACTION_MSG_PROCESSING_FAILED              -1

//
// KLDataMgr class related types
//
#define KLDATAMGR_INITIAL_BACKOFF_DURATION          1.0

#endif
