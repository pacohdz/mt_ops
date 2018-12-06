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
* Manages the operations related to resources.
*
* @author : Asanga Udugama (adu@comnets.uni-bremen.de)
* @date   : 22-Sept-2015
*
*/

#ifndef KLResourceMgr_H
#define KLResourceMgr_H

#include <list>
#include <string>

#include "KLTypes.h"

using namespace std;

class KLResourceMgr
{
	public:
		KLResourceMgr(void);
		~KLResourceMgr(void);

		// operations
		int determineResourceLimitationStatus(double cTime);

	private:
		int batteryLevel;

};

#endif
