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
                                                                        
#ifndef BrickSelfWeight_h
#define BrickSelfWeight_h

// Written: ZHYang, UCDavis

// Purpose: This file contains the class definition for 8 node brick self weight load.

#include <domain/load/ElementBodyLoad.h>

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Hexahedron self weight.
class BrickSelfWeight: public ElementBodyLoad
  {
  private:
    static Vector data;
  protected:
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

  public:
    BrickSelfWeight(int tag, const ID &theElementTags);
    BrickSelfWeight(int tag);
    BrickSelfWeight(void);    

    const Vector &getData(int &type, const double &loadFactor) const;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;       
  };
} // end of XC namespace

#endif

