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
//export_preprocessor_main.cxx

#include "python_interface.h"

void export_preprocessor_main(void)
  {
    using namespace boost::python;
    docstring_options doc_options;


class_<XC::MapSet, bases<EntCmd>, boost::noncopyable >("MapSet", no_init)
  .def("__getitem__",&XC::MapSet::getSet, return_internal_reference<>())
  .def("getSet", &XC::MapSet::getSet, return_internal_reference<>(),"Returns set by name.")
  .def("defSet", &XC::MapSet::defSet, return_internal_reference<>(),"Creates a new set with the name which is passed as a parameter.")
  .def("removeSet", &XC::MapSet::removeSet,"Deletes the set and removes it from the sets map.")

   ;


enum_<XC::dir_mallado>("meshDir")
  .value("I", XC::dirm_i)
  .value("J", XC::dirm_j)
  .value("K", XC::dirm_k)
   ;

XC::NodeLoader &(XC::Preprocessor::*getNodeLoaderRef)(void)= &XC::Preprocessor::getNodeLoader;
XC::MaterialLoader &(XC::Preprocessor::*getMaterialLoaderRef)(void)= &XC::Preprocessor::getMaterialLoader;
XC::BeamIntegratorLoader &(XC::Preprocessor::*getBeamIntegratorLoaderRef)(void)= &XC::Preprocessor::getBeamIntegratorLoader;
XC::ElementLoader &(XC::Preprocessor::*getElementLoaderRef)(void)= &XC::Preprocessor::getElementLoader;
XC::ConstraintLoader &(XC::Preprocessor::*getConstraintLoaderRef)(void)= &XC::Preprocessor::getConstraintLoader;
XC::LoadLoader &(XC::Preprocessor::*getLoadLoaderRef)(void)= &XC::Preprocessor::getLoadLoader;
XC::TransfCooLoader &(XC::Preprocessor::*getTransfCooLoaderRef)(void)= &XC::Preprocessor::getTransfCooLoader;
XC::Cad &(XC::Preprocessor::*getCadRef)(void)= &XC::Preprocessor::getCad;
XC::MapSet &(XC::Preprocessor::*getSetsRef)(void)= &XC::Preprocessor::get_sets;
XC::Domain *(XC::Preprocessor::*getDomainRf)(void)= &XC::Preprocessor::GetDominio;
class_<XC::Preprocessor, bases<EntCmd>, boost::noncopyable >("Preprocessor", no_init)
  .add_property("getNodeLoader", make_function( getNodeLoaderRef, return_internal_reference<>() ))
  .add_property("getMaterialLoader", make_function( getMaterialLoaderRef, return_internal_reference<>() ))
  .add_property("getBeamIntegratorLoader", make_function( getBeamIntegratorLoaderRef, return_internal_reference<>() ))
  .add_property("getElementLoader", make_function( getElementLoaderRef, return_internal_reference<>() ))
  .add_property("getConstraintLoader", make_function( getConstraintLoaderRef, return_internal_reference<>() ))
  .add_property("getLoadLoader", make_function( getLoadLoaderRef, return_internal_reference<>() ))
  .add_property("getTransfCooLoader", make_function( getTransfCooLoaderRef, return_internal_reference<>() ))
  .add_property("getCad", make_function( getCadRef, return_internal_reference<>() ))
  .add_property("getSets", make_function( getSetsRef, return_internal_reference<>() ))
  .add_property("getDomain", make_function( getDomainRf, return_internal_reference<>() ))
  .def("resetLoadCase",&XC::Preprocessor::resetLoadCase)
  .def("setDeadSRF",XC::Preprocessor::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  ;

  }

