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

class_<XC::CentralDifference, bases<XC::RayleighBase>, boost::noncopyable >("CentralDifference", no_init);

class_<XC::Collocation , bases<XC::RayleighBase>, boost::noncopyable >("Collocation", no_init);

class_<XC::HHTGeneralized , bases<XC::RayleighBase>, boost::noncopyable >("HHTGeneralized", no_init);

class_<XC::WilsonTheta, bases<XC::RayleighBase>, boost::noncopyable >("WilsonTheta", no_init);

class_<XC::HHTRayleighBase, bases<XC::RayleighBase>, boost::noncopyable >("HHTRayleighBase", no_init);

class_<XC::HHTExplicit , bases<XC::HHTRayleighBase>, boost::noncopyable >("HHTExplicit", no_init);

class_<XC::HHTBase, bases<XC::HHTRayleighBase>, boost::noncopyable >("HHTBase", no_init);

class_<XC::CollocationHybridSimulation , bases<XC::HHTBase>, boost::noncopyable >("CollocationHybridSimulation", no_init);

class_<XC::HHT, bases<XC::HHTBase>, boost::noncopyable >("HHT", no_init);

class_<XC::HHTGeneralizedExplicit , bases<XC::HHTBase>, boost::noncopyable >("HHTGeneralizedExplicit", no_init);

class_<XC::HHTHybridSimulation , bases<XC::HHTBase>, boost::noncopyable >("HHTHybridSimulation", no_init);

class_<XC::AlphaOSBase , bases<XC::HHTBase>, boost::noncopyable >("AlphaOSBase", no_init);

class_<XC::AlphaOS , bases<XC::AlphaOSBase>, boost::noncopyable >("AlphaOS", no_init);

class_<XC::AlphaOSGeneralized, bases<XC::AlphaOSBase>, boost::noncopyable >("AlphaOSGeneralized", no_init);



