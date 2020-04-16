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
//SectionFDPhysicalProperties.h

#ifndef SectionFDPhysicalProperties_h
#define SectionFDPhysicalProperties_h

#include "PhysicalProperties.h"
#include "material/section/SectionForceDeformation.h"

namespace XC {

//! @ingroup PhysicalProperties
//
//! @brief Physical properties for shells.
class SectionFDPhysicalProperties: public PhysicalProperties<SectionForceDeformation>
  {
  public:
    SectionFDPhysicalProperties(const size_t &nMat= 0,const SectionForceDeformation *ptr_mat= nullptr); 

    double getMeanInternalForce(const std::string &) const;
    double getMeanInternalDeformation(const std::string &) const;
    double getMeanGeneralizedStrainByName(const std::string &) const;
    double getMeanGeneralizedStressByName(const std::string &) const;
    Vector getGeneralizedStrainAtGaussPointsByName(const std::string &) const;
    Vector getGeneralizedStressAtGaussPointsByName(const std::string &) const;


    bool haveRho(void) const;
    Vector getRhoi(void) const;
    
    int getResponse(int responseID, Information &eleInformation);
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);

    void Print(std::ostream &s, int) const;
  }; 

} // end of XC namespace
#endif
