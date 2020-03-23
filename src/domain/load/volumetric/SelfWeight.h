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
                                                                        
#ifndef SelfWeight_h
#define SelfWeight_h

// Written: Chris McGann, U.Washington
//          02.2011
//
// Description: This file contains the class definition for SelfWeight, a load class for 
//               applying body forces inside load patterns for continuum elements.
#include <domain/load/ElementBodyLoad.h>

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Self weight.
class SelfWeight: public ElementBodyLoad
  {
  private:
    static Vector data;
    double xFact;
    double yFact;
    double zFact;
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    SelfWeight(int tag, double xf, double yf, double zf, const ID &theElementTags);
    SelfWeight(int tag);
    SelfWeight(void);    

    const Vector &getData(int &type, const double &loadFactor) const;
    inline const double &getXFact(void) const
      { return xFact; }
    inline const double &getYFact(void) const
      { return yFact; }
    inline const double &getZFact(void) const
      { return zFact; }

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;       
  };
} // end of XC namespace

#endif

