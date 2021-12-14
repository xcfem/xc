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

// $Revision: 1.6 $
// $Date: 2005/03/25 00:32:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/fiber_section/FiberSectionRepr.cpp,v $


// File: FiberSectionRepr.C
// Written by Remo M. de Souza
// November 1998

#include <material/section/repres/section/FiberSectionRepr.h>
#include <material/section/repres/section/FiberData.h>

#include "material/section/fiber_section/fiber/Fiber.h"

#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "material/section/fiber_section/FiberSectionGJ.h"


#include "utility/geom/d2/2d_polygons/Polygon2d.h"

void XC::FiberSectionRepr::clear_fibers(void)
  {
    fibers.erase(fibers.begin(),fibers.end());// NOTE: don't delete fiber objects themselves
                                              // leave this to FiberSection destructor
  }

void XC::FiberSectionRepr::copy_fibers(const FiberSectionRepr &other)
  {
    clear_fibers();
    for(fiber_list::const_iterator i= other.fibers.begin();i!=other.fibers.end();i++)
      addFiber(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::FiberSectionRepr::FiberSectionRepr(int sectionID,MaterialHandler *ml)
  :SectionRepres(sectionID, ml), sectID(sectionID) {}

//! @brief Copy constructor.
XC::FiberSectionRepr::FiberSectionRepr(const FiberSectionRepr &other)
  :SectionRepres(other), sectID(other.sectID)
  { copy_fibers(other); }

//! @brief Assignment operator.
XC::FiberSectionRepr &XC::FiberSectionRepr::operator=(const FiberSectionRepr &other)
  {
    SectionRepres::operator=(other);
    sectID= other.sectID;
    copy_fibers(other);
    return *this;
  }

XC::FiberSectionRepr *XC::FiberSectionRepr::getCopy(void) const
  { return new FiberSectionRepr(*this); }


XC::FiberSectionRepr::~FiberSectionRepr(void)
  { clear_fibers(); }


#define SEC_TAG_FiberSectionRepr 1

int XC::FiberSectionRepr::getType(void) const
  { return SEC_TAG_FiberSectionRepr; }



void XC::FiberSectionRepr::Print(std::ostream &s, int flag) const
  {
    SectionRepres::Print(s,flag);
  }


std::ostream &XC::operator<<(std::ostream &s, XC::FiberSectionRepr &fiberSectionRepr)
  {
    fiberSectionRepr.Print(s);
    return s;
  }


int XC::FiberSectionRepr::addFiber(Fiber &newFiber)
  {
    fibers.push_back(&newFiber);
    return 0;
  }

//! @brief Returns the number of fibers of the section.
int XC::FiberSectionRepr::getNumFibers(void) const
  { return fibers.size(); }

//! @brief Returns cells and rebars fiber data.
XC::FiberData XC::FiberSectionRepr::getFiberData(void) const
  {
    assert(gmSecc);
    return FiberData(*gmSecc);
  }

//! @brief Returns a fiber container populated with UniaxialFiber2d objects.
XC::fiber_list XC::FiberSectionRepr::get2DFibers(void) const
  {
    fiber_list retval;
    if(!material_handler)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; material handler not defined.\n";
        return retval;
      }
    for(fiber_list::const_iterator i= fibers.begin();i!=fibers.end();i++)
      retval.push_back(*i);

    FiberData fiberData= getFiberData();
    fiberData.get2DFibers(retval);
    return retval;
  }

//! @brief Returns a fiber container populated with UniaxialFiber3d objects.
XC::fiber_list XC::FiberSectionRepr::get3DFibers(void) const
  {
    fiber_list retval;
    if(!material_handler)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; material handler not defined.\n";
        return retval;
      }
    for(fiber_list::const_iterator i= fibers.begin();i!=fibers.end();i++)
      retval.push_back(*i);

    FiberData fiberData= getFiberData();
    fiberData.get3DFibers(retval);
    return retval;
  }

XC::FiberSection2d XC::FiberSectionRepr::getFiberSection2d(int secTag) const
  { return FiberSection2d(secTag,fibers,material_handler); }

XC::FiberSection3d XC::FiberSectionRepr::getFiberSection3d(int secTag) const
  { return FiberSection3d(secTag,fibers,material_handler); }

XC::FiberSectionGJ XC::FiberSectionRepr::getFiberSectionGJ(int secTag,const double &GJ) const
  { return FiberSectionGJ(secTag,fibers,GJ,material_handler); }

