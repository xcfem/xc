//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/EigenIntegrator.h,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/eigenIntegrator/EigenIntegrator.h
//
// Written: Jun Peng
// Created: Wed Jan 27, 1999
// Revision: A
//
// Description: This file contains the class definition of EigenIntegrator.
// EigenIntegrator is an algorithmic class for setting up the finite element 
// equations for a eigen problem analysis. 
//
// This class is inheritanted from the base class of Integrator which was
// created by fmk (Frank).


#ifndef EigenIntegrator_h
#define EigenIntegrator_h

#include <solution/analysis/integrator/Integrator.h>

namespace XC {
class EigenSOE;
class AnalysisModel;
class FE_Element;
class DOF_Group;
class Vector;

//! @ingroup AnalysisIntegrator
//
//! @defgroup EigenIntegrator Integrators for eigenproblems.
//
//! @ingroup EigenIntegrator
//
//! @brief Base class for eigenproblem integrators.
class EigenIntegrator: public Integrator
  {
  protected:
    int flagK;

    EigenSOE *getEigenSOEPtr(void);
    const EigenSOE *getEigenSOEPtr(void) const;

    friend class SoluMethod;
    EigenIntegrator(SoluMethod *);
    Integrator *getCopy(void) const;
  public:
     
    // methods to form the M and K matrices.
    virtual int formK(void);
    virtual int formM(void);
     
    // methods to instruct the FE_Element and DOF_Group objects
    // how to determing their contribution to M and K
    virtual int formEleTangK(FE_Element *theEle);
    virtual int formEleTangM(FE_Element *theEle);
    virtual int formNodTangM(DOF_Group *theDof);
    virtual int update(const Vector &deltaU);

    virtual int formEleTangent(FE_Element *theEle);
    virtual int formNodTangent(DOF_Group *theDof);
    virtual int formEleResidual(FE_Element *theEle);
    virtual int formNodUnbalance(DOF_Group *theDof);

    virtual int newStep(void);
    
    virtual int getLastResponse(Vector &result, const ID &id);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);     
  };
inline Integrator *EigenIntegrator::getCopy(void) const
  { return new EigenIntegrator(*this); }
} // end of XC namespace

#endif




