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


class_<XC::LogWEnergy , bases<XC::WEnergy>, boost::noncopyable >("LogWEnergy", no_init);

class_<XC::MooneyRivlinSimoWEnergy , bases<XC::WEnergy>, boost::noncopyable >("MooneyRivlinSimoWEnergy", no_init);

class_<XC::MooneyRivlinWEnergy , bases<XC::WEnergy>, boost::noncopyable >("MooneyRivlinWEnergy", no_init);

class_<XC::NeoHookeanWEnergy , bases<XC::WEnergy>, boost::noncopyable >("NeoHookeanWEnergy", no_init);

class_<XC::SimoPisterWEnergy , bases<XC::WEnergy>, boost::noncopyable >("SimoPisterWEnergy", no_init);

class_<XC::OgdenWEnergyBase , bases<XC::WEnergy>, boost::noncopyable >("OgdenWEnergyBase", no_init);

class_<XC::OgdenSimoWEnergy , bases<XC::OgdenWEnergyBase>, boost::noncopyable >("OgdenSimoWEnergy", no_init);

class_<XC::OgdenWEnergy , bases<XC::OgdenWEnergyBase>, boost::noncopyable >("OgdenWEnergy", no_init);



