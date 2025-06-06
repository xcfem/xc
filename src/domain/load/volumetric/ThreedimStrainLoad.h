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
//ThreedimStrainLoad.h
                                                                        
#ifndef ThreedimStrainLoad_h
#define ThreedimStrainLoad_h

#include "ThreedimLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Load due to restricted material expansion or contraction on bidimensional elements.
class ThreedimStrainLoad: public ThreedimLoad
  {
  private:
    std::vector<Vector> strains; //!< Imposed strains on each Gauss point.	
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

  public:
    ThreedimStrainLoad(const size_t &sz= 8);
    ThreedimStrainLoad(int tag, const size_t &);
    ThreedimStrainLoad(int tag, const size_t &, const Vector &);
    ThreedimStrainLoad(int tag, const size_t &, const ID &theElementTags);
    ThreedimStrainLoad(int tag, const std::vector<Vector> &, const ID &theElementTags);
    ThreedimStrainLoad(int tag, const size_t &, const Vector &, const ID &theElementTags);
  
    std::string Category(void) const;
    
    inline const std::vector<Vector> &getStrains(void) const
      { return strains; }
    Matrix getElementStrainsMatrix(const Element &) const;
    inline std::vector<Vector> &Strains(void)
      { return strains; }
    inline const Vector &getStrain(const size_t &i) const
      { return strains[i]; }
    inline Vector &Strain(const size_t &i)
      { return strains[i]; }
    void setStrains(const Matrix &);
    void setStrainComp(const size_t &,const size_t &,const double &);
    boost::python::list getStrainsPy(void) const;
    void setStrainsPy(const boost::python::list &);
    const Vector &getData(int &type, const double &loadFactor) const;

  
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;       

  };
} // end of XC namespace

#endif

