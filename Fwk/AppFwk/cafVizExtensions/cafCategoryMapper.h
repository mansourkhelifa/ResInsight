
#pragma once

#include "cvfBase.h"
#include "cvfObject.h"
#include "cvfScalarMapper.h"
#include "cvfString.h"

namespace caf {


//==================================================================================================
//
//
//==================================================================================================
class CategoryMapper : public cvf::ScalarMapper
{
public:
    CategoryMapper();

    void                setCategories(const std::vector<int>& categoryValues);
    void                setCategoriesWithNames(const std::vector<int>& categoryValues, const std::vector<cvf::String>& categoryNames);
    
    // Colors in color array are cycled, if category count is larger than color count, colors are reused
    void                setCycleColors(const cvf::Color3ubArray& colorArray);

    // Colors are interpolated to make sure all categories get a unique color
    void                setInterpolateColors(const cvf::Color3ubArray& colorArray);

    
    // Overrides used from legend

    virtual cvf::Vec2f  mapToTextureCoord(double scalarValue) const;
    virtual bool        updateTexture(cvf::TextureImage* image) const;

    virtual cvf::Color3ub   mapToColor(double normalizedValue) const;

    virtual void        majorTickValues(std::vector<double>* domainValues) const;
    virtual double      normalizedValue(double domainValue) const;
    virtual double      domainValue(double normalizedValue) const;

private:
    friend class CategoryLegend;
    size_t              categoryCount() const;
    const cvf::String   textForCategoryIndex(size_t index) const;

    int                 categoryIndexForCategory(double domainValue) const;
    void                recomputeMaxTexCoord();

private:
    cvf::Color3ubArray  m_colors;
    cvf::uint           m_textureSize;      // The size of texture that updateTexture() is will produce. 
    double              m_maxTexCoord;      // The largest allowable s texture coordinate, scalar values >= m_rangeMax will get mapped to this coordinate

    std::vector<int>         m_categoryValues;
    std::vector<cvf::String> m_categoryNames;
};

}
