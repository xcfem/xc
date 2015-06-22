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

// class_<XC::ResponseQuantities, bases<XC::MovableObject>, boost::noncopyable >("ResponseQuantities", no_init);

class_<XC::CentralDifferenceBase , bases<XC::TransientIntegrator>, boost::noncopyable >("CentralDifferenceBase", no_init);

class_<XC::CentralDifferenceAlternative , bases<XC::CentralDifferenceBase>, boost::noncopyable >("CentralDifferenceAlternative", no_init);

class_<XC::CentralDifferenceNoDamping, bases<XC::CentralDifferenceBase>, boost::noncopyable >("CentralDifferenceNoDamping", no_init);

class_<XC::DampingFactorsIntegrator, bases<XC::TransientIntegrator>, boost::noncopyable >("DampingFactorsIntegrator", no_init);

class_<XC::NewmarkBase, bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("NewmarkBase", no_init);

class_<XC::RayleighBase , bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("RayleighBase", no_init);

class_<XC::HHT1, bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("HHT1", no_init);

#include "newmark/python_interface.tcc"
#include "rayleigh/python_interface.tcc"

