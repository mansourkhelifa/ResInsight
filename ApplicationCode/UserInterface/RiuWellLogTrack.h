/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2015-     Statoil ASA
//  Copyright (C) 2015-     Ceetron Solutions AS
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

#include "qwt_plot.h"

#include "cafPdmPointer.h"

class RimWellLogTrack;

class QwtLegend;
class QwtPicker;
class QwtPlotGrid;
class QwtPlotMarker;

class QEvent;

//==================================================================================================
//
//
//
//==================================================================================================
class RiuWellLogTrack : public QwtPlot
{
    Q_OBJECT

public:
    RiuWellLogTrack(RimWellLogTrack* plotTrackDefinition, QWidget* parent = NULL);
    virtual ~RiuWellLogTrack();

    void                                    setDepthZoom(double minDepth, double maxDepth);
    void                                    setDepthTitle(const QString& title);

    void                                    setXRange(double min, double max);

    bool                                    isRimTrackVisible();

protected:
    virtual bool                            eventFilter(QObject* watched, QEvent* event);
    virtual void                            focusInEvent(QFocusEvent* event);
    virtual QSize                           sizeHint() const;
    virtual QSize                           minimumSizeHint() const;
    virtual void                            leaveEvent(QEvent *) override;

private:
    friend class RiuWellLogTrackQwtPicker;
    QPointF                                 closestCurvePoint(const QPoint& pos, QString* valueString, QString* depthString) const;
    void                                    updateClosestCurvePointMarker(const QPointF& pos);

    void                                    setDefaults();
    void                                    selectClosestCurve(const QPoint& pos);

private:
    caf::PdmPointer<RimWellLogTrack>        m_plotTrackDefinition;
    QwtPlotGrid*                            m_grid;
    QwtPicker*                              m_plotPicker;
    QwtPlotMarker*                          m_plotMarker;
};
