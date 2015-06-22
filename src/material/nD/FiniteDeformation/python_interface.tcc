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

class_<XC::FiniteDeformationElastic3D , bases<XC::NDMaterial>, boost::noncopyable >("FiniteDeformationElastic3D", no_init);

class_<XC::FiniteDeformationEP3D, bases<XC::NDMaterial>, boost::noncopyable >("FiniteDeformationEP3D", no_init);

class_<XC::FDdecoupledElastic3D, bases<XC::FiniteDeformationElastic3D>, boost::noncopyable >("FDdecoupledElastic3D", no_init);

class_<XC::NeoHookeanCompressible3D , bases<XC::FiniteDeformationElastic3D>, boost::noncopyable >("NeoHookeanCompressible3D", no_init);

#include "fdFlow/python_interface.tcc"
#include "WEnergy/python_interface.tcc"
#include "fdEvolution/python_interface.tcc"
#include "fdYield/python_interface.tcc"

