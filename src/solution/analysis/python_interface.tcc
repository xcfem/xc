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

#include "analysis/python_interface.tcc"
#include "handler/python_interface.tcc"
#include "numberer/python_interface.tcc"
#include "algorithm/python_interface.tcc"
#include "integrator/python_interface.tcc"
#include "convergenceTest/python_interface.tcc"

class_<XC::ModelWrapper, bases<EntCmd>, boost::noncopyable >("ModelWrapper", no_init)
    .def("newNumberer", &XC::ModelWrapper::newNumberer,return_internal_reference<>(),"Crea un nuevo numerador del tipo que se pasa como parámetro.")
    .def("newConstraintHandler", &XC::ModelWrapper::newConstraintHandler,return_internal_reference<>(),"Crea un nuevo manejador de coacciones del tipo que se pasa como parámetro.")
    ;

class_<XC::MapModelWrapper, bases<EntCmd>, boost::noncopyable >("MapModelWrapper", no_init)
    .add_property("existeModelWrapper", &XC::MapModelWrapper::existeModelWrapper)
    .def("newModelWrapper", &XC::MapModelWrapper::creaModelWrapper,return_internal_reference<>(),"Crea un nuevo contenedor de ModelWrappers.")
  ;
