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
//BidimStrainLoad.h
                                                                        
#ifndef BidimStrainLoad_h
#define BidimStrainLoad_h

#include "BidimLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Load due to restricted material expansion or contraction on bidimensional elements.
class BidimStrainLoad: public BidimLoad
  {
  private:
    std::vector<Vector> strains; //!< Restricted strains on each Gauss point.	
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    BidimStrainLoad(int tag, const std::vector<Vector> &, const ID &theElementTags);
    BidimStrainLoad(int tag, const size_t &, const Vector &, const ID &theElementTags);
    BidimStrainLoad(int tag, const size_t &, const ID &theElementTags);
    BidimStrainLoad(int tag, const size_t &, const Vector &);
    BidimStrainLoad(int tag, const size_t &);
    BidimStrainLoad(const size_t &s= 4);
  
    inline const std::vector<Vector> &getStrains(void) const
      { return strains; }
    inline std::vector<Vector> &Strains(void)
      { return strains; }
    inline const Vector &getStrain(const size_t &i) const
      { return strains[i]; }
    inline Vector &Strain(const size_t &i)
      { return strains[i]; }
    void setStrains(const Matrix &);
    void setStrainComp(const size_t &,const size_t &,const double &);
    const Vector &getData(int &type, const double &loadFactor) const;

  
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;       

  };
} // end of XC namespace

#endif

