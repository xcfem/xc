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

// $Revision: 1.8 $
// $Date: 2003/02/25 23:33:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection3dBase.h,v $

// Written: fmk
// Created: 04/01
//
// Description: This file contains the class definition for
// FiberSection3dBase.h. FiberSection3dBase provides the abstraction of a
// 3d beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.

#ifndef FiberSection3dBase_h
#define FiberSection3dBase_h

#include <material/section/fiber_section/FiberSectionBase.h>
#include <material/section/repres/section/fiber_list.h>
#include <utility/matrix/Vector.h>

namespace XC {
class FiberSectionRepr;

//! @ingroup MATSCCFiberModel
//
//! @brief Base class for fiber sections
//! on three-dimensional problems.
class FiberSection3dBase : public FiberSectionBase
  {
  protected:

    friend class FiberPtrDeque;
    friend class FiberContainer;
    double get_strain(const double &y,const double &z) const;
  public:
    FiberSection3dBase(int classTag, int dim,MaterialHandler *mat_ldr= nullptr);
    FiberSection3dBase(int tag, int classTag, int dim,MaterialHandler *mat_ldr= nullptr);
    FiberSection3dBase(int tag, int classTag, int dim,const fiber_list &fibers,MaterialHandler *mat_ldr= nullptr);
    FiberSection3dBase(const FiberSection3dBase &);
    FiberSection3dBase &operator=(const FiberSection3dBase &);

    Fiber *addFiber(Fiber &)= 0;
    Fiber *addFiber(int tag,const MaterialHandler &,const std::string &nmbMat,const double &, const Vector &position);

    virtual double getStrain(const double &y,const double &z) const;

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    inline virtual double getCenterOfMassZ(void) const
      { return fibers.getCenterOfMassZ(); }


  };

} // end of XC namespace

#endif
