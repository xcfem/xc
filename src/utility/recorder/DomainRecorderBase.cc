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
//DomainRecorderBase.cc

#include <utility/recorder/DomainRecorderBase.h>

//! @brief Constructor.
//!
//! @param classTag: class identifier.
//! @param ptr_dom: pointer to the domain.
XC::DomainRecorderBase::DomainRecorderBase(int classTag,Domain *ptr_dom)
  :Recorder(classTag),theDomain(ptr_dom) {}

//! @brief Set the link with the domain.
int XC::DomainRecorderBase::setDomain(Domain &theDom)
  {
    theDomain = &theDom;
    return 0;
  }
