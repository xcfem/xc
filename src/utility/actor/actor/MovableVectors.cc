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
//MovableVectors.cc

#include "MovableVectors.h"
#include "MovableID.h"
#include "MovableVector.h"
#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "CommMetaData.h"

//! @brief Constructor.
XC::MovableVectors::MovableVectors(std::vector<Vector> &v)
  : MovableContainer<std::vector<Vector> >(v) {}

//! @brief Envía un objecto del contenedor.
int XC::MovableVectors::sendItem(const Vector &v,CommParameters &cp,DbTagData &dt, const CommMetaData &meta)
  { return cp.sendVector(v,dt,meta); }

//! @brief Recibe un objecto del contenedor.
int XC::MovableVectors::receiveItem(Vector &v, const CommParameters &cp,DbTagData &dt, const CommMetaData &meta)
  { return cp.receiveVector(v,dt,meta); }
