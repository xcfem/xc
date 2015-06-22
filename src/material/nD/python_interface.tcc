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

class_<XC::NDMaterial, XC::NDMaterial *, bases<XC::Material>, boost::noncopyable >("NDMaterial", no_init)
    .add_property("getRho", &XC::NDMaterial::getRho,"Devuelve la densidad del material.")
    .add_property("getE", &XC::NDMaterial::getE)
    .add_property("getnu", &XC::NDMaterial::getnu)
    .add_property("getpsi", &XC::NDMaterial::getpsi)
       ;

class_<XC::ElasticIsotropicMaterial, bases<XC::NDMaterial>, boost::noncopyable >("ElasticIsotropicMaterial", no_init)
    .add_property("rho", &XC::ElasticIsotropicMaterial::getRho, &XC::ElasticIsotropicMaterial::setRho)
    .add_property("E", &XC::ElasticIsotropicMaterial::getE, &XC::ElasticIsotropicMaterial::setE)
    .add_property("nu", &XC::ElasticIsotropicMaterial::getnu, &XC::ElasticIsotropicMaterial::setnu)
       ;
#include "elastic_isotropic/python_interface.tcc"

//class_<XC::FeapMaterial , bases<XC::NDMaterial>, boost::noncopyable >("FeapMaterial", no_init);
#include "feap/python_interface.tcc"

//#include "FiniteDeformation/python_interface.tcc"

class_<XC::J2Plasticity, bases<XC::NDMaterial>, boost::noncopyable >("J2Plasticity", no_init);
#include "j2_plasticity/python_interface.tcc"

class_<XC::NDAdaptorMaterial, bases<XC::NDMaterial>, boost::noncopyable >("NDAdaptorMaterial", no_init);
#include "nd_adaptor/python_interface.tcc"

#include "soil/python_interface.tcc"
