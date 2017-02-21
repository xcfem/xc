//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC is free software: you can redistribute it and/or modify 
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

class_<XC::DeformationPlane, bases<Plano3d> >("DeformationPlane")
  .def(init<const double&>())
  .def(init<Plano3d>())
  .def(init<Pos2d,double,Pos2d,double,Pos2d,double>())
  .def(init<Pos3d,Pos3d,Pos3d>())
  .def("constantStrain",&XC::DeformationPlane::ConstantStrain)
.def("getDeformation",&XC::DeformationPlane::Strain,"DEPRECATED; returns strain at position.")
  .def("getStrain",&XC::DeformationPlane::Strain,"returns strain at position.")
  ;

class_<XC::CalcPivotes, bases<Ref3d3d>, boost::noncopyable >("CalcPivotes", no_init)
  ;

class_<XC::DefAgotPivotes, bases<EntCmd>, boost::noncopyable >("DefAgotPivotes", no_init)
  .add_property("defAgotPivoteA",make_function(&XC::DefAgotPivotes::getDefAgotPivoteA,return_value_policy<copy_const_reference>()),&XC::DefAgotPivotes::setDefAgotPivoteA)
  .add_property("defAgotPivoteB",make_function(&XC::DefAgotPivotes::getDefAgotPivoteB,return_value_policy<copy_const_reference>()),&XC::DefAgotPivotes::setDefAgotPivoteB)
  .add_property("defAgotPivoteC",make_function(&XC::DefAgotPivotes::getDefAgotPivoteC,return_value_policy<copy_const_reference>()),&XC::DefAgotPivotes::setDefAgotPivoteC)
  ;

class_<XC::InteractionDiagramData, bases<EntCmd>>("InteractionDiagramParameters")
  .add_property("umbral",make_function(&XC::InteractionDiagramData::getUmbral,return_value_policy<copy_const_reference>()),&XC::InteractionDiagramData::setUmbral)
  .add_property("incEps",make_function(&XC::InteractionDiagramData::getIncEps,return_value_policy<copy_const_reference>()),&XC::InteractionDiagramData::setIncEps)
  .add_property("incTheta",make_function(&XC::InteractionDiagramData::getIncTheta,return_value_policy<copy_const_reference>()),&XC::InteractionDiagramData::setIncTheta)
  .add_property("defsAgotPivotes",make_function(&XC::InteractionDiagramData::getDefsAgotPivotes,return_internal_reference<>()),&XC::InteractionDiagramData::setDefsAgotPivotes)
  .add_property("nmbSetHormigon",make_function(&XC::InteractionDiagramData::getNmbSetHormigon,return_internal_reference<>()),&XC::InteractionDiagramData::setNmbSetHormigon)
  .add_property("tagHormigon",make_function(&XC::InteractionDiagramData::getTagHormigon,return_value_policy<copy_const_reference>()),&XC::InteractionDiagramData::setTagHormigon)
  .add_property("nmbSetArmadura",make_function(&XC::InteractionDiagramData::getNmbSetArmadura,return_internal_reference<>()),&XC::InteractionDiagramData::setNmbSetArmadura)
  .add_property("tagArmadura",make_function(&XC::InteractionDiagramData::getTagArmadura,return_value_policy<copy_const_reference>()),&XC::InteractionDiagramData::setTagArmadura)
  ;

class_<XC::ClosedTriangleMesh, bases<GeomObj3d>, boost::noncopyable >("ClosedTriangleMesh", no_init)
  .def("getMax",&XC::ClosedTriangleMesh::GetMax)
  .def("getMin",&XC::ClosedTriangleMesh::GetMin)
  .def("getArea",&XC::ClosedTriangleMesh::Area)
  .def("getVolume",&XC::ClosedTriangleMesh::Volumen)
  .def("getIx",&XC::ClosedTriangleMesh::Ix)
  .def("getIy",&XC::ClosedTriangleMesh::Iy)
  .def("getIz",&XC::ClosedTriangleMesh::Iz)
  .def("getDimension",&XC::ClosedTriangleMesh::Dimension)
  .def("getNumFacetas",&XC::ClosedTriangleMesh::GetNumFacetas)
  .def("getTriangleMap",&XC::ClosedTriangleMesh::getTriangleMap)
  ;

double (XC::InteractionDiagram::*getFactorCapacidad)(const Pos3d &esf_d) const= &XC::InteractionDiagram::FactorCapacidad;
class_<XC::InteractionDiagram, bases<XC::ClosedTriangleMesh>, boost::noncopyable >("InteractionDiagram", no_init)
  .def("centroid",&XC::InteractionDiagram::Cdg)
  .def("getLength",&XC::InteractionDiagram::Longitud)
  .def("getIntersection",&XC::InteractionDiagram::getIntersection,"Returns the intersection of the ray O->point(N,My,Mz) with the interaction diagram.")
  .def("getCapacityFactor",getFactorCapacidad)
  .def("writeTo",&XC::InteractionDiagram::writeTo)
  .def("readFrom",&XC::InteractionDiagram::readFrom)
  ;

double (XC::InteractionDiagram2d::*getFactorCapacidad2d)(const Pos2d &esf_d) const= &XC::InteractionDiagram2d::FactorCapacidad;
class_<XC::InteractionDiagram2d, bases<Poligono2d>, boost::noncopyable >("InteractionDiagram2d", no_init)
  .def("getIntersection",&XC::InteractionDiagram2d::getIntersection,"Returns the intersection of the ray O->point(N,My,Mz) with the interaction diagram.")
  .def("getCapacityFactor",getFactorCapacidad2d)
  .def("simplify",&XC::InteractionDiagram2d::Simplify)
  ;
