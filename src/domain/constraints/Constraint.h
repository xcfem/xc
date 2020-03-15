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
//Constraint.h                                                                        
                                                                        
#ifndef Constraint_h
#define Constraint_h

#include "domain/component/ContinuaReprComponent.h"


namespace XC {
//! @ingroup Dom
//
//!
//! @defgroup BoundCond Boundary conditions.
//
//! @ingroup BoundCond
//
//!
//! @defgroup CCont Essential boundary conditions (displacements).
//
//! @ingroup CCont
//
//! @brief Base class for model constraints.
class Constraint: public ContinuaReprComponent
  {
  protected:
    int constrNodeTag; //!< constrained node tag

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // constructors
    Constraint(int classTag);        
    Constraint(int spTag, int nodeTag, int classTag);
    ~Constraint(void);

    void setNodeTag(const int &nt);
    virtual int getNodeTag(void) const;
    virtual const Node *getNode(void) const;
    virtual int getNodeIdx(void) const;
    virtual int applyConstraint(double loadFactor)= 0;    
    virtual bool affectsNode(int ) const;
    virtual bool affectsNodeAndDOF(int , int ) const;

    void setDomain(Domain *);

    virtual int getVtkCellType(void) const;

    virtual void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif


