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
//ZeroLengthContact.h

#ifndef ZeroLengthContact_h
#define ZeroLengthContact_h

#include "domain/mesh/element/Element0D.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class Node;
class Channel;
class Response;

//! \ingroup ElemZL
//
//! @brief Elemento de longitud cero para problemas de contacto.
class ZeroLengthContact: public Element0D
  {
  protected:
    // contact forces
    mutable double pressure; // contact pressure at n+1
    double t1; //!< friction in local dir1
    double t2; //!< friction in local dir2

  

    // parameters
    mutable double gap; //!< gap of time n+1 step
    double gap_n; //!< gap of time n step
    double Kn; //!< normal penalty
    double Kt; //!< tangential penalty
    double fs; //!< friction ratio

    // Normal and Tangental Vectors for Elemental Nodes, (4*1)
    mutable Vector N;

    mutable int ContactFlag; //!< 0: not contact; 1: stick; 2: slide
  
    
  public:
    ZeroLengthContact(int tag= 0,int classTag= 0, int dim= 2);
    ZeroLengthContact(int tag, int classTag, int dim, int Nd1, int Nd2,double Kn, double Kt, double fRatio);

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &);

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &os, int flag =0);
  };

} // end of XC namespace


#endif











