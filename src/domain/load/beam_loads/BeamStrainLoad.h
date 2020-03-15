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
//BeamStrainLoad.h
                                                                        
#ifndef BeamStrainLoad_h
#define BeamStrainLoad_h


#include <domain/load/beam_loads/BeamLoad.h>
#include "material/section/interaction_diagram/DeformationPlane.h"

namespace XC {
class Matrix;
class CrossSectionProperties2d;
class CrossSectionProperties3d;

//! @ingroup ElemLoads
//
//! @brief Load due to restricted material expansion or contraction on beam elements.
class BeamStrainLoad : public BeamLoad
  {
    DeformationPlane backEndDeformationsPlane; //!< Restricted generalized strains at element first node.
    DeformationPlane frontEndDeformationPlane; //!< Restricted generalized strains at element last node.
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    BeamStrainLoad(int tag, const ID &theElementTags);
    BeamStrainLoad(int tag= 0);

    inline const DeformationPlane &getDeformationPlane1(void) const
      { return backEndDeformationsPlane; }
    inline void setDeformationPlane1(const DeformationPlane &p)
      { backEndDeformationsPlane= p; }
    inline const DeformationPlane &getDeformationPlane2(void) const
      { return frontEndDeformationPlane; }
    inline void setDeformationPlane2(const DeformationPlane &p)
      { frontEndDeformationPlane= p; }
    const Vector &getSection1Deformation(const size_t &order,const ResponseId &code) const;
    const Vector &getSection2Deformation(const size_t &order,const ResponseId &code) const;
    const Vector &getData(int &type, const double &loadFactor) const;

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    //void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

