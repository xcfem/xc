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
//EQBasePattern.h

#ifndef EQBasePattern_h
#define EQBasePattern_h

#include "domain/load/pattern/LoadPattern.h"
#include "domain/load/groundMotion/DqGroundMotions.h"

namespace XC {
class GroundMotion;
class Vector;

//! @ingroup LPatterns
//
//! @brief Earthquake load pattern.
class EQBasePattern: public LoadPattern
  {
  protected:
    DqGroundMotions theMotions;

    int addMotion(GroundMotion &theMotion);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    EQBasePattern(int tag, int classTag);

    inline size_t getNumMotions(void) const
      { return theMotions.getNumGroundMotions(); }

    void clear(void)
      { theMotions.clear(); }

    virtual void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif
