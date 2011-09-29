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

#include <boost/test/unit_test.hpp>
#include <boost/cstdint.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <pdal/Dimension.hpp>

using namespace pdal;

BOOST_AUTO_TEST_SUITE(DimensionTest)

BOOST_AUTO_TEST_CASE(test_ctor)
{
    Dimension d1(DimensionId::X_i32);

    BOOST_CHECK(d1.getId() == DimensionId::X_i32);
    BOOST_CHECK(d1.getName() == "X");
    BOOST_CHECK(d1.getDataType() == Dimension::Int32);

    Dimension d2(d1);
    BOOST_CHECK(d1.getId() == DimensionId::X_i32);
    BOOST_CHECK(d1.getDataType() == Dimension::Int32);

    Dimension d3 = d1;
    BOOST_CHECK(d1.getId() == DimensionId::X_i32);
    BOOST_CHECK(d1.getDataType() == Dimension::Int32);

    BOOST_CHECK(d1 == d1);
    BOOST_CHECK(d1 == d2);
    BOOST_CHECK(d2 == d1);
    BOOST_CHECK(d1 == d3);
    BOOST_CHECK(d3 == d1);

    Dimension d4(DimensionId::Y_i32);
    d4.setEndianness(Endian_Big);
    BOOST_CHECK(d1 != d4);
    BOOST_CHECK(d4 != d1);
}


BOOST_AUTO_TEST_CASE(DimensionTest_ptree)
{
    Dimension d1(DimensionId::X_i32);

    std::stringstream ss1(std::stringstream::in | std::stringstream::out);
  
    boost::property_tree::ptree tree = d1.toPTree();
    boost::property_tree::write_xml(ss1, tree);

    std::string out1 = ss1.str();

    std::string xml_header = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    std::string ref = xml_header + 
        "<name>X</name><datatype>Int32</datatype>"
        "<description>x coordinate as a long integer. You must use the scale and offset information of the header to determine the double value.</description>"
        "<bytesize>4</bytesize><endianness>little</endianness><scale>0</scale><isValid>false</isValid>";

    boost::algorithm::erase_all(out1, "\n");
    boost::algorithm::erase_all(ref, "\n");
    BOOST_CHECK_EQUAL(ref, out1);

    return;
}


BOOST_AUTO_TEST_SUITE_END()

