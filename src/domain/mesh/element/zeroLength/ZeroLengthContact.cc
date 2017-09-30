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
//ZeroLengthContact.cpp

#include "ZeroLengthContact.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/recorder/response/ElementResponse.h"
#include "domain/mesh/node/Node.h"


//! @brief Constructor.
XC::ZeroLengthContact::ZeroLengthContact(int tag, int classTag, int dim, int Nd1, int Nd2,double Knormal, double Ktangent, double frictionRatio)
  :Element0D(tag,classTag,Nd1,Nd2,dim), N(dim*numNodos())
  {
    // assign Kn, Kt, fs
    Kn = Knormal;
    Kt = Ktangent;
    fs = frictionRatio;

    // initialized contact flag be zero
    ContactFlag=0;

    gap_n  = 0 ;
    pressure=0; // add for augmented lagrange
  }

//! @brief Default constructor.
XC::ZeroLengthContact::ZeroLengthContact(int tag,int classTag, int dim)
  :Element0D(tag,classTag,0,0,dim),
   N(dim*numNodos())
  {}

int XC::ZeroLengthContact::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    // meaningless to addLoad to a contact !
    if(isDead())
      std::cerr << getClassName() 
                << "; load over inactive element: "
                << getTag() << std::endl;
    return 0;
  }

int XC::ZeroLengthContact::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // does nothing as element has no mass yet!
    return 0;
  }

int XC::ZeroLengthContact::sendSelf(CommParameters &cp)
  {
    // doing nothing here
    return 0;
  }

int XC::ZeroLengthContact::recvSelf(const CommParameters &cp)
  {
    // doing nothing here
    return 0;
  }


void XC::ZeroLengthContact::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      { // print everything
        s << "Element: " << this->getTag()
          << " type: ZeroLengthContact  Nodes: "
          << theNodes << std::endl;
      }
    else if(flag == 1)
      {
        s << this->getTag() << "  ";
      }
  }

