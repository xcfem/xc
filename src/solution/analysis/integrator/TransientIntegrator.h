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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/03/06 20:32:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/TransientIntegrator.h,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/TransientIntegrator.h
// 
// Written: fmk 
// Created: Tue Sept 17 15:54:47: 1996
// Revision: A
//
// Description: This file contains the class definition for TransientIntegrator.
// TransientIntegrator is an algorithmic class for setting up the finite element
// equations for a static analysis and for Incrementing the nodal displacements
// with the values in the soln vector to the LinearSOE object. 
//
// What: "@(#) TransientIntegrator.h, revA"

#ifndef TransientIntegrator_h
#define TransientIntegrator_h

#include <solution/analysis/integrator/IncrementalIntegrator.h>
namespace XC {
class Information;
class LinearSOE;
class AnalysisModel;
class FE_Element;
class DOF_Group;
class Vector;

//! @ingroup AnalisisIntegrator
//
//! @defgroup TransientIntegrator Integration of the dynamic equations of motion.
//
//! @ingroup TransientIntegrator
//
//! @brief Base class for dynamic equations of motion integrators.
class TransientIntegrator : public IncrementalIntegrator
  {
  protected:
    TransientIntegrator(SoluMethod *,int classTag);
  public:

    virtual int formTangent(int statFlag);
    virtual int formEleResidual(FE_Element *theEle);
    virtual int formNodUnbalance(DOF_Group *theDof);    
    virtual int initialize(void) {return 0;};    
  };
} // end of XC namespace

#endif

