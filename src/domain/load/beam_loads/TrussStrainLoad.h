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
//TrussStrainLoad.h
                                                                        
#ifndef TrussStrainLoad_h
#define TrussStrainLoad_h

#include "domain/load/ElementBodyLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Load due to restricted material expansion or contraction on truss elements.
class TrussStrainLoad: public ElementBodyLoad
  {
  private:
    double e1; //!< Restricted strain in node 1	
    double e2; //!< Restricted strain in node 2
  protected:

  public:
  // Constructors based on 2 temperature changes given
    TrussStrainLoad(int tag, const double &e1, const double &e2, const ID &theElementTags);
    TrussStrainLoad(int tag, const double &e_uniform, const ID &theElementTags);
    TrussStrainLoad(int tag, const ID &theElementTags);
    TrussStrainLoad(int tag);
    TrussStrainLoad(void);    
  
    inline const double &E1(void) const
      { return e1; }
    inline void setE1(const double &d)
      { e1= d; }
    inline const double &E2(void) const
      { return e2; }
    inline void setE2(const double &d)
      { e2= d; }
    const Vector &getData(int &type, const double &loadFactor) const;

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;       

  };
} // end of XC namespace

#endif

