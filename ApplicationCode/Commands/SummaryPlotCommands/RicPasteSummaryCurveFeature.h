/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2016-     Statoil ASA
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

#include "cafCmdFeature.h"
#include "cafPdmPointer.h"

#include <vector>

class RimSummaryCurve;
class RimSummaryCurveFilter;

//==================================================================================================
/// 
//==================================================================================================
class RicPasteSummaryCurveFeature : public caf::CmdFeature
{
    CAF_CMD_HEADER_INIT;

protected:
    // Overrides
    virtual bool isCommandEnabled() override;
    virtual void onActionTriggered( bool isChecked ) override;
    virtual void setupActionLook(QAction* actionToSetup) override;

private:
    static std::vector<caf::PdmPointer<RimSummaryCurve> > summaryCurves();
};


//==================================================================================================
/// 
//==================================================================================================
class RicPasteSummaryCurveFilterFeature : public caf::CmdFeature
{
    CAF_CMD_HEADER_INIT;

protected:
    // Overrides
    virtual bool isCommandEnabled() override;
    virtual void onActionTriggered(bool isChecked) override;
    virtual void setupActionLook(QAction* actionToSetup) override;

private:
    static std::vector<caf::PdmPointer<RimSummaryCurveFilter> > summaryCurveFilters();
};