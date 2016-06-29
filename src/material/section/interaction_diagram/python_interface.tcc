//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

class_<XC::PlanoDeformacion, bases<Plano3d> >("PlanoDeformacion")
  .def(init<const double&>())
  .def(init<Plano3d>())
  .def(init<Pos2d,double,Pos2d,double,Pos2d,double>())
  .def(init<Pos3d,Pos3d,Pos3d>())
  .def("constantStrain",&XC::PlanoDeformacion::ConstantStrain)
  .def("getDeformation",&XC::PlanoDeformacion::Deformacion)
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
  .def("getCapacityFactor",getFactorCapacidad)
  .def("writeTo",&XC::InteractionDiagram::writeTo)
  .def("readFrom",&XC::InteractionDiagram::readFrom)
  ;

double (XC::InteractionDiagram2d::*getFactorCapacidad2d)(const Pos2d &esf_d) const= &XC::InteractionDiagram2d::FactorCapacidad;
class_<XC::InteractionDiagram2d, bases<Poligono2d>, boost::noncopyable >("InteractionDiagram2d", no_init)
  .def("getCapacityFactor",getFactorCapacidad2d)
  .def("simplify",&XC::InteractionDiagram2d::Simplify)
  ;
