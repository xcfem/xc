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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/11/29 21:59:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/DistributedDisplacementControl.h,v $

// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for DistributedDisplacementControl.
// DistributedDisplacementControl is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: 
//  i=1        delta U^T delta U + alpha^2 delta lambda^2 = delta s^2
//  i>1        dU^T delta U + alpha^2 dLambda delta lambda = 0
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and DistributedDisplacementControl is a control parameter.
//
// What: "@(#) DistributedDisplacementControl.h, revA"

#ifndef DistributedDisplacementControl_h
#define DistributedDisplacementControl_h

#include "solution/analysis/integrator/static/DisplacementControl.h"
#include "utility/actor/actor/DistributedObj.h"

namespace XC {
class LinearSOE;
class AnalysisModel;
class Domain;

//! @ingroup StaticIntegrator
//
//! @brief ??.
class DistributedDisplacementControl: public DisplacementControl, public DistributedObj
  {
  private:
    bool allHaveDofID;
    
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class SoluMethod;
    DistributedDisplacementControl(SoluMethod *);
    DistributedDisplacementControl(SoluMethod *,int node, int dof, double increment, int numIncrStep, double minIncrement, double maxIncrement);
    Integrator *getCopy(void) const;
  public:

    int newStep(void);    
    int update(const Vector &deltaU);
    int domainChanged(void);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    
  };
inline Integrator *DistributedDisplacementControl::getCopy(void) const
  { return new DistributedDisplacementControl(*this); }
} // fin namespace XC

#endif

