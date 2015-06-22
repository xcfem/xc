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

// $Revision: 1.2 $
// $Date: 2005/12/21 00:31:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/AlphaOSGeneralized.h,v $

#ifndef AlphaOSGeneralized_h
#define AlphaOSGeneralized_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the class definition for AlphaOSGeneralized.
// AlphaOSGeneralized is an algorithmic class for performing a transient analysis
// using the generalized Alpha-Operator-Splitting integration scheme.
// The parameters alpha correspond to 1+alpha_{HHT}.
//
// What: "@(#) AlphaOSGeneralized.h, revA"

#include <solution/analysis/integrator/transient/rayleigh/AlphaOSBase.h>
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {
class DOF_Group;
class FE_Element;

//! @ingroup RayleighIntegrator
//
//! @brief AlphaOSGeneralized is an algorithmic class for performing a transient analysis
//! using the generalized Alpha-Operator-Splitting integration scheme.
//! The parameters alpha correspond to 1+alpha_{HHT}.
class AlphaOSGeneralized: public AlphaOSBase
  {
  private:
    double alphaI;
    double alphaF;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    virtual int formElementResidual(void);

    friend class SoluMethod;
    AlphaOSGeneralized(SoluMethod *);
    AlphaOSGeneralized(SoluMethod *,double rhoInf);
    AlphaOSGeneralized(SoluMethod *,double rhoInf,const RayleighDampingFactors &rF);
    AlphaOSGeneralized(SoluMethod *,double alphaI, double alphaF, double beta, double gamma);
    AlphaOSGeneralized(SoluMethod *,double alphaI, double alphaF, double beta, double gamma,const RayleighDampingFactors &rF);
    Integrator *getCopy(void) const;

  public:
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);

    int domainChanged(void);
    int newStep(double deltaT);    
    int update(const Vector &deltaU);
    int commit(void);
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);        
  };
inline Integrator *AlphaOSGeneralized::getCopy(void) const
  { return new AlphaOSGeneralized(*this); }
} // fin namespace XC

#endif
