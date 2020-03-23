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
//ConnectedMaterial.h                                                                        
                                                                        
#ifndef ConnectedMaterial_h
#define ConnectedMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>
#include <material/uniaxial/DqUniaxialMaterial.h>

namespace XC{
//! @ingroup MatUnx
//
//! @brief Connected uniaxial materials (parallel or serial).
class ConnectedMaterial: public UniaxialMaterial
  {
  protected:
    DqUniaxialMaterial theModels; //!< an array of pointers to the UniaxialMaterials
  public:
    ConnectedMaterial(int tag, int classTag,const DqUniaxialMaterial &theMaterials);
    ConnectedMaterial(int tag, int classTag);
    ConnectedMaterial(const ConnectedMaterial &);
    ConnectedMaterial &operator=(const ConnectedMaterial &);

    int sendData(CommParameters &);  
    int recvData(const CommParameters &);
  };
} // end of XC namespace


#endif

