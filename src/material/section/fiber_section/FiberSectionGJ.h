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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.3 $
// $Date: 2003/02/25 23:33:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSectionGJ.h,v $

// Written: fmk
// Created: 04/01
//
// Description: This file contains the class definition for
// FiberSectionGJ.h. FiberSectionGJ provides the abstraction of a
// 3d beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.

#ifndef FiberSectionGJ_h
#define FiberSectionGJ_h

#include <material/section/fiber_section/FiberSection3dBase.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class MaterialHandler;
class FiberSectionRepr;

//! @ingroup MATSCCFiberModel
//
//! @brief Fiber section with torsional stiffness.
class FiberSectionGJ: public FiberSection3dBase
  {
  private:
    double GJ;
  protected:
    friend class FiberDeque;
 
  public:
    FiberSectionGJ(MaterialHandler *mat_ldr= nullptr);
    FiberSectionGJ(int tag,MaterialHandler *mat_ldr= nullptr);
    FiberSectionGJ(int tag,const fiber_list &, double GJ = 1.0e10,MaterialHandler *mat_ldr= nullptr);

    Fiber *addFiber(Fiber &theFiber);

    int setInitialSectionDeformation(const Vector &deforms);
    int setTrialSectionDeformation(const Vector &deforms);

    virtual void setupFibers(void);

    inline const double &getGJ(void) const
      { return GJ; }
    const Matrix &getInitialTangent(void) const;

    int revertToLastCommit(void);
    int revertToStart(void);

    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder (void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag = 0);

  };

FiberSectionGJ FiberSectionReprToFiberSectionGJ(const int &tag,const FiberSectionRepr &fiberSectionRepr,const double &GJ);


} // end of XC namespace

#endif
