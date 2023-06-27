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

// Written: Chris McGann, U.Washington
//          02.2011
//
// Description: This file contains the class definition for SurfaceLoad.

#ifndef SurfaceLoad_h
#define SurfaceLoad_h

#include "ElementBodyLoad.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup ElemLoads
//
//! @brief Load due to a pressure on the element surface.
class SurfaceLoad: public ElementBodyLoad
  {
  private:
    double pressure; //!< Pressure on the surface.
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    SurfaceLoad(int tag= 0, const double &pressure= 0.0);
    SurfaceLoad(int tag,const ID &tags_elems, const double &pressure= 0.0);

    const Vector &getData(int &type, const double &loadFactor) const;
    
    double getPressure(void) const;
    void setPressure(const double &);

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    void Print(std::ostream &, int flag = 0);
  };
} // end of XC namespace

#endif
