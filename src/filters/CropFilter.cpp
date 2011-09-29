/******************************************************************************
* Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#include <pdal/filters/CropFilter.hpp>

#include <pdal/filters/CropFilterIterator.hpp>
#include <pdal/SchemaLayout.hpp>
#include <pdal/PointBuffer.hpp>
#include <sstream>

namespace pdal { namespace filters {


CropFilter::CropFilter(Stage& prevStage, const Options& options)
    : pdal::Filter(prevStage, options)
    , m_bounds(options.getValueOrThrow<Bounds<double> >("bounds"))
{
    return;
}


CropFilter::CropFilter(Stage& prevStage, Bounds<double> const& bounds)
    : Filter(prevStage, Options::none())
    , m_bounds(bounds)
{
    return;
}


void CropFilter::initialize()
{
    Filter::initialize();

    this->setBounds(m_bounds);

    this->setNumPoints(0);
    this->setPointCountType(PointCount_Unknown);

    return;
}


const Options CropFilter::getDefaultOptions() const
{
    Options options;
    Option bounds("bounds",Bounds<double>(),"bounds to crop to");
    options.add(bounds);
    return options;
}


const Bounds<double>& CropFilter::getBounds() const
{
    return m_bounds;
}


// append all points from src buffer to end of dst buffer, based on the our bounds
boost::uint32_t CropFilter::processBuffer(PointBuffer& dstData, const PointBuffer& srcData) const
{
    const SchemaLayout& schemaLayout = dstData.getSchemaLayout();

    bool isDouble = schemaLayout.getSchema().hasDimension(DimensionId::X_f64);
    assert(isDouble || (!isDouble && schemaLayout.getSchema().hasDimension(DimensionId::X_i32)));

    const Bounds<double>& bounds = this->getBounds();

    boost::uint32_t numSrcPoints = srcData.getNumPoints();
    boost::uint32_t dstIndex = dstData.getNumPoints();

    boost::uint32_t numPointsAdded = 0;

    if (isDouble)
    {
        const int fieldX = schemaLayout.getDimensionIndex(DimensionId::X_f64);
        const int fieldY = schemaLayout.getDimensionIndex(DimensionId::Y_f64);
        const int fieldZ = schemaLayout.getDimensionIndex(DimensionId::Z_f64);

        for (boost::uint32_t srcIndex=0; srcIndex<numSrcPoints; srcIndex++)
        {
            const double x = srcData.getField<double>(srcIndex, fieldX);
            const double y = srcData.getField<double>(srcIndex, fieldY);
            const double z = srcData.getField<double>(srcIndex, fieldZ);

            Vector<double> point(x,y,z);
        
            if (bounds.contains(point))
            {
                dstData.copyPointFast(dstIndex, srcIndex, srcData);
                dstData.setNumPoints(dstIndex+1);
                ++dstIndex;
                ++numPointsAdded;
            }
        }
    }
    else
    {
        const int fieldX = schemaLayout.getDimensionIndex(DimensionId::X_i32);
        const int fieldY = schemaLayout.getDimensionIndex(DimensionId::Y_i32);
        const int fieldZ = schemaLayout.getDimensionIndex(DimensionId::Z_i32);

        const Dimension& xdim = schemaLayout.getSchema().getDimension(DimensionId::X_i32);
        const Dimension& ydim = schemaLayout.getSchema().getDimension(DimensionId::X_i32);
        const Dimension& zdim = schemaLayout.getSchema().getDimension(DimensionId::X_i32);

        for (boost::uint32_t srcIndex=0; srcIndex<numSrcPoints; srcIndex++)
        {
            // need to scale the values
            boost::int32_t xi = srcData.getField<boost::int32_t>(srcIndex, fieldX);
            boost::int32_t yi = srcData.getField<boost::int32_t>(srcIndex, fieldY);
            boost::int32_t zi = srcData.getField<boost::int32_t>(srcIndex, fieldZ);

            const double x = xdim.applyScaling(xi);
            const double y = ydim.applyScaling(yi);
            const double z = zdim.applyScaling(zi);
        
            Vector<double> point(x,y,z);
        
            if (bounds.contains(point))
            {
                dstData.copyPointFast(dstIndex, srcIndex, srcData);
                dstData.setNumPoints(dstIndex+1);
                ++dstIndex;
                ++numPointsAdded;
            }
        }
    }

    assert(dstIndex <= dstData.getCapacity());

    return numPointsAdded;
}


pdal::StageSequentialIterator* CropFilter::createSequentialIterator() const
{
    return new CropFilterSequentialIterator(*this);
}


} } // namespaces
