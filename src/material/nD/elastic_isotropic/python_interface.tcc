//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC is free software: you can redistribute it and/or modify 
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
