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
* @date   : 16-Sept-2015
*
*/
#include "KLCommMgr.h"

KLCommMgr::KLCommMgr(double changeSigThreshold)
{
    neighbourhoodChangeSignificanceThreshold = changeSigThreshold;
}

KLCommMgr::~KLCommMgr(void)
{

}

int KLCommMgr::determineChangeSignificance(list<KLNodeInfo*>& newNeighbourNodeList, double cTime)
{
    int commonNodeCount;
    int newlyAppearedNodeCount;
    double changeRatio;
    int changeSignificance;

    // find number of common nodes
    commonNodeCount = 0;
    list<KLNodeInfo*>::iterator iteratorNewListNodeInfo = newNeighbourNodeList.begin();
    while (iteratorNewListNodeInfo != newNeighbourNodeList.end()) {
        KLNodeInfo *newListNodeInfo = *iteratorNewListNodeInfo;

        list<KLNodeInfo*>::iterator iteratorOldListNodeInfo = neighbourNodeInfoList.begin();
        while (iteratorOldListNodeInfo != neighbourNodeInfoList.end()) {
            KLNodeInfo *oldListNodeInfo = *iteratorOldListNodeInfo;

            if (newListNodeInfo->getNodeAddress() == oldListNodeInfo->getNodeAddress()) {
                commonNodeCount++;
                break;
            }
            iteratorOldListNodeInfo++;
        }
        iteratorNewListNodeInfo++;

    }

    // how many new nodes have appeared?
    newlyAppearedNodeCount = (newNeighbourNodeList.size() > commonNodeCount ? newNeighbourNodeList.size() - commonNodeCount : 0);


    if (newlyAppearedNodeCount == 0) {

        // if no newly appeared nodes exist, then still the same neighbourhood (minus
        // the left nodes)
        changeSignificance = KLKEETCHI_INSIGNIFICANT_CHANGE;

    // } else if (newlyAppearedNodeCount > 0 && neighbourNodeInfoList.size() == 0) {
    //
    //     // if there are a newly appeared nodes and there were no nodes before,
    //     // that means a completely new neighbourhood
    //     changeSignificance = KLKEETCHI_SIGNIFICANT_CHANGE;
    //
    } else {

        // control comes here is there is a partil change
        // so, find the ratio of partial change
        changeRatio = ((double) newlyAppearedNodeCount) / newNeighbourNodeList.size();

        // changeSignificanceThreshold is between 0.0 and 1.0
        // e.g, if changeSignificanceThreshold = 0.25 and the change ratio is >= 0.25 (i.e., 25% or more)
        //      then consider that as a signicant change
        //      else consider that as an insignificant change
        if (changeRatio >= neighbourhoodChangeSignificanceThreshold) {

            changeSignificance = KLKEETCHI_SIGNIFICANT_CHANGE;

        } else {

            changeSignificance = KLKEETCHI_INSIGNIFICANT_CHANGE;

        }
    }

    return changeSignificance;
}

int KLCommMgr::updateNeighbours(list<KLNodeInfo*>& newNeighbourNodeList, double cTime)
{
    // remove old nodes
    while (!neighbourNodeInfoList.empty()) {
      KLNodeInfo *nodeInfo = neighbourNodeInfoList.front();
      neighbourNodeInfoList.pop_front();
      delete nodeInfo;
    }

    // add new nodes
    list<KLNodeInfo*>::iterator iteratorNewListNodeInfo = newNeighbourNodeList.begin();
    while (iteratorNewListNodeInfo != newNeighbourNodeList.end()) {
        KLNodeInfo *newListNodeInfo = (*iteratorNewListNodeInfo);
        KLNodeInfo *nodeInfo = new KLNodeInfo(newListNodeInfo->getNodeAddress());
        neighbourNodeInfoList.push_back(nodeInfo);
        iteratorNewListNodeInfo++;
    }

    return 0;
}

