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
//DiscretBase.h

#ifndef DiscretBase_h 
#define DiscretBase_h 


#include "xc_utils/src/nucleo/EntConNmb.h"

namespace XC {
  class Material;

//! @ingroup MATSCC
//!
//! @defgroup MATSCCRepres Cross-section material representation.
//
//! @ingroup MATSCCRepres
//!
//! @brief Base class for cross-section discretization.
class DiscretBase: public EntConNmb
  {
  private:
    Material *mat; //!< Pointer to the fibers material.
  protected:

  public:
    DiscretBase(Material *mat);

    virtual double getMaxY(void) const= 0;
    virtual double getMaxZ(void) const= 0;
    virtual double getMinY(void) const= 0;
    virtual double getMinZ(void) const= 0;

    void setMaterialPtr(Material *mat);
    Material *getMaterialPtr(void) const;

  };
} // end of XC namespace


#endif

