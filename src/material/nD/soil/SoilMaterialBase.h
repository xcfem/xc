// -*-c++-*-
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
// $Revision: 1.11 $
// $Date: 2003/10/07 21:20:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/SoilMaterialBase.h,v $
                                                                        
// Written: ZHY
// Created: August 2000
// Revision: A
//
// Description: This file contains the class prototype for SoilMaterialBase.
//
// What: "@(#) SoilMaterialBase.h, revA"

#ifndef SoilMaterialBase_h
#define SoilMaterialBase_h

#include "material/nD/NDMaterial.h"

namespace XC {
class Response;

//! @ingroup NDMat
//!
//! @defgroup SoilNDMat Constitutive models for soil materials.
//
//! @ingroup SoilNDMat
//
//! @brief Material that couples the responses of two phases: fluid and solid.
//!
//! The fluid phase response is only volumetric and linear elastic. The solid
//! phase can be any NDMaterial. This material is developed to simulate the
//! response of saturated porous media under fully undrained condition. 
class SoilMaterialBase: public NDMaterial
  {
  protected:
    static int matCount;
    static std::vector<int> ndmx;
    static std::vector<int> loadStagex;
    int matN;
    mutable int e2p;

    int sendData(Communicator &);
    int recvData(const Communicator &);
    void resizeIfNeeded(void);
  public:
    // Initialization constructor
    SoilMaterialBase(int tag, int classTag);

    inline int getDimension(void) const
      { return ndmx[matN]; }
    void setDimension(const int &);
    const std::string &getType(void) const;
    int getOrder(void) const;

    int getMaterialStage(void) const;
    void setMaterialStage(const int &);
    void updateMaterialStage(int);

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
