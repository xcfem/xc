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
//beam2d.h

#ifndef beam2d_h
#define beam2d_h

#include <domain/mesh/element/truss_beam_column/ProtoBeam2d.h>

namespace XC {
class CrdTransf2d;

//! \ingroup OneDimensionalElem
//
//! @brief 2D beam element.
class beam2d : public ProtoBeam2d
  {
  protected:
    // protected attributes - a copy for each object of the class        
    mutable double L;
    mutable double sn;
    mutable double cs;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    beam2d(int tag, int class_tag);
    beam2d(int tag, int class_tag, double A, double E, double I, int Nd1, int Nd2);    
  };
} // end of XC namespace

#endif

