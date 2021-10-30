// -*-c++-*-
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
//ContinuaReprComponent.h                                                         
                                                                        
#ifndef ContinuaReprComponent_h
#define ContinuaReprComponent_h

#include "domain/component/DomainComponent.h"

namespace XC {

//! @brief Base class for components used to represent the material (continuum).
//! @ingroup Dom
class ContinuaReprComponent: public DomainComponent
  {
    bool dead; //!< True if domain component is not active.
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructors
    ContinuaReprComponent(int classTag);
    ContinuaReprComponent(int tag, int classTag);

    //! @brief Return true if the component is not active.
    virtual const bool isDead(void) const
      { return dead; }
    //! @brief Return true if the component is active.
    virtual const bool isAlive(void) const
      { return !dead; }
    //! @brief Deactivates the component.
    virtual void kill(void)
      { dead= true; }
    //! @brief Activates the component.
    virtual void alive(void)
      { dead= false; }
  };

} // end of XC namespace

#endif

