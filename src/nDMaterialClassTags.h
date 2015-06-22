//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
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

#endif
