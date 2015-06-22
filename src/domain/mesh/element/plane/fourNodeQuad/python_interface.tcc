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

class_<XC::FourNodeQuad, bases<QuadBase4N_Mech2D>, boost::noncopyable >("FourNodeQuad", no_init)
  .add_property("rho", &XC::FourNodeQuad::getRho,&XC::FourNodeQuad::setRho)
  .add_property("thickness", &XC::FourNodeQuad::getThickness,&XC::FourNodeQuad::setThickness)
//.add_property("getAvgStress", make_function(&XC::FourNodeQuad::getAvgStress, return_internal_reference<>()), "Returns average stress in element.")
//.add_property("getAvgStrain", make_function(&XC::FourNodeQuad::getAvgStrain, return_internal_reference<>()), "Returns average strain in element.")
  .def("detJ", &XC::FourNodeQuad::detJ)
   ;
