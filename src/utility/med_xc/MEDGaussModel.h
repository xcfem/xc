//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDGaussModel.h

#ifndef MEDGAUSSMODEL_H
#define MEDGAUSSMODEL_H

#include "MEDObject.h"
#include "xc_basic/src/med_xc/MEDMEM_define.hxx"
#include "xc_basic/src/med_xc/MEDMEM_GaussLocalization.hxx"

namespace XC {
class GaussModel;
class MEDMeshing;

//! @ingroup MED
//
//!  @brief Information about Gauss model.
class MEDGaussModel: public MEDObject
  {
  public:
    typedef MEDMEM::GAUSS_LOCALIZATION<> med_gauss_model;
  private:
    std::string nombre; //!< set name.
    MED_EN::medGeometryElement type; //!< Cell type (MED_TRIA3,MED_QUAD4,MED_POINT1,...).
    int num_nodes;
    std::vector<double> ref_nodes_coo; //!< Reference nodes coordinates.
    int num_gauss_pt;
    std::vector<double> coo_gauss_pt; //!< Gauss points coordinates.
    std::vector<double> gauss_weights; //!< Gauss points weights.
    mutable med_gauss_model *model; //!< Definition of the Gauss model in MEDMEM.
  public:
    MEDGaussModel(const std::string &,const MED_EN::medGeometryElement &, const GaussModel &);

    const std::string &getName(void) const;
    const MED_EN::medGeometryElement &getType(void) const;
    med_gauss_model *getGaussModelMED(void) const;

    void to_med(void) const;

  };
} // end of XC namespace
#endif
