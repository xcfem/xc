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
//ConstraintLoader.h

#ifndef CONSTRAINTLOADER_H
#define CONSTRAINTLOADER_H

#include "Loader.h"

namespace XC {

class SFreedom_Constraint;
class MFreedom_Constraint;
class MRMFreedom_Constraint;

//! \ingroup Ldrs
//! 
//! @brief Constraint cration tools
//!  
class ConstraintLoader: public Loader
  {
    int tag_sp_constraint; //!< Default value for next single freedom constraint tag.
    int tag_mp_constraint; //!< Default value for next multiple freedom constraint tag.
    int tag_mrmp_constraint; //!< Default value for next multi-row multi-freedom constraint tag.
  protected:

    friend class Node;
    friend class SetMeshComp;
    SFreedom_Constraint *addSFreedom_Constraint(const int &,const SFreedom_Constraint &);
    SFreedom_Constraint *addSFreedom_Constraint(const int &,const int &,const double &);
  public:
    ConstraintLoader(Preprocessor *owr);
    virtual ~ConstraintLoader(void);
    inline const int &getTagNextSPConstraint(void) const
      { return tag_sp_constraint; }
    inline const int &getTagNextMPConstraint(void) const
      { return tag_mp_constraint; }
    inline const int &getTagNextMRMPConstraint(void) const
      { return tag_mrmp_constraint; }
    int getNumSPs(void) const;
    int getNumMPs(void) const;
    int getNumMRMPs(void) const;
    int getNumLPs(void) const;
    SFreedom_Constraint *newSPConstraint(const int &,const int &,const double &);
    void removeSPConstraint(const int &tagC);
    MFreedom_Constraint *newMPConstraint(const int &, const int &, const ID &, const ID &);
    MFreedom_Constraint *newEqualDOF(const int &, const int &, const ID &);
    MFreedom_Constraint *newRigidBeam(const int &, const int &);
    MFreedom_Constraint *newRigidRod(const int &, const int &);
    //MFreedom_Constraint *newRigidDiaphragm(void);
    MRMFreedom_Constraint *newMRMPConstraint(const ID &, const int &, const ID &);
    MRMFreedom_Constraint *newGlueNodeToElement(const Node &, const Element &, const ID &);
    void clearAll(void);
  };

} // end of XC namespace

#endif
