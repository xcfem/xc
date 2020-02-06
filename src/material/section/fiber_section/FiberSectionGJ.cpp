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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/25 23:33:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSectionGJ.cpp,v $
                                                                        
// Written: fmk
// Created: 04/04
//
// Description: This file contains the class implementation of FiberSectionGJ.

#include <cstdlib>

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <material/section/fiber_section/fiber/Fiber.h>
#include <classTags.h>
#include <material/section/fiber_section/FiberSectionGJ.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>

#include "material/section/ResponseId.h"
#include "xc_utils/src/geom/d2/2d_polygons/Polygon2d.h"

//! @brief Constructor.
XC::FiberSectionGJ::FiberSectionGJ(int tag,const fiber_list &fiberList, double gj,XC::MaterialHandler *mat_ldr)
  : FiberSection3dBase(tag, SEC_TAG_FiberSectionGJ,4,fiberList,mat_ldr), GJ(gj)
  { fibers.setup(*this,fiberList,kr); }

XC::FiberSectionGJ::FiberSectionGJ(int tag,MaterialHandler *mat_ldr)
  : FiberSection3dBase(tag, SEC_TAG_FiberSectionGJ,4,mat_ldr),GJ(1.0) {}

//! @brief Constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSectionGJ::FiberSectionGJ(MaterialHandler *mat_ldr):
  FiberSection3dBase(0,SEC_TAG_FiberSectionGJ,4,mat_ldr), GJ(1.0) {}

void XC::FiberSectionGJ::setupFibers(void)
  {
    if(section_repres)
      fibers.setup(*this,section_repres->get3DFibers(),kr);
    else
      fibers.updateKRCenterOfMass(*this,kr);
  }

//! @brief Adds a fiber to the section.
XC::Fiber *XC::FiberSectionGJ::addFiber(Fiber &newFiber)
  { return fibers.addFiber(*this,newFiber,kr); }

//! @brief Set the initial strains.
int XC::FiberSectionGJ::setInitialSectionDeformation(const Vector &deforms)
  {
    FiberSection3dBase::setInitialSectionDeformation(deforms);
    return fibers.setInitialSectionDeformation(*this);
  }

//! @brief Sets trial generalized strains values.
int XC::FiberSectionGJ::setTrialSectionDeformation(const XC::Vector &deforms)
  {
    FiberSection3dBase::setTrialSectionDeformation(deforms);
    return fibers.setTrialSectionDeformation(*this,kr);
  }

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberSectionGJ::getInitialTangent(void) const
  { return fibers.getInitialTangent(*this);  }

XC::SectionForceDeformation *XC::FiberSectionGJ::getCopy(void) const
  { return new FiberSectionGJ(*this); }

const XC::ResponseId &XC::FiberSectionGJ::getType(void) const
  { return RespElasticSection3d; }

int XC::FiberSectionGJ::getOrder(void) const
  { return kr.dim(); }

//! @brief Returns to last committed state.
int XC::FiberSectionGJ::revertToLastCommit(void)
  {
    FiberSection3dBase::revertToLastCommit();
    return fibers.revertToLastCommit(*this,kr);
  }

//! @brief Return to the initial state.
int XC::FiberSectionGJ::revertToStart(void)
  { return fibers.revertToStart(*this,kr); }

int XC::FiberSectionGJ::sendSelf(CommParameters &cp)
  {
    int res= 0;
    return res;
  }

int XC::FiberSectionGJ::recvSelf(const CommParameters &)
  {
    int res= 0;
    return res;
  }

void XC::FiberSectionGJ::Print(std::ostream &s, int flag)
  {
    s << "\nFiberSectionGJ, tag: " << this->getTag() << std::endl;
    s << "\tSection code: " << getType();
    s << "\tTorsional Stiffness: " << GJ << std::endl;

    if(flag == 1)
      fibers.Print(s,flag);
  }

XC::FiberSectionGJ XC::FiberSectionReprToFiberSectionGJ(const int &tag,const XC::FiberSectionRepr &fiberSectionRepr,const double &GJ)
  { return fiberSectionRepr.getFiberSectionGJ(tag,GJ); }
