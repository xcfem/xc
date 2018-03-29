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

// $Revision: 1.20 $
// $Date: 2005/05/20 16:44:07 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection3d.cpp,v $

// Written: fmk
// Created: 04/04
//
// Description: This file contains the class implementation of XC::FiberSection2d.

#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <material/section/fiber_section/fiber/Fiber.h>
#include "classTags.h"
#include <material/section/fiber_section/FiberSection3d.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>

#include "material/section/ResponseId.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

//! @brief Constructor (it's used in FiberSectionShear3d).
XC::FiberSection3d::FiberSection3d(int tag,int classTag,MaterialHandler *mat_ldr)
  : FiberSection3dBase(tag, classTag,3,mat_ldr) {}

//! @brief Constructor.
XC::FiberSection3d::FiberSection3d(int tag,const fiber_list &fiberList,XC::MaterialHandler *mat_ldr):
  FiberSection3dBase(tag, SEC_TAG_FiberSection3d, 3,fiberList,mat_ldr)
  {fibers.setup(*this,fiberList,kr);}

//! @brief Constructor.
XC::FiberSection3d::FiberSection3d(int tag,MaterialHandler *mat_ldr)
  : FiberSection3dBase(tag, SEC_TAG_FiberSection3d,3,mat_ldr) {}

//! @brief Constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSection3d::FiberSection3d(XC::MaterialHandler *mat_ldr)
  : FiberSection3dBase(0, SEC_TAG_FiberSection3d,3,mat_ldr) {}

void XC::FiberSection3d::setupFibers(void)
  {
    if(section_repres)
      fibers.setup(*this,section_repres->get3DFibers(),kr);
    else
      fibers.updateKRCDG(*this,kr);
  }

//! @brief Adds a fiber to the section.
XC::Fiber *XC::FiberSection3d::addFiber(Fiber &newFiber)
  { return fibers.addFiber(*this,newFiber,kr); }


//! @brief Sets values for initial generalized strains.
int XC::FiberSection3d::setInitialSectionDeformation(const Vector &deforms)
  {
    FiberSectionBase::setInitialSectionDeformation(deforms);
    return fibers.setInitialSectionDeformation(*this);
  }

//! @brief Set trial strains.
int XC::FiberSection3d::setTrialSectionDeformation(const Vector &deforms)
  {
    FiberSection3dBase::setTrialSectionDeformation(deforms);
    return fibers.setTrialSectionDeformation(*this,kr);
  }

//! @brief Return the tangent initial stiffness matrix.
const XC::Matrix &XC::FiberSection3d::getInitialTangent(void) const
  { return fibers.getInitialTangent(*this); }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::FiberSection3d::getCopy(void) const
  { return new FiberSection3d(*this); }

//! @brief Returns the tipo.
const XC::ResponseId &XC::FiberSection3d::getType(void) const
  { return RespFiberSection3d; }

int XC::FiberSection3d::getOrder(void) const
  { return 3; }

//! @brief Returns to the last commited state.
int XC::FiberSection3d::revertToLastCommit(void)
  {
    FiberSection3dBase::revertToLastCommit();
    return fibers.revertToLastCommit(*this,kr);
  }

//! @brief Returns to the initial state.
int XC::FiberSection3d::revertToStart(void)
  { return fibers.revertToStart(*this,kr); }

int XC::FiberSection3d::sendSelf(CommParameters &cp)
  {
    int res= 0;
    std::cerr << "FiberSection3d::sendSelf - not implemented.\n";
    return res;
  }

int XC::FiberSection3d::recvSelf(const CommParameters &cp)
  {
    int res= 0;
    std::cerr << "FiberSection3d::recvSelf - not implemented.\n";
    return res;
  }

void XC::FiberSection3d::Print(std::ostream &s, int flag)
  {
    if(flag == 2)
      fibers.Print(s,flag);
    else
      {
        s << "\nFiberSection3d, tag: " << getTag() << std::endl;
        s << "\tSection code: " << getType();

        if(flag == 1)
          fibers.Print(s,flag);
      }
  }

XC::FiberSection3d XC::FiberSectionReprToFiberSection3d(const int &tag,const FiberSectionRepr &fiberSectionRepr)
  { return fiberSectionRepr.getFiberSection3d(tag); }
