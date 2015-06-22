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
//
//
//////////////////////////////////////////////////////////////////////

#include "PlasticHardeningMaterial.h"
#include <cmath>
#include "utility/matrix/Vector.h"


#define evolDebug 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::PlasticHardeningMaterial::PlasticHardeningMaterial(int tag, int classTag)
:Material(tag, classTag), val_hist(0), val_trial(0),
 residual(1.0), sFactor(1.0) {}

int XC::PlasticHardeningMaterial::setTrialValue(double xVal, double factor)
  {
    sFactor = factor;
    val_trial = xVal;
    if(val_trial < 0) val_trial = 0;
    return 0;	
  }

double XC::PlasticHardeningMaterial::getTrialValue(void)
  { return val_trial; }

int XC::PlasticHardeningMaterial::setTrialIncrValue(double dxVal)
  {
    sFactor = 1.0;
    val_trial = val_hist + dxVal;
    if(val_trial < 0) val_trial = 0;
    return 0;	
  }


void XC::PlasticHardeningMaterial::setResidual(double res)
  { residual = res; }

int XC::PlasticHardeningMaterial::commitState(void)
  {
    val_hist= val_trial;
    sFactor = 1.0;
    //cerr << "------ Ep value = " <<  val_hist << std::endl;
    return 0;	
  }

int XC::PlasticHardeningMaterial::revertToLastCommit(void)
  {
    val_trial = val_hist;
    return 0;	
  }

int XC::PlasticHardeningMaterial::revertToStart (void)
  {
    val_trial = 0;
    val_hist = 0;
    return 0;	
  }

//! @brief Devuelve el valor de la tensión generalizada.
const XC::Vector &XC::PlasticHardeningMaterial::getGeneralizedStress(void) const
  {
    static Vector retval;
    std::cerr << "PlasticHardeningMaterial::getGeneralizedStress no implementada." << std::endl;
    return retval;
  }

//! @brief Devuelve el valor de la deformación generalizada.
const XC::Vector &XC::PlasticHardeningMaterial::getGeneralizedStrain(void) const
  {
    static Vector retval;
    std::cerr << "PlasticHardeningMaterial::getGeneralizedStrain no implementada." << std::endl;
    return retval;
  }

//! @brief Asigna a la deformación inicial el valor que se pasa como parámetro.
void XC::PlasticHardeningMaterial::setInitialGeneralizedStrain(const Vector &)
  {
    std::cerr << "PlasticHardeningMaterial::setInitialGeneralizedStrain no implementada." << std::endl;
  }

//! @brief Añade a la deformación inicial el valor que se pasa como parámetro.
const XC::Vector &XC::PlasticHardeningMaterial::getInitialGeneralizedStrain(void) const
  {
    static Vector retval;
    std::cerr << "PlasticHardeningMaterial::getInitialGeneralizedStrain no implementada." << std::endl;
    return retval;
  }


XC::Response *XC::PlasticHardeningMaterial::setResponse(const std::vector<std::string> &argv, Information &matInformation)
  { return 0; }

int XC::PlasticHardeningMaterial::getResponse(int responseID, Information &matInformation)
  { return -1; }

void XC::PlasticHardeningMaterial::Print(std::ostream &s, int flag)
  {
     s << "+Plastic XC::Material: " << getTag() << std::endl;
     s << "|  sFactor =  " << sFactor << std::endl;
     s << "|  val_hist = " << val_hist << ", val_trial = " << val_trial << std::endl;
     return;
  }


