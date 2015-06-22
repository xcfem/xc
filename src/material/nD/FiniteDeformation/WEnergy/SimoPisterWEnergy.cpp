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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite XC::Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:             
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              19AUg2003
//# UPDATE HISTORY:    Sept 2003
//#		       28May2004
//#
//===============================================================================

#ifndef SimoPisterWEnergy_CPP
#define SimoPisterWEnergy_CPP

#include "SimoPisterWEnergy.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/nDarray/Tensor.h>

//================================================================================
// Normal constructor
//================================================================================
XC::SimoPisterWEnergy::SimoPisterWEnergy(const double &K_in)
  :K(K_in) {}


//================================================================================
//! @brief Constructor virtual
//================================================================================
XC::WEnergy *XC::SimoPisterWEnergy::getCopy(void) const
  { return new SimoPisterWEnergy(*this); }


//================================================================================
// w
//================================================================================
const double  XC::SimoPisterWEnergy::wE( const double &J_in, const XC::Vector &lambda_wave_in )
{
    double wEnergy = 0.25 * K * (J_in*J_in - 1.0 - 2.0*log(J_in));
    return wEnergy;
}

//================================================================================
// d(vol)w / dJ
//================================================================================
const double  XC::SimoPisterWEnergy::dvolwOdJ(const double &J_in )
{
   double dcolwOverdJ = K * (-2.0 / J_in + 2.0 * J_in) * 0.25;
   return dcolwOverdJ;
}

//================================================================================
// d2(vol)w / dJ2
//================================================================================
const double  XC::SimoPisterWEnergy::d2volwOdJ2(const double &J_in )
{
   double d2colwOverdJ2 = K * (2.0 / J_in / J_in + 2.0) * 0.25;
   return d2colwOverdJ2;
}


#endif

