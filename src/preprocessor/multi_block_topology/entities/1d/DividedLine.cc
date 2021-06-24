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
//DividedLine.cc

#include "DividedLine.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/pos_vec/Pos3dArray.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::DividedLine::DividedLine(Preprocessor *m,const size_t &nd)
  : Line(m,nd), lengths(nd,1.0){}

//! @brief Comparison operator.
bool XC::DividedLine::operator==(const DividedLine &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Line::operator==(other);
        if(retval)
          retval= (lengths==other.lengths);
       } 
    return retval;
  }

//! @brief Virtual constructor.
XC::SetEstruct *XC::DividedLine::getCopy(void) const
  { return new DividedLine(*this); }

//! @brief assigns lengths.
void XC::DividedLine::setLengths(const Vector &ls)
  {
    lengths= vector_to_std_vector(ls);
    setNDiv(lengths.size());
  }

//! @brief Return ndiv+1 positions along the line spaced
//! according to the lengths specifiend in the lengths member.
Pos3dArray XC::DividedLine::get_positions(void) const
  {
    static Pos3dArray retval;
    if(P1() && P2())
      {
	std::vector<GEOM_FT> tmp(NDiv(),0);
        for(size_t i= 0;i<NDiv();i++)
          tmp[i]= double_to_FT(lengths[i]);
        retval= Pos3dArray(P1()->getPos(),P2()->getPos(),tmp);
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; undifined line." << std::endl;
    return retval;
  }

