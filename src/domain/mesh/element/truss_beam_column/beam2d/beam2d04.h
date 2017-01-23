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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/14 23:01:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam2d/beam2d04.h,v $
                                                                        
                                                                        
// File: ~/element/beam2d04.h
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for beam2d04.
// beam2d04 is a plane frame member.

#ifndef beam2d04_h
#define beam2d04_h

#include "beam2d.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Channel;

//! \ingroup OneDimensionalElem
//
//! @brief Elemento barra 04 para problemas bidimensionales.
class beam2d04 : public beam2d
  {
  private:    
    mutable double oneEA;
    mutable double twoEI; 
    mutable double fourEI;
    mutable double twelveEI;
    mutable double sixEI;
    
    mutable Vector rForce;
    mutable int isStiffFormed;

    static Matrix k;
    static Matrix trans;  // hold part of transformation matrix

    const Matrix &getStiff(void) const;    
    void formVar(void) const;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    beam2d04(int tag= 0);        
    beam2d04(int tag, double A, double E, double I, int Nd1, int Nd2);
    Element *getCopy(void) const;

    int revertToLastCommit(void);        
    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;

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


