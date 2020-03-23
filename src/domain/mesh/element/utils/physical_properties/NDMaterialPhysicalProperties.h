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
//NDMaterialPhysicalProperties.h
                                                                        
#include "PhysicalProperties.h"
#include "material/nD/NDMaterial.h"

#ifndef NDMaterialPhysicalProperties_h
#define NDMaterialPhysicalProperties_h

namespace XC {
//! @ingroup PysicalProperties
//
//! @brief Physical properties for solid mechanics.
class NDMaterialPhysicalProperties: public PhysicalProperties<NDMaterial>
  {
  protected:
    virtual bool check_material_type(const std::string &type) const;
  public:
    static bool check_material_elast_plana(const std::string &type);

    NDMaterialPhysicalProperties(const size_t &nMat= 0, const NDMaterial *ptr_mat= nullptr);
    NDMaterialPhysicalProperties(const size_t &, NDMaterial &theMaterial, const std::string &type);

    inline const Vector &getCommittedStrain(const size_t &i) const
      { return theMaterial[i]->getCommittedStrain(); }
    inline const Vector &getCommittedStress(const size_t &i) const
      { return theMaterial[i]->getCommittedStress(); }
    Vector getCommittedAvgStrain(void) const;
    Vector getCommittedAvgStress(void) const;
    double getCommittedAvgStrain(const size_t &iComp) const;
    double getCommittedAvgStress(const size_t &iComp) const;

    Matrix getCommittedStrain(void) const;
    Matrix getCommittedStress(void) const;

    bool haveRho(void) const;
    double getRho(void) const;
    void setRho(const double &);
    Vector getRhoi(const double &rhoDefault= 0.0) const;
 };

} // end of XC namespace
#endif
