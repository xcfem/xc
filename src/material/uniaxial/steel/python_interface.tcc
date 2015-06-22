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

class_<XC::SteelBase, bases<XC::UniaxialMaterial>, boost::noncopyable >("SteelBaseMaterial", no_init)
  .add_property("E", &XC::SteelBase::getInitialTangent, &XC::SteelBase::setInitialTangent,"Intial Young's modulus.")
  .add_property("fy", &XC::SteelBase::getFy, &XC::SteelBase::setFy,"Yield stress.")
  .add_property("b", &XC::SteelBase::getHardeningRatio, &XC::SteelBase::setHardeningRatio,"Hardening ratio.")
   ;

class_<XC::Steel02, bases<XC::SteelBase> >("Steel02")
  .add_property("initialStress", &XC::Steel02::getInitialStress, &XC::Steel02::setInitialStress,"Initial stress.")
   ;

class_<XC::SteelBase0103, bases<XC::SteelBase>, boost::noncopyable >("SteelBase0103", no_init);

class_<XC::Steel01 , bases<XC::SteelBase0103>, boost::noncopyable >("Steel01", no_init);

//class_<XC::Steel03 , bases<XC::SteelBase0103>, boost::noncopyable >("Steel03", no_init);


