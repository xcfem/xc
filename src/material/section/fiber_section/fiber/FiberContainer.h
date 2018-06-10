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
//FiberContainer.h

#ifndef FiberContainer_h
#define FiberContainer_h

#include "FiberDeque.h"
#include <material/section/repres/section/fiber_list.h>

namespace XC {

//! @ingroup MATSCCFibers
//
//! @brief Fiber container.
class FiberContainer : public FiberDeque
  {
    void free_mem(void);
    void copy_fibers(const FiberContainer &otro);
    void copy_fibers(const fiber_list &);

  public:
    FiberContainer(const size_t &num= 0); 
    FiberContainer(const FiberContainer &otro);
    FiberContainer &operator=(const FiberContainer &otro);
    void allocFibers(int numOfFibers,const Fiber *muestra= nullptr);
    void setup(FiberSection2d &,const fiber_list &,CrossSectionKR &);
    void setup(FiberSection3d &,const fiber_list &,CrossSectionKR &);
    void setup(FiberSectionGJ &,const fiber_list &,CrossSectionKR &);
    ~FiberContainer(void);
  };
} // end of XC namespace

#endif
