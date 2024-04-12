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

#ifndef NDMaterialWrapper_h
#define NDMaterialWrapper_h

#include "material/MaterialWrapper.h"
#include "material/nD/NDMaterial.h"

namespace XC {
  
//! @ingroup MatUnx
//
//! @brief Encapsulates a copy to an uniaxial material.
class NDMaterialWrapper: public MaterialWrapper<NDMaterial, MAT_TAG_NDMaterialWrapper>
  {
  public:
    NDMaterialWrapper(void);
    NDMaterialWrapper(const NDMaterial &material);
    
    double getRho(void) const;
    // send back strain
    const Vector& getStrain() const;
    // send back stress
    const Vector& getStress() const;
    // send back the tangent
    const Matrix &getTangent() const;
    const Matrix &getInitialTangent() const;
    
    // set the strain to be sent to the main material
    int setTrialStrain(const Vector &);
    
    const std::string &getType(void) const;
    int getOrder(void) const;
    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    Response *setResponse(const std::vector<std::string> &argv, Information &matInfo);
    int getResponse(int responseID, Information &matInformation);
  };
} // end of XC namespace


#endif

