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
//# PROJECT:           Object Oriented Finite Element Program
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
//# DATE:              July 2004
//# UPDATE HISTORY:
//#
//===============================================================================

#ifndef fdYield_H
#define fdYield_H

#include <iostream>

namespace XC {
  class stresstensor;
  class FDEPState;

//! @ingroup FDNDMat
//
//! @defgroup FDYieldNDMat Superficie de fluencia.
//
//! @ingroup FDYieldNDMat
//
//! @brief ??.
class fdYield
  {
  public:
    fdYield();
    virtual ~fdYield() {}; 
    
    virtual fdYield *newObj() = 0;   

    virtual int getNumRank();
    virtual double getTolerance();
    virtual double Yd(const stresstensor &sts, const FDEPState &fdepstate ) const = 0;	
    
    virtual stresstensor dYods(const stresstensor &sts, const FDEPState &fdepstate ) const = 0; 
    
    virtual double dYodq(const stresstensor &sts, const FDEPState &fdepstate ) const;	 
    virtual stresstensor dYoda(const stresstensor &sts, const FDEPState &fdepstate ) const; 
    
    virtual void print() = 0; 

    friend std::ostream& operator<< (std::ostream& os, const fdYield & fdyd);
  };
} // fin namespace XC


#endif
