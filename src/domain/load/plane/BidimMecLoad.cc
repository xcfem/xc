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
//BidimMecLoad.cpp

#include "BidimMecLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"
#include "utility/actor/actor/MovableVector.h"
#include "domain/mesh/node/Node.h"
#include "xc_utils/src/geom/pos_vec/SlidingVectorsSystem3d.h"

XC::BidimMecLoad::BidimMecLoad(int tag,int classTag,const double &wt,const double &wa1,const double &wa2,const ID &theElementTags)
  :BidimLoad(tag, classTag, theElementTags), Trans(wt), Axial1(wa1), Axial2(wa2) {}

XC::BidimMecLoad::BidimMecLoad(int tag,int classTag)
  :BidimLoad(tag, classTag), Trans(0.0), Axial1(0.0), Axial2(0.0) {}

void XC::BidimMecLoad::Print(std::ostream &s, int flag) const
  {
    s << "BidimMecLoad - Reference load" << std::endl;
    s << "  Transverse: " << Trans << std::endl;
    s << "  Axial1:      " << Axial1 << std::endl;
    s << "  Axial2:      " << Axial2 << std::endl;
    BidimLoad::Print(s,flag);
  }

//! brief Returns load resultant (force and moment integration over the elements).
SlidingVectorsSystem3d XC::BidimMecLoad::getResultant(const Pos3d &centro, bool initialGeometry) const
  {
    std::cerr << getClassName()
              << "::getResultant not yet implemented." << std::endl;
    return SlidingVectorsSystem3d(centro);
  }

//! @brief Send data through the channel being passed as parameter.
int XC::BidimMecLoad::sendData(CommParameters &cp)
  {
    int res= BidimLoad::sendData(cp);
    res+= cp.sendDoubles(Trans,Axial1,Axial2,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::BidimMecLoad::recvData(const CommParameters &cp)
  {
    int res= BidimLoad::recvData(cp);
    res+= cp.receiveDoubles(Trans,Axial1,Axial2,getDbTagData(),CommMetaData(5));
    return res;
  }
