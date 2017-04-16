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
// $Date: 2004/09/01 04:01:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/joint/MFreedom_Joint2D.h,v $
                                                                        
#ifndef MFreedom_Joint2D_h
#define MFreedom_Joint2D_h

// Written: Arash Altoontash, Gregory Deierlein
// Created: 08/01
// Revision: Arash

// Purpose: This file contains the class definition for MFreedom_Joint2D.
// It is a sub class for MFreedom_Constraint specialized to be used for simple joint 
// connection element. MFreedom_Joint2D defines a nonlinear, time dependent multi 
// point constraint.
//

#include "MFreedom_Joint.h"

namespace XC {

class Node;
class Matrix;
class ID;


//! @ingroup CContMP
//
//! @brief ??.
class MFreedom_Joint2D : public MFreedom_Joint
  {
  private:
    int MainDOF; //!<  main degree of freedom for rotation
    int AuxDOF; //!<  Auxilary degree of freedom for shear
    int FixedEnd; //!<  fixed rotational degree of freedom at the end released = 0 , fixed = 1
    
  public:
    // constructors        
    MFreedom_Joint2D(void);

    MFreedom_Joint2D(Domain *theDomain, int tag, int nodeRetain, int nodeConstr,int Maindof, int fixedend , int LrgDsp = 0 ); //LrgDsp=0 means large displacement is not enabled

    // method to get information about the constraint
    int applyConstraint(double pseudoTime);
    virtual const Matrix &getConstraint(void) const;

    // methods for output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif

