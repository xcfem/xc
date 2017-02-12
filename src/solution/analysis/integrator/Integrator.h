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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Integrator.h,v $
                                                                        
                                                                        
#ifndef Integrator_h
#define Integrator_h

// File: ~/analysis/integrator/Integrator.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class interface for Integrator.
// Integrator is an abstract base class, i.e. no objects of it's
// type can be created. 
//
// What: "@(#) Integrator.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
class FE_Element;
class DOF_Group;
class SystemOfEqn;
class AnalysisModel;
class Vector;
class ID;
class FEM_ObjectBroker;
class RayleighDampingFactors;
class SoluMethod;

//! @ingroup Analysis
//
//! @defgroup AnalysisIntegrator Integration of physical
//! properties over the domain to form the system stiffness matrix.
//
//! @ingroup AnalysisIntegrator
//
//! @brief Base class for the object that performs the
//! integration of physical properties over the domain to
//! form the system stiffness matrix
class Integrator: public MovableObject, public EntCmd
  {
  protected:
    SoluMethod *getSoluMethod(void);
    const SoluMethod *getSoluMethod(void) const;
    void applyLoadModel(double newTime);
    int updateModel(void);
    int updateModel(double newTime, double dT);
    int commitModel(void);
    double getCurrentModelTime(void);
    void setCurrentModelTime(const double &t);
    void setRayleighDampingFactors(const RayleighDampingFactors &rF);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    virtual AnalysisModel *getAnalysisModelPtr(void);
    virtual const AnalysisModel *getAnalysisModelPtr(void) const;

    Integrator(SoluMethod *,int classTag);
    friend class SoluMethod;
    virtual Integrator *getCopy(void) const= 0;
  public:
    inline virtual ~Integrator(void) {}
    virtual int domainChanged(void);

    virtual int formEleTangent(FE_Element *theEle) =0;
    virtual int formNodTangent(DOF_Group *theDof) =0;    
    virtual int formEleResidual(FE_Element *theEle) =0;
    virtual int formNodUnbalance(DOF_Group *theDof) =0;    

    // Methods provided for Domain Decomposition
    virtual int getLastResponse(Vector &result, const ID &id) =0;

    // Method provided for Output
    virtual void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif






