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


#ifndef TipoMaterialND_H
#define TipoMaterialND_H

#include <string>

namespace XC {
  const std::string strTipo3D= "3D";
  const std::string strTipoAxiSymmetric= "AxiSymmetric";
  const std::string strTipoAxiSymmetric2D= "AxiSymmetric2D";
  const std::string strTipoBeamFiber= "BeamFiber";
  const std::string strTipoElasticIsotropic3D= "ElasticIsotropic3D";
  const std::string strTipoFDEP3D= "FDEP3D";
  const std::string strTipoFiniteDeformationEP3D= "FiniteDeformationEP3D";
  const std::string strTipoFluidSolidPorous= "FluidSolidPorous";
  const std::string strTipoPlaneStress2D= "PlaneStress2D";
  const std::string strTipoPlaneStress= "PlaneStress";
  const std::string strTipoPlaneStrain2D= "PlaneStrain2D";
  const std::string strTipoPlaneStrain= "PlaneStrain";
  const std::string strTipoPlateFiber= "PlateFiber";
  const std::string strTipoPressureIndependMultiYield= "PressureIndependMultiYield";
  const std::string strTipoPressureIndependMultiYield02= "PressureIndependMultiiYield02";
  const std::string strTipoTemplate3Dep= "Template3Dep";
  const std::string strTipoThreeDimensional= "ThreeDimensional";
  const std::string strTipoThreeDimensionalFD= "ThreeDimensionalFD";

  const std::string strTipoError= "error";
} // fin namespace XC

#endif
