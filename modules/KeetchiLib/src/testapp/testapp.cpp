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
* @author : Jens Dede (adu@comnets.uni-bremen.de)
* @date   : 5-January-2016
*
*/


#include <iostream>
#include <string>
#include <ctime>
#include <KLKeetchi.h>
#include <config.h>

int main(){
    std::cout << "Testapp for \"" << PACKAGE_NAME << "\"" << std::endl;

    int cachePolicy = 0;
    int cacheSize = 0;
    string ownAddr = "00:11:22:33:44:55";
    double changeSigThresh = 0.25;
    double silenceDur = 300.0;
    double learningConst = 0.5;
    int simKeetchi = TRUE;
    double backoffTimerIncFactor = 0.5;
    int ret;

    double time = static_cast<double>(std::time(NULL));

    KLKeetchi *keetchi = new KLKeetchi(cachePolicy, cacheSize, ownAddr, changeSigThresh, silenceDur,
                                       learningConst, simKeetchi, backoffTimerIncFactor);
    ret = keetchi->registerApplication("MyApp", "myAddress", time);
    std::cout << "Return value: " << ret << std::endl;
    ret = keetchi->registerApplication("MyApp", "myAddress", time);
    std::cout << "Return value: " << ret << std::endl;
}
