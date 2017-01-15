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
// $Date: 2003/02/14 23:01:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam2d/beam2d02.h,v $
                                                                        
                                                                        
// File: ~/model/element/beam2d02.h
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for beam2d02.
// beam2d02 is a linear elastic plane frame member.

#ifndef beam2d02_h
#define beam2d02_h

#include "beam2d.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class CrdTransf2d;

//! \ingroup ElemBarra
//
//! @brief Elemento barra 02 para problemas bidimensionales.
class beam2d02 : public beam2d
  {
  private:
    // private attributes - a copy for each object of the class        
    double M;

    Matrix Kd; // the stiffness matrix
    mutable Matrix m;//!< the mass matrix	
    Matrix d; // the damping matrix

    mutable Vector q;
    mutable Vector rForce;
    Matrix trans;  // for the transformation matrix
//    int isStiffFormed;
    CrdTransf2d *theCoordTransf;

    const Matrix &getStiff(void) const;    
    
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    beam2d02(int tag= 0);
    beam2d02(int tag, double A, double E, double I, int Nd1, int Nd2,CrdTransf2d &theTrans, double rho = 0.0);
    Element *getCopy(void) const;
    ~beam2d02(void);

    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;

    void setDomain(Domain *theDomain);
    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);            
    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);

    
  };
} // end of XC namespace

#endif

