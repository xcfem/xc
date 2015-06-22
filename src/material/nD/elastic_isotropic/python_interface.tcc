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

class_<XC::ElasticIsotropic2D, bases<XC::ElasticIsotropicMaterial>, boost::noncopyable >("ElasticIsotropic2D", no_init)
       ;

class_<XC::ElasticIsotropicPlaneStrain2D, bases<XC::ElasticIsotropic2D> >("ElasticIsotropicPlaneStrain2D")
       ;

class_<XC::ElasticIsotropicPlaneStress2D, bases<XC::ElasticIsotropic2D> >("ElasticIsotropicPlaneStress2D")
       ;

class_<XC::ElasticIsotropic3D , bases<XC::ElasticIsotropicMaterial>, boost::noncopyable >("ElasticIsotropic3D", no_init);

class_<XC::ElasticIsotropicAxiSymm , bases<XC::ElasticIsotropicMaterial>, boost::noncopyable >("ElasticIsotropicAxiSymm", no_init);

//class_<XC::ElasticIsotropicBeamFiber , bases<XC::ElasticIsotropicMaterial>, boost::noncopyable >("ElasticIsotropicBeamFiber", no_init);

class_<XC::ElasticIsotropicPlateFiber , bases<XC::ElasticIsotropicMaterial>, boost::noncopyable >("ElasticIsotropicPlateFiber", no_init);

//class_<XC::PressureDependentElastic3D , bases<XC::ElasticIsotropicMaterial>, boost::noncopyable >("PressureDependentElastic3D", no_init);
