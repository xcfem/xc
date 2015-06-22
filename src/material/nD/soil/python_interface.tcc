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

class_<XC::FluidSolidPorousMaterial, bases<XC::NDMaterial>, boost::noncopyable >("FluidSolidPorousMaterial", no_init);

class_<XC::PressureMultiYieldBase, bases<XC::NDMaterial>, boost::noncopyable >("PressureMultiYieldBase", no_init);

class_<XC::PressureDependMultiYieldBase, bases<XC::PressureMultiYieldBase>, boost::noncopyable >("PressureDependMultiYieldBase", no_init);

class_<XC::PressureDependMultiYield, bases<XC::PressureDependMultiYieldBase>, boost::noncopyable >("PressureDependMultiYield", no_init);

//class_<XC::PressureDependMultiYield02, bases<XC::PressureDependMultiYieldBase>, boost::noncopyable >("PressureDependMultiYield02", no_init);


class_<XC::PressureIndependMultiYield, bases<XC::PressureMultiYieldBase>, boost::noncopyable >("PressureIndependMultiYield", no_init)
  .def("setup",&XC::PressureIndependMultiYield::setup)
  ;

class_<XC::T2Vector, bases<XC::MovableObject>, boost::noncopyable >("T2Vector", no_init);

#include "cyclicSoil/python_interface.tcc"
