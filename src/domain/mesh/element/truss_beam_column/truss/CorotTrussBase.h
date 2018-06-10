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
//CorotTrussBase.h

#ifndef CorotTrussBase_h
#define CorotTrussBase_h

#include "ProtoTruss.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
//! @ingroup TrussElem
//
//! @brief Base class for corotational truss elements.
class CorotTrussBase: public ProtoTruss
  {
  protected:
    double Lo;	        //!< initial length of truss
    double Ln;		//!< current length of truss
    double d21[3];	//!< current displacement offsets in basic system

    Matrix R;//!< Rotation matrix

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    CorotTrussBase(int tag, int classTag, int dim,int Nd1, int Nd2);
    CorotTrussBase(int tag, int classTag,int dimension);
    CorotTrussBase(const CorotTrussBase &);
    CorotTrussBase &operator=(const CorotTrussBase &);
    const double &getLo(void) const;
    const double &getLn(void) const;
  };
} // end of XC namespace

#endif




