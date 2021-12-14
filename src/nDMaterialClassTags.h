//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
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
//nDMaterialClassTags.h

#ifndef nDMaterialClassTags_h
#define nDMaterialClassTags_h

#define ND_TAG_ElasticIsotropic			        2001
#define ND_TAG_ElasticIsotropicPlaneStrain2d	        2002
#define ND_TAG_ElasticIsotropicPlaneStress2d	        2003
#define ND_TAG_ElasticIsotropicAxiSymm                  2004
#define ND_TAG_ElasticIsotropicPlateFiber		2005
#define ND_TAG_ElasticIsotropicBeamFiber		2006
#define ND_TAG_ElasticIsotropic3D                       2007
#define ND_TAG_ElasticCrossAnisotropic3D                2008
#define ND_TAG_J2PlaneStrain                            2009 
#define ND_TAG_J2PlaneStress                            2010 
#define ND_TAG_J2AxiSymm                                2011 
#define ND_TAG_J2ThreeDimensional                       2012 
#define ND_TAG_J2PlateFiber				2013
#define ND_TAG_J2PlateFibre			       91625
#define ND_TAG_J2BeamFiber				2014
#define ND_TAG_PressureDependentElastic3D               2015
#define ND_TAG_Template3Dep 			        2016
#define ND_TAG_FluidSolidPorousMaterial                 2017
#define ND_TAG_PressureDependMultiYield		        2018
#define ND_TAG_PressureIndependMultiYield		2019
#define ND_TAG_PressureDependMultiYield02		2020
#define ND_TAG_FeapMaterial                             2021
#define ND_TAG_FeapMaterial01                           2022
#define ND_TAG_FeapMaterial02                           2023
#define ND_TAG_FeapMaterial03                           2024
#define ND_TAG_PlaneStressMaterial                      2025
#define ND_TAG_PlateFiberMaterial                       2026
#define ND_TAG_BeamFiberMaterial		        2027
#define ND_TAG_CompressibleFluid		        2028
#define ND_TAG_GeneralizedPlasticity                    2029
#define ND_TAG_J2Plasticity02                           2030
#define ND_TAG_FiniteDeformationElastic3D	        2031
#define ND_TAG_NeoHookeanCompressible3D	                2032
#define ND_TAG_FDdecoupledElastic3D	                2033
#define ND_TAG_FiniteDeformationEP3D	                2034

// MultiaxialCyclicPlasticity, add by Gang Wang
#define ND_TAG_MultiaxialCyclicPlasticity               2035
#define ND_TAG_MultiaxialCyclicPlasticity3D             2036 
#define ND_TAG_MultiaxialCyclicPlasticityAxiSymm        2037
#define ND_TAG_MultiaxialCyclicPlasticityPlaneStrain    2038

// Drucker-Prager - P.Arduino
#define ND_TAG_DruckerPrager			14003
#define ND_TAG_DruckerPragerThreeDimensional	14004
#define ND_TAG_DruckerPragerTensionCutoff	14005
#define ND_TAG_DruckerPrager3D	                14006
#define ND_TAG_DruckerPragerPlaneStrain         14007

#endif
