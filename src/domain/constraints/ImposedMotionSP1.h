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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/22 19:41:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/ImposedMotionSP1.h,v $
                                                                        
#ifndef ImposedMotionSP1_h
#define ImposedMotionSP1_h

// File: ~/domain/constraints/ImposedMotionSP1.h
//
// Written: fmk 
// Created: 11/00
// Revision: A
//
// Purpose: This file contains the class definition for ImposedMotionSP.
// ImposedMotionSP is a class which returns as the constraint, the DISPLACEMENT 
// ground motion response for a particular time.
//
// What: "@(#) ImposedMotionSP, revA"

#include <domain/constraints/ImposedMotionBase.h>

namespace XC {
class GroundMotion;
class Node;

//! @ingroup CContSP
//
//! @brief Movimiento impuesto sobre un nodo.
class ImposedMotionSP1 : public ImposedMotionBase
  {
  private:
    int destroyMotion; //!< flag indicating if destructor to be invoked on GMotion when done    
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    // constructors    
    ImposedMotionSP1(void);        
    ImposedMotionSP1(int spTag, int nodeTag, int ndof, int patternTag, int groundMotionTag);

    int applyConstraint(double loadFactor);    
    bool isHomogeneous(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif


