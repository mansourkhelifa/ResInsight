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

#pragma once

#include "RigStatisticsCalculator.h"

#include "cvfBase.h"
#include "cvfObject.h"

#include <vector>
#include <cmath> // Needed for HUGE_VAL on Linux

//==================================================================================================
/// 
//==================================================================================================
class RigStatisticsDataCache : public cvf::Object
{
public:
    RigStatisticsDataCache(RigStatisticsCalculator* statisticsCalculator);

    void                                    clearAllStatistics();

    void                                    minMaxCellScalarValues(double& min, double& max);
    void                                    minMaxCellScalarValues(size_t timeStepIndex, double& min, double& max);

    void                                    posNegClosestToZero(double& pos, double& neg);
    void                                    posNegClosestToZero(size_t timeStepIndex, double& pos, double& neg);

    void                                    p10p90CellScalarValues(double& p10, double& p90);
    void                                    p10p90CellScalarValues(size_t timeStepIndex, double& p10, double& p90);

    void                                    meanCellScalarValues(double& meanValue);
    void                                    meanCellScalarValues(size_t timeStepIndex, double& meanValue);

    const std::vector<size_t>&              cellScalarValuesHistogram();
    const std::vector<size_t>&              cellScalarValuesHistogram(size_t timeStepIndex);

private:
    void                                    computeHistogramStatisticsIfNeeded();
    void                                    computeHistogramStatisticsIfNeeded(size_t timeStepIndex);

private:
    struct StatisticsValues
    {
        StatisticsValues()
        {
            m_minValue                  = HUGE_VAL;
            m_maxValue                  = -HUGE_VAL;
            m_isMaxMinCalculated        = false;
            m_meanValue                 = HUGE_VAL;
            m_isMeanCalculated          = false;
            m_posClosestToZero          = HUGE_VAL;
            m_negClosestToZero          = -HUGE_VAL;
            m_isClosestToZeroCalculated = false;
            m_p10                       = HUGE_VAL;
            m_p90                       = HUGE_VAL;
        }

        double              m_minValue;
        double              m_maxValue;
        bool                m_isMaxMinCalculated;
        
        double              m_meanValue;
        bool                m_isMeanCalculated;

        double              m_posClosestToZero;
        double              m_negClosestToZero;
        bool                m_isClosestToZeroCalculated;
        
        double              m_p10;
        double              m_p90;

        std::vector<size_t> m_histogram;
    };

    StatisticsValues                        m_statsAllTimesteps;
    std::vector<StatisticsValues>           m_statsPrTs;

    cvf::ref<RigStatisticsCalculator>       m_statisticsCalculator;
};
