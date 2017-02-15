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
//BeamPointLoad.h
                                                                        
#ifndef BeamPointLoad_h
#define BeamPointLoad_h

#include "domain/load/beam_loads/BeamMecLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Punctual load over beam elements.
class BeamPointLoad: public BeamMecLoad
  {
  protected:
    double x; //!< relative distance (x/L) along length from end 1 of element

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    BeamPointLoad(int tag, int classTag, double Pt, double x,const ID &theElementTags, double Pa = 0.0);
    BeamPointLoad(int tag, int classTag);

    inline double X(void) const
      { return x; }
    inline void setX(const double &X)
      { x= X; } 
    
    std::string Categoria(void) const;


  };
} // end of XC namespace

#endif

