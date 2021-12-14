//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.cc

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/docstring_options.hpp>
#include "GTSBBoxTree.h"
#include "GTSGraph.h"
#include "GTSSurfaceIntersection.h"
#include "TriangleMap.h"
#include "GTSEdge.h"
#include "GTSSplit.h"
#include "GTSSurfaceTraverse.h"
#include "GTSFace.h"
#include "GTSSurface.h"
#include "GTSVertex.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"


BOOST_PYTHON_MODULE(xcGnuGts)
  {
    using namespace boost::python;
    docstring_options doc_options;

    class_<GTSVertex>("GTSVertex")
      .def(init<double, double, double>())
      .def(init<Pos3d>())
      .add_property("getPos3d", &GTSVertex::Posicion)
      .def("getGaussianCurvature",&GTSVertex::GaussianCurvature)
      .def("getMeanCurvatureNormal",&GTSVertex::MeanCurvatureNormal)
      ;

    class_<GTSEdge>("GTSEdge", init<GTSVertex &,GTSVertex &>())
       .def("getV1",&GTSEdge::V1)
       .def("getV2",&GTSEdge::V2)
       .def("CollapseIsValid",&GTSEdge::CollapseIsValid)
     ;

    class_<GTSFace>("GTSFace", init<GTSEdge &,GTSEdge &,GTSEdge &>())
       .def("isNull",&GTSFace::Null)
     ;

    class_<GTSSurfaceTraverse>("GTSSurfaceTraverse", init<GTSSurface &,GTSFace &>())
      .def("next",&GTSSurfaceTraverse::Next)
     ;

    class_<GTSSurfaceIntersection>("GTSSurfaceIntersection")
      ;

    class_<GTSSurface>("GTSSurface")
      .def("appendFace",&GTSSurface::appendFace)
      .def("removeFace",&GTSSurface::removeFace);
     ;

    class_<GTSBBoxTree>("GTSBBoxTree")
      ;

    class_<GTSGraph>("GTSGraph", init<const GTSSurface &>())
      ;

    class_<GTSSplit>("GTSSplit", init<const GTSVertex &,const GTSVertex &,const GTSVertex &>())
      ;

    class_<VerticesKDTree>("VerticesKDTree")
      .def("getNearest",&VerticesKDTree::getNearest)
      .def("getNearest",&VerticesKDTree::getNearestBallPoint)
      ;

    typedef std::map<size_t,Pos3d> map_szt_pos3d;
    class_<map_szt_pos3d>("map_szt_pos3d")
      .def(map_indexing_suite<map_szt_pos3d >())
      ;

    class_<VerticesMap, bases<map_szt_pos3d> >("VerticesMap")
      ;

    const size_t &(TriangleVerticesIndexes::*v1)(void) const= &TriangleVerticesIndexes::V1;
    const size_t &(TriangleVerticesIndexes::*v2)(void) const= &TriangleVerticesIndexes::V2;
    const size_t &(TriangleVerticesIndexes::*v3)(void) const= &TriangleVerticesIndexes::V3;
    class_<TriangleVerticesIndexes>("TriangleVerticesIndexes", init<const size_t &,const size_t &,const size_t &>())
      .def("getV1",make_function(v1,return_value_policy<copy_const_reference>()),"Returns first vertex index.")
      .def("getV2",make_function(v2,return_value_policy<copy_const_reference>()),"Returns second vertex index.")
      .def("getV3",make_function(v3,return_value_policy<copy_const_reference>()),"Returns third vertex index.")
      ;

    typedef std::deque<TriangleVerticesIndexes> deque_triang;
    class_<deque_triang>("deque_triang")
      .def(vector_indexing_suite<deque_triang, false >())
      ;
    
    class_<TriangleFaces, bases<deque_triang> >("TriangleFaces")
      ;

    const VerticesMap &(TriangleMap::*GetVertices)(void) const= &TriangleMap::getVertices;
    const TriangleFaces &(TriangleMap::*GetFaces)(void) const= &TriangleMap::getFaces;
    class_<TriangleMap>("TriangleMap")
      .def("getVertices",make_function(GetVertices,return_internal_reference<>()),"Returns the vertices container.")
      .def("getFaces",make_function(GetFaces,return_internal_reference<>()),"Returns the faces container.")
      ;

  }
