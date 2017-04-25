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
#include "Pnt.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::DividedLine::DividedLine(Preprocessor *m,const size_t &nd)
  : Linea(m,nd), longs(nd,1.0){}

//! @brief Virtual constructor.
XC::SetEstruct *XC::DividedLine::getCopy(void) const
  { return new DividedLine(*this); }

void XC::DividedLine::setLongs(const XC::Vector &ls)
  {
    longs= vector_to_std_vector(ls);
    SetNDiv(longs.size());
  }

//! @brief Return ndiv+1 positions along the line spaced
//! according to the lengths specifiend in the longs member.
MatrizPos3d XC::DividedLine::get_posiciones(void) const
  {
    static MatrizPos3d retval;
    if(P1() && P2())
      {
	std::vector<GEOM_FT> tmp(NDiv(),0);
        for(size_t i= 0;i<NDiv();i++)
          tmp[i]= double_to_FT(longs[i]);
        retval= MatrizPos3d(P1()->GetPos(),P2()->GetPos(),tmp);
      }
    else
      std::cerr << nombre_clase() << __FUNCTION__
		<< "; undifined line." << std::endl;
    return retval;
  }

