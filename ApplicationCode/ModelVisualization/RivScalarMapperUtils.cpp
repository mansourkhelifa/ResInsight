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

#include "RivScalarMapperUtils.h"

#include "RimCellEdgeColors.h"
#include "RimLegendConfig.h"
#include "RimEclipseView.h"
#include "RimEclipseCellColors.h"
#include "RimTernaryLegendConfig.h"

#include "RivCellEdgeEffectGenerator.h"
#include "RivCellEdgeGeometryUtils.h"
#include "RivTernaryScalarMapper.h"
#include "RivTernaryScalarMapperEffectGenerator.h"

#include "cafEffectGenerator.h"

#include "cvfDrawableGeo.h"
#include "cvfPart.h"


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RivScalarMapperUtils::applyTextureResultsToPart(cvf::Part* part, cvf::Vec2fArray* textureCoords, const cvf::ScalarMapper* mapper, float opacityLevel, caf::FaceCulling faceCulling, bool disableLighting)
{
    CVF_ASSERT(part && textureCoords && mapper);

    cvf::DrawableGeo* dg = dynamic_cast<cvf::DrawableGeo*>(part->drawable());
    if (dg) dg->setTextureCoordArray(textureCoords);

    cvf::ref<cvf::Effect> scalarEffect = RivScalarMapperUtils::createScalarMapperEffect(mapper, opacityLevel, faceCulling, disableLighting);
    part->setEffect(scalarEffect.p());
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RivScalarMapperUtils::applyTernaryTextureResultsToPart(cvf::Part* part, cvf::Vec2fArray* textureCoords, const RivTernaryScalarMapper* mapper, float opacityLevel, caf::FaceCulling faceCulling, bool disableLighting)
{
    CVF_ASSERT(part && textureCoords && mapper);

    cvf::DrawableGeo* dg = dynamic_cast<cvf::DrawableGeo*>(part->drawable());
    if (dg) dg->setTextureCoordArray(textureCoords);

    cvf::ref<cvf::Effect> scalarEffect = RivScalarMapperUtils::createTernaryScalarMapperEffect(mapper, opacityLevel, faceCulling, disableLighting);
    part->setEffect(scalarEffect.p());
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
cvf::ref<cvf::Effect> RivScalarMapperUtils::createCellEdgeEffect(cvf::DrawableGeo* dg,
    const cvf::StructGridQuadToCellFaceMapper* quadToCellFaceMapper,
    size_t gridIndex,
    size_t timeStepIndex,
    RimEclipseCellColors* cellResultColors,
    RimCellEdgeColors* cellEdgeResultColors,
    float opacityLevel,
    cvf::Color3f defaultColor,
    caf::FaceCulling faceCulling,
    bool disableLighting)
{
    CellEdgeEffectGenerator cellFaceEffectGen(cellEdgeResultColors->legendConfig()->scalarMapper());

    if (cellResultColors->isTernarySaturationSelected())
    {
        RivCellEdgeGeometryUtils::addTernaryCellEdgeResultsToDrawableGeo(timeStepIndex, cellResultColors, cellEdgeResultColors,
            quadToCellFaceMapper, dg, gridIndex, opacityLevel);

        RivTernaryScalarMapper* ternaryCellScalarMapper = cellResultColors->ternaryLegendConfig()->scalarMapper();
        cellFaceEffectGen.setTernaryScalarMapper(ternaryCellScalarMapper);
    }
    else
    {
        RivCellEdgeGeometryUtils::addCellEdgeResultsToDrawableGeo(timeStepIndex, cellResultColors, cellEdgeResultColors,
            quadToCellFaceMapper, dg, gridIndex, opacityLevel);

        cvf::ScalarMapper* cellScalarMapper = cellResultColors->legendConfig()->scalarMapper();
        cellFaceEffectGen.setScalarMapper(cellScalarMapper);
    }

    cellFaceEffectGen.setOpacityLevel(opacityLevel);
    cellFaceEffectGen.setDefaultCellColor(defaultColor);
    cellFaceEffectGen.setFaceCulling(faceCulling);
    cellFaceEffectGen.disableLighting(disableLighting);

    cvf::ref<cvf::Effect> eff = cellFaceEffectGen.generateCachedEffect();
    return eff;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
cvf::ref<cvf::Effect> RivScalarMapperUtils::createScalarMapperEffect(const cvf::ScalarMapper* mapper, float opacityLevel, caf::FaceCulling faceCulling, bool disableLighting)
{
    CVF_ASSERT(mapper);

    caf::PolygonOffset polygonOffset = caf::PO_1;
    caf::ScalarMapperEffectGenerator scalarEffgen(mapper, polygonOffset);
    scalarEffgen.setOpacityLevel(opacityLevel);
    scalarEffgen.setFaceCulling(faceCulling);
    scalarEffgen.disableLighting(disableLighting);

    cvf::ref<cvf::Effect> scalarEffect = scalarEffgen.generateCachedEffect();

    return scalarEffect;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
cvf::ref<cvf::Effect> RivScalarMapperUtils::createTernaryScalarMapperEffect(const RivTernaryScalarMapper* mapper, float opacityLevel, caf::FaceCulling faceCulling, bool disableLighting)
{
    CVF_ASSERT(mapper);

    caf::PolygonOffset polygonOffset = caf::PO_1;
    RivTernaryScalarMapperEffectGenerator scalarEffgen(mapper, polygonOffset);
    scalarEffgen.setOpacityLevel(opacityLevel);
    scalarEffgen.setFaceCulling(faceCulling);
    scalarEffgen.disableLighting(disableLighting);
    cvf::ref<cvf::Effect> scalarEffect = scalarEffgen.generateCachedEffect();

    return scalarEffect;
}
