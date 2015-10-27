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
//python_interface_lines.tcc

XC::Node *(XC::Edge::*getFirstNodePtr)(void)= &XC::Edge::GetPrimerNodo;
XC::Node *(XC::Edge::*getLastNodePtr)(void)= &XC::Edge::GetUltimoNodo;
class_<XC::Edge, bases<XC::EntMdlr>, boost::noncopyable >("Edge", no_init)
  .add_property("nDiv", &XC::Edge::NDiv, &XC::Edge::SetNDiv)
  .add_property("getNumVertices", &XC::Edge::NumVertices)
  .add_property("getIdxVertices",&XC::Edge::getIndicesVertices)
  .add_property("firstNode",make_function(getFirstNodePtr, return_internal_reference<>()))
  .add_property("lastNode",make_function(getLastNodePtr, return_internal_reference<>()))
  .def("getLong", &XC::Edge::getLongitud,"Returns the length of the edge.")
  .def("getTang", &XC::Edge::getTang, return_internal_reference<>(),"Returns a vector tangent to the edge.")
  .def("divide",&XC::Edge::divide,"Creates points along the line.")
  .def("getKPoints",&XC::Edge::getKPoints,"Return endpoints of the line.")
  .def("getTagsNodes",&XC::Edge::GetTagsNodosDir,"Return node tags in direct order.")
  .def("getTagsNodesReverse",&XC::Edge::GetTagsNodosInv,"Return node tags in reverse order.")
   ;

class_<XC::LineaBase, bases<XC::Edge>, boost::noncopyable >("LineaBase", no_init)
   ;

class_<XC::Linea, bases<XC::LineaBase>, boost::noncopyable >("Line", no_init)
  .def("splitAtPoint", make_function(&XC::Linea::splitAtPoint, return_internal_reference<>()),"Breaks the line at the point.")
  .def("splitAtLambda", make_function(&XC::Linea::splitAtLambda, return_internal_reference<>()),"Breaks the line at the point defined by the paramenter (0.0->L).")
  .def("splitAtNaturalCoordinate", make_function(&XC::Linea::splitAtCooNatural, return_internal_reference<>()),"Breaks the line at the point defined by the natural coordinate (0.0->1.0).")
   ;

class_<XC::LineaTramos, bases<XC::Linea>, boost::noncopyable >("DividedLine", no_init)
  .def("setLongs",&XC::LineaTramos::setLongs,"Asigns length for each division.")
  ;

class_<XC::ArcoCircunf, bases<XC::LineaBase>, boost::noncopyable >("CircleArc", no_init)
  .def("getAngle", &XC::ArcoCircunf::getAnguloComprendido,"Returns the angle of the arc.")
  .def("getTheta1",&XC::ArcoCircunf::getTheta1,"Returns initial angle.")
  .def("getTheta2",&XC::ArcoCircunf::getTheta2,"Returns final angle.")
  .def("getCentro",&XC::ArcoCircunf::getCentro,"Returns center.")
  .def("getPInic",&XC::ArcoCircunf::getPInic,"Returns initial point.")
  .def("getPFin",&XC::ArcoCircunf::getPFin,"Returns final point.")
  .def("getPMed",&XC::ArcoCircunf::getPMed,"Returns mid point.")
  .def("getRadio",&XC::ArcoCircunf::getRadio,"Returns radius.")
   ;

XC::Edge *(XC::CmbEdge::Lado::*getEdge)(void)= &XC::CmbEdge::Lado::Borde;
class_<XC::CmbEdge::Lado,bases<EntCmd> >("Lado", no_init)
  .add_property("isDirect", &XC::CmbEdge::Lado::esDirecto)
  .add_property("getEdge", make_function(getEdge, return_internal_reference<>()))
  .def("getLong", &XC::CmbEdge::Lado::getLongitud,"Returns side's length.")
  .def("getTang", &XC::CmbEdge::Lado::getTang, return_internal_reference<>(),"Returns a vector tangent to the edge.")
  ;

typedef std::deque<XC::CmbEdge::Lado> dq_lados;
class_<dq_lados, boost::noncopyable >("DqEdges", no_init)
  .def("__iter__", boost::python::iterator<dq_lados>())
  .add_property("size", &dq_lados::size)
   ;


class_<XC::CmbEdge, bases<XC::Edge>, boost::noncopyable >("CmbEdge", no_init)
  .add_property("getNumVertices", &XC::CmbEdge::NumVertices)
  .add_property("getNumEdges", &XC::CmbEdge::NumEdges)
  .add_property("getEdges", make_function( &XC::CmbEdge::getLados, return_internal_reference<>()))
  .def("addLines",&XC::CmbEdge::addLines, return_internal_reference<>(),"Add lines to the sequence.")
  .def("addPoints",&XC::CmbEdge::addPoints, return_internal_reference<>(),"Add points to the sequence.")
   ;
