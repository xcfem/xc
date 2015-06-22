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

#ifndef NeoHookeanWEnergy_CPP
#define NeoHookeanWEnergy_CPP

#include "NeoHookeanWEnergy.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/nDarray/Tensor.h>

//================================================================================
// Normal constructor
//================================================================================
XC::NeoHookeanWEnergy::NeoHookeanWEnergy(double K_in, double G_in )
 :K(K_in), G(G_in) {
}

XC::NeoHookeanWEnergy::NeoHookeanWEnergy(  )
 :K(0.0), G(0.0) {}

//================================================================================
//create a clone of itself
//================================================================================
XC::WEnergy * XC::NeoHookeanWEnergy::getCopy(void) const
  { return new NeoHookeanWEnergy(*this); }


//================================================================================
// w
//================================================================================
const double  XC::NeoHookeanWEnergy::wE(const double &J_in, const XC::Vector &lambda_wave_in )
  {
    double w_iso = 0.5 * G * (lambda_wave_in(0) * lambda_wave_in(0)
                            + lambda_wave_in(1) * lambda_wave_in(1)
                            + lambda_wave_in(2) * lambda_wave_in(2) );

    double w_vol = 0.5 * K * (J_in-1.0) * (J_in-1.0); //version I
//    double w_vol = 0.25 * K * ( J_in*J_in -1.0 -2.0*log(J_in) );  //version II
//    double w_vol = 0.0; //incompressible material
    double w_total = w_iso + w_vol;
    return w_total;
  }

//================================================================================
// d(iso)w / d(lambda)
//================================================================================
const XC::Vector  XC::NeoHookeanWEnergy::disowOdlambda(const XC::Vector &lambda_wave_in )
  {
    XC::Vector disowOverdlambda(3);
    disowOverdlambda(0) = G * lambda_wave_in(0) ;
    disowOverdlambda(1) = G * lambda_wave_in(1) ;
    disowOverdlambda(2) = G * lambda_wave_in(2) ;

    return disowOverdlambda;
  }

//================================================================================
// d2(iso)w / d(lambda)2
//================================================================================
const XC::Vector  XC::NeoHookeanWEnergy::d2isowOdlambda2(const XC::Vector &lambda_wave_in )
  {
    XC::Vector d2isowOverdlambda2(3);
    d2isowOverdlambda2(0) = G;
    d2isowOverdlambda2(1) = G;
    d2isowOverdlambda2(2) = G;
    return d2isowOverdlambda2;
  }


//================================================================================
// d(vol)w / dJ
//================================================================================
const double  XC::NeoHookeanWEnergy::dvolwOdJ(const double &J_in )
{
  double temp1 = K * (J_in - 1.0);  // Version I
//  double temp1 = 0.0;
  return temp1;
}

//================================================================================
// d2(vol)w / dJ2
//================================================================================
const double  XC::NeoHookeanWEnergy::d2volwOdJ2(const double &J_in )
{
  double temp2 = K ;  // version I
//  double temp2 = 0.0;
  return temp2;
}


#endif

