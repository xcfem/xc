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
//export_modelador_build_model.cxx

#include "python_interface.h"

void export_modelador_build_model(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

class_<XC::EntMdlrBase, bases<EntConNmb>, boost::noncopyable >("EntMdlrBase", no_init)
  .add_property("tag", &XC::EntMdlrBase::GetTag)
   ;

bool (XC::SetBase::*isNodeIn)(const XC::Node *) const= &XC::SetBase::In;
bool (XC::SetBase::*isElementIn)(const XC::Element *) const= &XC::SetBase::In;
bool (XC::SetBase::*isPntIn)(const XC::Pnt *) const= &XC::SetBase::In;
bool (XC::SetBase::*isEdgeIn)(const XC::Edge *) const= &XC::SetBase::In;
bool (XC::SetBase::*isFaceIn)(const XC::Face *) const= &XC::SetBase::In;
bool (XC::SetBase::*isBodyIn)(const XC::Body *) const= &XC::SetBase::In;
bool (XC::SetBase::*isUniformGridIn)(const XC::UniformGrid *) const= &XC::SetBase::In;
class_<XC::SetBase, bases<XC::EntMdlrBase>, boost::noncopyable >("SetBase", no_init)
  .def("genMesh", &XC::SetBase::Malla,"Triggers mesh generation.")
  .def("getNodeTags",&XC::SetBase::getNodeTags,"return set of node tags.")
  .def("getElementTags",&XC::SetBase::getElementTags,"return set of node tags.")
  .def("resetTributarias",&XC::SetBase::resetTributarias)
  .def("calculaLongsTributarias",&XC::SetBase::calculaLongsTributarias)
  .def("calculaAreasTributarias",&XC::SetBase::calculaAreasTributarias)
  .def("calculaVolsTributarios",&XC::SetBase::calculaVolsTributarios)
  .def("isNodeIn",isNodeIn,"True if sets contains node.")
  .def("isElementIn",isElementIn,"True if sets contains element.")
  .def("isPntIn",isPntIn,"True if sets contains point.")
  .def("isEdgeIn",isEdgeIn,"True if sets contains edge.")
  .def("isFaceIn",isFaceIn,"True if sets contains face.")
  .def("isBodyIn",isBodyIn,"True if sets contains body.")
  .def("isUniformGridIn",isUniformGridIn,"True if sets contains uniform grid.")
  .add_property("getNumNodes",&XC::SetBase::NumNodos)
  .add_property("getNumElements",&XC::SetBase::NumElementos)
   ;

class_<XC::SetEstruct, bases<XC::SetBase>, boost::noncopyable >("SetEstruct", no_init)
  .add_property("getNumNodes", &XC::SetEstruct::NumNodos)
  .add_property("getNumElements", &XC::SetEstruct::NumElementos)
  .add_property("isIRow", &XC::SetEstruct::EsFilaI)
  .add_property("isJRow", &XC::SetEstruct::EsFilaJ)
  .add_property("isKRow", &XC::SetEstruct::EsFilaK)
  .add_property("getDimension", &XC::SetEstruct::Dimension)
  .add_property("getStrType", &XC::SetEstruct::GetStrTipo)
  .def("getNodes", &XC::SetEstruct::getNodes, "Returns entitie's nodes.")
  .def("getElements", &XC::SetEstruct::getElements, "Returns entitie's elements.")
  .def("getNodeI", &XC::SetEstruct::getNodeI, return_internal_reference<>(), "Returns node in position (i)." )
  .def("getNodeIJ", &XC::SetEstruct::getNodeIJ, return_internal_reference<>(), "Returns node in position (i,j)." )
  .def("getNodeIJK", &XC::SetEstruct::getNodeIJK, return_internal_reference<>(), "Returns node in position (i,j,k).")
  .def("getElementI", &XC::SetEstruct::getElementI, return_internal_reference<>(), "Returns element in position (i)." )
  .def("getElementIJ", &XC::SetEstruct::getElementIJ, return_internal_reference<>(), "Returns element in position (i,j)." )
  .def("getElementIJK", &XC::SetEstruct::getElementIJK, return_internal_reference<>(), "Returns element in position (i,j,k)." )
   ;

#include "modelador/cad/python_interface.tcc"

  }

