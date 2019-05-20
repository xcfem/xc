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
//FiberContainer.cc

#include "FiberContainer.h"
#include "material/section/fiber_section/fiber/Fiber.h"
#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "xc_utils/src/geom/d2/2d_polygons/Polygon2d.h"

//! @brief Allocates memory for each fiber material and for its data;
//! two (yLoc,Area) for 2D sections (getOrder()= 2) and three (yLoc,zLoc,Area) for 3D sections (getOrder()= 3).
void XC::FiberContainer::allocFibers(int numOfFibers,const Fiber *sample)
  {
    free_mem();
    if(numOfFibers)
      {
        resize(numOfFibers);
        if(sample)
          for(int i= 0;i<numOfFibers;i++)
            (*this)[i]= sample->getCopy();
      }
  }

//! @brief Copy the fibers from te container into this object.
void XC::FiberContainer::copy_fibers(const FiberContainer &other)
  {
    free_mem();
    const size_t numFibers= other.getNumFibers();
    if(numFibers)
      {
        allocFibers(numFibers);
        for(register size_t i= 0;i<numFibers;i++)
          (*this)[i]= other[i]->getCopy();
      }
  }

//! @brief frees memory
void XC::FiberContainer::free_mem(void)
  {
    const size_t numFibers= getNumFibers();
    for(register size_t i= 0;i<numFibers;i++)
      if((*this)[i])
        {
          delete (*this)[i];
          (*this)[i]= nullptr;
        }
    clear();
  }

//! @brief Default constructor.
XC::FiberContainer::FiberContainer(const size_t &num)
  : FiberPtrDeque(num) {}

//! @brief Copy constructor.
XC::FiberContainer::FiberContainer(const FiberContainer &other)
  : FiberPtrDeque() //Don't copy pointers
  { copy_fibers(other); }

//! @brief Assignment operator.
XC::FiberContainer &XC::FiberContainer::operator=(const FiberContainer &other)
  {
    CommandEntity::operator=(other); //Don't copy pointers
    copy_fibers(other); //They are copied here.
    return *this;
  }

//! @brief Copy the fibers from the container being passed as parameter.
void XC::FiberContainer::copy_fibers(const fiber_list &fibers)
  {
    const size_t numFibers= fibers.size();
    allocFibers(numFibers);
    int i= 0;
    for(fiber_list::const_iterator ifib=fibers.begin();ifib!=fibers.end(); ifib++,i++)
      {
        (*this)[i]= (*ifib)->getCopy();
        if(!(*this)[i])
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to get copy of a Fiber.\n";
            exit(-1);
          }
      }
  }

void XC::FiberContainer::setup(FiberSection2d &Section2d,const fiber_list &fibers,CrossSectionKR &kr2)
  {
    if(!fibers.empty())
      {
        copy_fibers(fibers);
        updateKRCenterOfMass(Section2d,kr2);
      }
  }

void XC::FiberContainer::setup(FiberSection3d &Section3d,const fiber_list &fibers,CrossSectionKR &kr3)
  {
    if(!fibers.empty())
      {
        copy_fibers(fibers);
        updateKRCenterOfMass(Section3d,kr3);
      }
  }

void XC::FiberContainer::setup(FiberSectionGJ &SectionGJ,const fiber_list &fibers,CrossSectionKR &krGJ)
  {
    if(!fibers.empty())
      {
        copy_fibers(fibers);
        updateKRCenterOfMass(SectionGJ,krGJ);
      }
  }

//! @brief Adds the fiber to the container.
XC::Fiber *XC::FiberContainer::insert(const Fiber &f)
  {
    Fiber *retval= f.getCopy();
    push_back(retval);
    return retval;
  }

//! @brief Adds a fiber XXX Enhance parameter updating.
XC::Fiber *XC::FiberContainer::addFiber(FiberSection2d &Section2d,Fiber &newFiber,CrossSectionKR &kr2)
  {
    Fiber *retval= insert(newFiber);
    updateKRCenterOfMass(Section2d,kr2);
    return retval;
  }

//! @brief Adds a fiber to the section XXX Enhance parameter updating.
XC::Fiber *XC::FiberContainer::addFiber(FiberSection3d &Section3d,Fiber &newFiber,CrossSectionKR &kr3)
  {
    Fiber *retval= insert(newFiber);
    updateKRCenterOfMass(Section3d,kr3);
    return retval;
  }


//! @brief Adds a fiber to the container. XXX Enhance parameter updating.
XC::Fiber *XC::FiberContainer::addFiber(FiberSectionGJ &SectionGJ,Fiber &newFiber,CrossSectionKR &krGJ)
  {
    Fiber *retval= insert(newFiber);
    updateKRCenterOfMass(SectionGJ,krGJ);
    return retval;
  }

//! @brief Destructor.
XC::FiberContainer::~FiberContainer(void)
  { free_mem(); }
