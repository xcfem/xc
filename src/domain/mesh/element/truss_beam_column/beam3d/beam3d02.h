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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/25 23:32:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam3d/beam3d02.h,v $
                                                                        
                                                                        
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for beam3d02.
// beam3d02 is a plane frame member.

#ifndef beam3d02_h
#define beam3d02_h

#include "beam3dBase.h"

namespace XC {

//! @ingroup OneDimensionalElem
//
//! @brief 3D beam 02 element.
class beam3d02: public beam3dBase
  {
  private:
    double dx,dy,dz,EA,twoE,fourE,twelveE,sixE;
    const Matrix &getStiff(void) const;    
    void formVar(void);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);	
  public:
    beam3d02(int tag= 0);        
    beam3d02(int tag, double A, double E, double G,double Jx, double Iy, double Iz, int Nd1, int Nd2, double theta);
    Element *getCopy(void) const;

    void setDomain(Domain *theDomain);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);    
  };
} // end of XC namespace

#endif


