// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/transformation/TransformationFE.h,v $
                                                                        
                                                                        
#ifndef TransformationFE_h
#define TransformationFE_h

// Written: fmk 
// Created: 05/99
// Revision: A
//
// Description: This file contains the class definition for TransformationFE.
// TransformationFE objects handle MFreedom_Constraints using the transformation
// method T^t K T. SFreedom_Constraints are handled by the TransformationConstraintHandler.
//
// What: "@(#) TransformationFE.h, revA"

#include <solution/analysis/model/fe_ele/FE_Element.h>
#include "solution/analysis/UnbalAndTangent.h"

namespace XC {
class SFreedom_Constraint;
class DOF_Group;
class TransformationConstraintHandler;

//! @ingroup AnalysisFE
//
//! @brief TransformationFE objects handle MFreedom_Constraints using
//! the transformation method T^t K T. SFreedom_Constraints are
//! handled by the TransformationConstraintHandler.
//!
//! TransformationFE is a subclass of FE\_Element used to enforce a
//! multi point constraint, of the form \f$U_c = C_{cr} U_r\f$, where
//! \f$U_c\f$ are the constrained degrees-of-freedom at the constrained node,
//! \f$U_r\f$ are the retained degrees-of-freedom at the retained node and
//! \f$C_{cr}\f$ a matrix defining the relationship between these
//! degrees-of-freedom. 
//! 
//! To enforce the constraint a matrix \f$T^T K T\f$ is added to the
//! tangent and \f$T^T R\f$ is added to the residual where \f$T\f$ is a block
//! diagonal matrix equal to WHAT?
class TransformationFE: public FE_Element
  {
  private:
    std::vector<DOF_Group *> theDOFs; //!< DOF groups.
    ID modID;
    int numGroups;
    int numTransformedDOF;
    int numOriginalDOF;
    UnbalAndTangent unbalAndTangentMod;
    
    // static variables - single copy for all objects of the class	
    static UnbalAndTangentStorage unbalAndTangentArrayMod; //!< array of class wide vectors and matrices
    static std::vector<Matrix *> theTransformations; //!< for holding pointers to the T matrices
    static int numTransFE;     //!< number of objects    
    static int transCounter;   //!< a counter used to indicate when to do something
    static int sizeTransformations; //!< size of theTransformations array
    static Vector dataBuffer;
    static Vector localKbuffer;
    static ID dofData;
    static int sizeBuffer;
  protected:
    int transformResponse(const Vector &modResponse, Vector &unmodResponse);
 
    friend class AnalysisModel;
    TransformationFE(int tag, Element *theElement);
  public:
    ~TransformationFE(void);    

    // public methods for setting/obtaining mapping information
    virtual const ID &getDOFtags(void) const;
    virtual const ID &getID(void) const;
    void setAnalysisModel(AnalysisModel &theModel);
    virtual int setID(void);
    
    // methods to form and obtain the tangent and residual
    virtual const Matrix &getTangent(Integrator *theIntegrator);
    virtual const Vector &getResidual(Integrator *theIntegrator);
    
    // methods for ele-by-ele strategies
    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);
    virtual const Vector &getK_Force(const Vector &accel, double fcat = 1.0);
    virtual const Vector &getM_Force(const Vector &accel, double fcat = 1.0);
    virtual const Vector &getC_Force(const Vector &vel, double fcat = 1.0);
    virtual void addD_Force(const Vector &vel,   double fact = 1.0);
    virtual void addM_Force(const Vector &accel, double fact = 1.0);    
    
    const Vector &getLastResponse(void);
    int addSP(SFreedom_Constraint &theSP);

    // AddingSensitivity:BEGIN ////////////////////////////////////
    virtual void addM_ForceSensitivity(int gradNumber, const Vector &vect, double fact = 1.0);
    virtual void addD_ForceSensitivity(int gradNumber, const Vector &vect, double fact = 1.0);
    // AddingSensitivity:END //////////////////////////////////////

  };
} // end of XC namespace

#endif




