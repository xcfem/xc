// -*-c++-*-
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
                                                                        
// $Revision: 1.13 $
// $Date: 2005/11/29 22:46:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/FE_Element.h,v $
                                                                        
                                                                        
#ifndef FE_Element_h
#define FE_Element_h

// File: ~/analysis/model/fe_ele/FE_Element.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for FE_Element.
//
// What: "@(#) FE_Element.h, revA"

#include <utility/tagged/TaggedObject.h>
#include "utility/matrix/ID.h"
#include "solution/analysis/UnbalAndTangent.h"

namespace XC {
class TransientIntegrator;
class Element;
class Vector;
class Matrix;
class Integrator;
class AnalysisModel;

//! @ingroup AnalysisModel
//!
//! @defgroup AnalysisFE Finite element and constraints as seen by the analysis.
//
//! @ingroup AnalysisFE
//
//! @brief Finite element as seen by analysis.
//!
//! FE\_Element is a base class, subtypes of which are used to
//! enforce the constraints on the domain. An object of type FE\_Element
//! represents an element of the domain in the analysis. It enforces no
//! constraints other than single point homogeneous boundary conditions,
//! imposed on any of the elements nodes. It provides a similar interface
//! to that of an Element but modified to provide features useful to an
//! Analysis class.  The FE\_Element is responsible for:
//!
//! - Holding information about the mapping between equation numbers
//!   and the degrees-of-freedom at the 
//!   element ends, this mapping is determined from the DOF\_Group objects
//!   associated with the elements Node objects.
//! - Providing methods to allow the integrator to combine the elements
//!   stiffness, mass and damping matrices into the elements contribution
//!   to the structure tangent matrix and the elements resisting force to the
//!   structure unbalance. Obtaining the stiffness, damping and mass matrices
//!   from the elements.
//! - Providing methods so other forces can be determined.
//!
//! While the FE\_Element class is associated with an element in the domain,
//! subclasses do not have to be. It is the subclasses that are used to
//! implement the constraints imposed on the nodal displacements in the domain.
class FE_Element: public TaggedObject
  {
  private:
    int numDOF;
    UnbalAndTangent unbalAndTangent;
    AnalysisModel *theModel;
    Element *myEle; //!< Domain element associated with this object.
    Integrator *theIntegrator; //!< need for Subdomain

    // static variables - single copy for all objects of the class	
    static Matrix errMatrix;
    static Vector errVector;
    static UnbalAndTangentStorage unbalAndTangentArray; //!< array of class wide vectors and matrices
    static int numFEs; //!< number of objects
    void set_pointers(void);

  protected:
    void  addLocalM_Force(const Vector &accel, double fact = 1.0);    
    void  addLocalD_Force(const Vector &vel, double fact = 1.0);    
    void  addLocalM_ForceSensitivity(int gradNumber, const Vector &accel, double fact = 1.0);    
    void  addLocalD_ForceSensitivity(int gradNumber, const Vector &vel, double fact = 1.0);    


    // protected variables - a copy for each object of the class        
    ID myDOF_Groups;
    ID myID;

    friend class AnalysisModel;
    FE_Element(int tag, Element *theElement);
    FE_Element(int tag, int numDOF_Group, int ndof);
  public:
    virtual ~FE_Element(void);    

    // public methods for setting/obtaining mapping information
    virtual const ID &getDOFtags(void) const;
    virtual const ID &getID(void) const;
    void setAnalysisModel(AnalysisModel &theModel);
    virtual int  setID(void);
    
    // methods to form and obtain the tangent and residual
    virtual const Matrix &getTangent(Integrator *theIntegrator);
    virtual const Vector &getResidual(Integrator *theIntegrator);

    // methods to allow integrator to build tangent
    virtual void  zeroTangent(void);
    virtual void  addKtToTang(double fact = 1.0);
    virtual void  addKiToTang(double fact = 1.0);
    virtual void  addCtoTang(double fact = 1.0);    
    virtual void  addMtoTang(double fact = 1.0);    
    
    // methods to allow integrator to build residual    
    virtual void  zeroResidual(void);    
    virtual void  addRtoResidual(double fact = 1.0);
    virtual void  addRIncInertiaToResidual(double fact = 1.0);    

    // methods for ele-by-ele strategies
    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);
    virtual const Vector &getK_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getC_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getM_Force(const Vector &x, double fact = 1.0);
    virtual void  addM_Force(const Vector &accel, double fact = 1.0);    
    virtual void  addD_Force(const Vector &vel, double fact = 1.0);    

    virtual int updateElement(void);

    virtual Integrator *getLastIntegrator(void);
    virtual const Vector &getLastResponse(void);
    Element *getElement(void);

    virtual void Print(std::ostream &, int = 0) {return;};

    // AddingSensitivity:BEGIN ////////////////////////////////////
    virtual void addResistingForceSensitivity(int gradNumber, double fact = 1.0);
    virtual void addM_ForceSensitivity(int gradNumber, const Vector &vect, double fact = 1.0);
    virtual void addD_ForceSensitivity(int gradNumber, const Vector &vect, double fact = 1.0);
    virtual int  commitSensitivity(int gradNum, int numGrads);
    // AddingSensitivity:END //////////////////////////////////////
    
  };
} // end of XC namespace

#endif


