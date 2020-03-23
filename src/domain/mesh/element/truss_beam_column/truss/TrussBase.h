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
//TrussBase.h
                                                  
#ifndef TrussBase_h
#define TrussBase_h

#include "ProtoTruss.h"

namespace XC {
class Channel;
class UniaxialMaterial;

//! @ingroup TrussElem
//
//! @brief Base class for truss elements.
class TrussBase: public ProtoTruss
  {
  protected:
    double L;	    //!< length of truss based on undeformed configuration.
    double cosX[3]; //!< Cosenos directories.

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    void initialize(void);
    void setup_L_cos_dir(void);
    void set_load(const Vector &);
    void alloc_load(const size_t &);

  public:
    TrussBase(int classTag,int tag,int dimension, int Nd1, int Nd2);
    TrussBase(int classTag,int tag,int dimension);
    TrussBase(int classTag);
    TrussBase(const TrussBase &);
    TrussBase &operator=(const TrussBase &);

    const Vector *getLoad(void) const
      { return &load; }
    Vector *getLoad(void)
      { return &load; }
    const double &getL(void) const;
  };
} // end of XC namespace

#endif




