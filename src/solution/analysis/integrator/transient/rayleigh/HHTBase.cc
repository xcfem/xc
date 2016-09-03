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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.10 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTBase.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the implementation of the XC::HHTBase class.
//
// What: "@(#) HHTBase.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/HHTBase.h>

//! @brief Constructor.
XC::HHTBase::HHTBase(SoluMethod *owr,int classTag)
  : HHTRayleighBase(owr,classTag), beta(0.0), c1(0.0) {}

//! @brief Constructor.
XC::HHTBase::HHTBase(SoluMethod *owr,int classTag,double _alpha)
  : HHTRayleighBase(owr,classTag,_alpha),
    beta((2-_alpha)*(2-_alpha)*0.25), c1(0.0) {}

//! @brief Constructor.
XC::HHTBase::HHTBase(SoluMethod *owr,int classTag,double _alpha,const RayleighDampingFactors &rF)
  : HHTRayleighBase(owr,classTag,_alpha,rF),
    beta((2-_alpha)*(2-_alpha)*0.25), c1(0.0) {}

//! @brief Constructor.
XC::HHTBase::HHTBase(SoluMethod *owr,int classTag,double _alpha, double _beta, double _gamma)
  : HHTRayleighBase(owr,classTag,_alpha,_gamma),
    beta(_beta), c1(0.0) {}

//! @brief Constructor.
XC::HHTBase::HHTBase(SoluMethod *owr,int classTag,double _alpha, double _beta, double _gamma,const RayleighDampingFactors &rF)
    : HHTRayleighBase(owr,classTag,_alpha,_gamma,rF), beta(_beta), c1(0.0) {}


//! @brief Send members del objeto through the channel being passed as parameter.
int XC::HHTBase::sendData(CommParameters &cp)
  {
    int res= HHTRayleighBase::sendData(cp);
    res+= cp.sendDoubles(beta,c1,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::HHTBase::recvData(const CommParameters &cp)
  {
    int res= HHTRayleighBase::recvData(cp);
    res+= cp.receiveDoubles(beta,c1,getDbTagData(),CommMetaData(8));
    return res;
  }

