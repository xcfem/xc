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

class_<XC::FedeasBondMaterial, bases<XC::FedeasMaterial>, boost::noncopyable >("FedeasBondMaterial", no_init);

class_<XC::FedeasHardeningMaterial , bases<XC::FedeasMaterial>, boost::noncopyable >("FedeasHardeningMaterial", no_init);

class_<XC::FedeasHyster1Material , bases<XC::FedeasMaterial>, boost::noncopyable >("FedeasHyster1Material", no_init);

class_<XC::FedeasHyster2Material , bases<XC::FedeasMaterial>, boost::noncopyable >("FedeasHyster2Material", no_init);

class_<XC::FedeasSteel1Material , bases<XC::FedeasMaterial>, boost::noncopyable >("FedeasSteel1Material", no_init);

class_<XC::FedeasSteel2Material , bases<XC::FedeasMaterial>, boost::noncopyable >("FedeasSteel2Material", no_init);


class_<XC::FedeasBond1Material, bases<XC::FedeasBondMaterial>, boost::noncopyable >("FedeasBond1Material", no_init);

class_<XC::FedeasBond2Material , bases<XC::FedeasBondMaterial>, boost::noncopyable >("FedeasBond2Material", no_init);

class_<XC::FedeasConcrMaterial, bases<XC::FedeasMaterial>, boost::noncopyable >("FedeasConcrMaterial", no_init);

class_<XC::FedeasConcr1Material , bases<XC::FedeasConcrMaterial>, boost::noncopyable >("FedeasConcr1Material", no_init);

class_<XC::FedeasConcr2Material, bases<XC::FedeasConcrMaterial>, boost::noncopyable >("FedeasConcr2Material", no_init);

class_<XC::FedeasConcr3Material, bases<XC::FedeasConcrMaterial>, boost::noncopyable >("FedeasConcr3Material", no_init);


