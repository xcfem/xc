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
//AggregatorAdditions.h
//deque de pointers (se emplear en la clase Set).


#ifndef AGGREGATORADDITIONS_H
#define AGGREGATORADDITIONS_H

#include <material/uniaxial/DqUniaxialMaterial.h>
#include "material/section/ResponseId.h"

namespace XC {

class ResponseId;

//! @ingroup MATSCC
//
//! @brief pointers to UniaxialMaterial with the degree
//! of freedom associated to each of them (used in SectionAggregator).
class AggregatorAdditions: public DqUniaxialMaterial
  {
    ResponseId matCodes; //!< Response for each material.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    AggregatorAdditions(EntCmd *owner= nullptr);
    AggregatorAdditions(EntCmd *owner,const UniaxialMaterial &,int c);
    AggregatorAdditions(const AggregatorAdditions &);
    AggregatorAdditions(const AggregatorAdditions &,SectionForceDeformation *s);
    ~AggregatorAdditions(void);

    inline bool check_ptrs(void) const
      { return !matCodes.isEmpty(); }

    void putMatCodes(const ResponseId &codes);

    void getType(ResponseId &retval,const size_t &offset) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;

  };

} //end of XC namespace
#endif
