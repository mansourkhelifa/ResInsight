/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Statoil ASA
//  Copyright (C) Ceetron Solutions AS
// 
//  ResInsight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  ResInsight is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.
// 
//  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
//  for more details.
//
/////////////////////////////////////////////////////////////////////////////////

#include "RigWellLogExtractor.h"
#include "RigWellPath.h"
#include "cvfTrace.h"

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RigWellLogExtractor::RigWellLogExtractor(const RigWellPath* wellpath) : m_wellPath(wellpath)
{

}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RigWellLogExtractor::~RigWellLogExtractor()
{

}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RigWellLogExtractor::populateReturnArrays(std::map<RigMDCellIdxEnterLeaveIntersectionSorterKey, HexIntersectionInfo > &uniqueIntersections)
{
    {
        // For same MD and same cell, remove enter/leave pairs, as they only touches the wellpath, and should not contribute.

        std::map<RigMDCellIdxEnterLeaveIntersectionSorterKey, HexIntersectionInfo >::iterator it1 = uniqueIntersections.begin();
        std::map<RigMDCellIdxEnterLeaveIntersectionSorterKey, HexIntersectionInfo >::iterator it2 = uniqueIntersections.begin();

        std::vector<std::map<RigMDCellIdxEnterLeaveIntersectionSorterKey, HexIntersectionInfo >::iterator> iteratorsToIntersectonsToErase;

        while (it2 != uniqueIntersections.end())
        {
            ++it2;
            if (it2 != uniqueIntersections.end())
            {
                if (RigHexIntersector::isEqualDepth(it1->first.measuredDepth, it2->first.measuredDepth))
                {
                    if (it1->first.hexIndex == it2->first.hexIndex)
                    {
                        // Remove the two from the map, as they just are a touch of the cell surface
                        CVF_TIGHT_ASSERT(!it1->first.isEnteringCell && it2->first.isEnteringCell);

                        iteratorsToIntersectonsToErase.push_back(it1);
                        iteratorsToIntersectonsToErase.push_back(it2);
                    }
                }
            }
            ++it1;
        }

        // Erase all the intersections that is not needed
        for (size_t erItIdx = 0; erItIdx < iteratorsToIntersectonsToErase.size(); ++erItIdx)
        {
            uniqueIntersections.erase(iteratorsToIntersectonsToErase[erItIdx]);
        }
    }

    // Copy the map into a different sorting regime, with enter leave more significant than cell index
    std::map<RigMDEnterLeaveCellIdxIntersectionSorterKey, HexIntersectionInfo > sortedUniqueIntersections;
    {
        std::map<RigMDCellIdxEnterLeaveIntersectionSorterKey, HexIntersectionInfo >::iterator it = uniqueIntersections.begin();
        while (it != uniqueIntersections.end())
        {
            sortedUniqueIntersections.insert(std::make_pair(RigMDEnterLeaveCellIdxIntersectionSorterKey(it->first.measuredDepth, it->first.isEnteringCell, it->first.hexIndex),
                it->second));
            ++it;
        }
    }


    // Make sure we have sensible pairs of intersections. One pair for each in/out of a cell
    {
        // Add an intersection for the well startpoint that is inside the first cell
        std::map<RigMDEnterLeaveCellIdxIntersectionSorterKey, HexIntersectionInfo >::iterator it = sortedUniqueIntersections.begin();
        if (it != sortedUniqueIntersections.end() && !it->first.isEnteringCell) // Leaving a cell as first intersection. Well starts inside a cell.
        {
            // Needs wellpath start point in front
            HexIntersectionInfo firstLeavingPoint = it->second;
            firstLeavingPoint.m_intersectionPoint =  m_wellPath->m_wellPathPoints[0];

            sortedUniqueIntersections.insert(std::make_pair(RigMDEnterLeaveCellIdxIntersectionSorterKey(m_wellPath->m_measuredDepths[0], firstLeavingPoint.m_hexIndex, true),
                firstLeavingPoint));
        }

        // Add an intersection for the well endpoint possibly inside the last cell.
        std::map<RigMDEnterLeaveCellIdxIntersectionSorterKey, HexIntersectionInfo >::reverse_iterator rit = sortedUniqueIntersections.rbegin();
        if (rit != sortedUniqueIntersections.rend() && rit->first.isEnteringCell) // Entering a cell as last intersection. Well ends inside a cell.
        {
            // Needs wellpath end point at end
            HexIntersectionInfo lastEnterPoint = rit->second;
            lastEnterPoint.m_intersectionPoint =  m_wellPath->m_wellPathPoints.back();

            sortedUniqueIntersections.insert(std::make_pair(RigMDEnterLeaveCellIdxIntersectionSorterKey(m_wellPath->m_measuredDepths.back(), lastEnterPoint.m_hexIndex, false),
                lastEnterPoint));
        }
    }

    std::map<RigMDEnterLeaveIntersectionSorterKey, HexIntersectionInfo > filteredSortedIntersections;

    {
        std::map<RigMDEnterLeaveCellIdxIntersectionSorterKey, HexIntersectionInfo >::iterator it1 = sortedUniqueIntersections.begin();
        std::map<RigMDEnterLeaveCellIdxIntersectionSorterKey, HexIntersectionInfo >::iterator it2;

        while (it1 != sortedUniqueIntersections.end())
        {
            it2 = it1;
            ++it2;

            if (it2 == sortedUniqueIntersections.end()) break;

            // If we have a proper pair, insert in the filtered list and continue
            if (!RigMDEnterLeaveCellIdxIntersectionSorterKey::isProperPair(it1->first, it2->first))
            {
                //CVF_ASSERT(false);
                cvf::Trace::show(cvf::String("Well log curve is inaccurate around MD:  ") + cvf::String::number((double)(it1->first.measuredDepth)) + ", " + cvf::String::number((double)(it2->first.measuredDepth)));
            }
            {
                filteredSortedIntersections.insert(std::make_pair(RigMDEnterLeaveIntersectionSorterKey(it1->first.measuredDepth, it1->first.isEnteringCell),
                    it1->second));
                ++it1;
                filteredSortedIntersections.insert(std::make_pair(RigMDEnterLeaveIntersectionSorterKey(it1->first.measuredDepth, it1->first.isEnteringCell),
                    it1->second));
                ++it1;
            }
        }
    }

    {
        // Now populate the return arrays
        std::map<RigMDEnterLeaveIntersectionSorterKey, HexIntersectionInfo >::iterator it;

        it = filteredSortedIntersections.begin();
        while (it != filteredSortedIntersections.end())
        {
            m_measuredDepth.push_back(it->first.measuredDepth);
            m_trueVerticalDepth.push_back(abs(it->second.m_intersectionPoint[2]));
            m_intersections.push_back(it->second.m_intersectionPoint);
            m_intersectedCells.push_back(it->second.m_hexIndex);
            m_intersectedCellFaces.push_back(it->second.m_face);
            ++it;
        }
    }
}
