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
//TrussPrestressLoad.h
                                                                        
#ifndef TrussPrestressLoad_h
#define TrussPrestressLoad_h

#include "domain/load/ElementBodyLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Load due to presstressing.
class TrussPrestressLoad: public ElementBodyLoad
  {
  private:
    double sg1; //!< Prestressing stress in node 1	
    double sg2; //!< Prestressing stress in node 2
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    // Constructors based on 2 temperature changes given
    TrussPrestressLoad(int tag, const double &sg1, const double &sg2, const ID &theElementTags);
    TrussPrestressLoad(int tag, const double &e_uniform, const ID &theElementTags);
    TrussPrestressLoad(int tag, const ID &theElementTags);
    TrussPrestressLoad(int tag);
    TrussPrestressLoad(void);    
  
    std::string Category(void) const;
  
    inline const double &getSigma1(void) const
      { return sg1; }
    inline void setSigma1(const double &d)
      { sg1= d; }
    inline const double &getSigma2(void) const
      { return sg2; }
    inline void setSigma2(const double &d)
      { sg2= d; }
    const Vector &getData(int &type, const double &loadFactor) const;
    std::vector<XC::Vector> getStresses(void) const;
    Matrix getElementStressesMatrix(const Element &) const;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;       

  };
} // end of XC namespace

#endif

