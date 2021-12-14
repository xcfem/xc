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
//----------------------------------------------------------------------------
                                                                        
#ifndef ElementBodyLoad_h
#define ElementBodyLoad_h

#include "ElementalLoad.h"
#include "ElementPtrs.h"

namespace XC {
class Element;
 class ID;
 class Vector;
 

//! @ingroup ElemLoads
//
//! @brief Base class for body loads over elements.
class ElementBodyLoad: public ElementalLoad
  {
  private:
    ElementPtrs theElements; //!< Loaded elements.
  protected:

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

  public:
    ElementBodyLoad(int classTag);    
    ElementBodyLoad(int tag, int classTag);
    ElementBodyLoad(int tag, int classTag, const ID &theElementTags);

    virtual void setDomain(Domain *theDomain);
    virtual void applyLoad(double loadfactor);

    virtual int removeElement(int tag);
    void Print(std::ostream &s, int flag =0) const;       
  };
} // end of XC namespace

#endif

