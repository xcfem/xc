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

class_<XC::EQBasePattern, bases<XC::LoadPattern>, boost::noncopyable >("EQBasePattern", no_init);

class_<XC::EarthquakePattern , bases<XC::EQBasePattern>, boost::noncopyable >("EarthquakePattern", no_init);

class_<XC::UniformExcitation, bases<XC::EarthquakePattern>, boost::noncopyable >("UniformExcitation", no_init)
  .add_property("motionRecord", make_function( &XC::UniformExcitation::getGroundMotionRecord, return_internal_reference<>()),"Ground motion data.")
  ;

class_<XC::MultiSupportPattern, bases<XC::EQBasePattern>, boost::noncopyable >("MultiSupportPattern", no_init);

class_<XC::PBowlLoading , bases<XC::LoadPattern>, boost::noncopyable >("PBowlLoading", no_init);


