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

XC::Mesh &(XC::Domain::*getMeshRef)(void)= &XC::Domain::getMesh;
class_<XC::Domain, bases<XC::ObjWithRecorders>, boost::noncopyable >("Domain", no_init)
  .add_property("getMesh", make_function( getMeshRef, return_internal_reference<>() ))
  .def("setDeadSRF",XC::Domain::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  .def("commit",&XC::Domain::commit)
  .def("revertToLastCommit",&XC::Domain::revertToLastCommit)
  .def("revertToStart",&XC::Domain::revertToStart)  
  .def("setTime",&XC::Domain::setTime,"Assigns time tracker time.")  
  ;
