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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/12/01 01:12:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/LinearSOE.h,v $
                                                                        
                                                                        
#ifndef LinearSOE_h
#define LinearSOE_h

// File: ~/system_of_eqn/LinearSOE.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
//
// What: "@(#) LinearSOE.h, revA"

#include <solution/system_of_eqn/SystemOfEqn.h>

namespace XC {
class LinearSOESolver;
class Graph;
class Matrix;
class Vector;
class ID;

//!  @ingroup SOE
//! 
//! @defgroup LinearSOE Sistemas de ecuaciones lineales.
//
//! @ingroup LinearSOE
//
//! @brief Objetos que se encargan de obtener la solución
//! de problemas de eigenvalues.
//! @brief Sistema de ecuaciones lineal.
//!
//! This is the class definition for LinearSOE.
//! LinearSOE is an abstract base class and thus no objects of it's type
//! can be instantiated. It has pure virtual functions which must be
//! implemented in it's derived classes. LinearSystemOfEqn is an abstraction 
//! of the linear system of equation given by : [A]{X} = {B} - {C},
//! where A is a matrix and B,C and X are vectors. To solve the equation means
//! given A, B and C to find the unknown X such that the equation is satisfied.
class LinearSOE : public SystemOfEqn
  {
  private:
    LinearSOESolver *theSolver;
  protected:
    void libera(void);
    void copia(const LinearSOESolver *);
    friend class FEM_ObjectBroker;
    virtual bool setSolver(LinearSOESolver *);
    int setSolverSize(void);

    LinearSOE(SoluMethod *,int classTag);
  public:
    virtual ~LinearSOE(void);

    virtual int solve(void);    

    // pure virtual functions
    virtual int setSize(Graph &theGraph) =0;    
    virtual int getNumEqn(void) const =0;
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0) =0;
    virtual int addB(const Vector &, const ID &,const double &fact= 1.0) =0;    
    virtual int setB(const Vector &, const double &fact= 1.0) =0;        

    virtual void zeroA(void) =0;
    virtual void zeroB(void) =0;

    virtual const Vector &getX(void) const= 0;
    virtual const Vector &getB(void) const= 0;    
    virtual double getDeterminant(void);
    virtual double normRHS(void) const= 0;

    virtual void setX(int loc, double value) =0;
    virtual void setX(const Vector &X) =0;
    
    LinearSOESolver *getSolver(void);
    LinearSOESolver &newSolver(const std::string &);
  };
} // end of XC namespace


#endif
