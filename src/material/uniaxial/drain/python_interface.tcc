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

class_<XC::DrainBilinearMaterial , bases<XC::DrainMaterial>, boost::noncopyable >("DrainBilinearMaterial", no_init);

class_<XC::DrainClough1Material , bases<XC::DrainMaterial>, boost::noncopyable >("DrainClough1Material", no_init);

class_<XC::DrainClough2Material, bases<XC::DrainMaterial>, boost::noncopyable >("DrainClough2Material", no_init);

//class_<XC::DrainHardeningMaterial , bases<XC::DrainMaterial>, boost::noncopyable >("DrainHardeningMaterial", no_init);

class_<XC::DrainPinch1Material , bases<XC::DrainMaterial>, boost::noncopyable >("DrainPinch1Material", no_init);

