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
//ShellRawLoad.h
                                              
#ifndef ShellRawLoad_h
#define ShellRawLoad_h

#include "ShellMecLoad.h"

namespace XC {

//! @ingroup ElemLoads
//
//! @brief Uniform load over shell elements.
class ShellRawLoad: public ShellMecLoad
  {
  protected:
    std::vector<Vector> nodalLoads; //!< Load in each node.

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
    
    DbTagData &getDbTagData(void) const;
  public:
    ShellRawLoad(int tag= 0);
    ShellRawLoad(int tag, const std::vector<Vector> &, const ID &);

    std::string Category(void) const;

    const Vector &getData(int &type, const double &loadFactor) const;

    virtual Vector getLocalForce(void) const;
    virtual Vector getLocalMoment(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    void addReactionsInBasicSystem(const std::vector<double> &,const double &,FVectorShell &) const;
    void addFixedEndForcesInBasicSystem(const std::vector<double> &,const double &loadFactor,FVectorShell &) const;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

