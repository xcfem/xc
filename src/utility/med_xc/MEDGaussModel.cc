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
//MEDGaussModel

#include "MEDGaussModel.h"
#include "MEDMeshing.h"

#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "xc_basic/src/med_xc/MEDMEM_GaussLocalization.hxx"

//! Constructor.
XC::MEDGaussModel::MEDGaussModel(const std::string &nmb,const MED_EN::medGeometryElement &tp,const GaussModel &gm)
  : nombre(nmb),type(tp), num_nodes(gm.getNumberOfReferenceNodes()),num_gauss_pt(gm.getNumGaussPoints()), model(nullptr)
  { 
    const size_t dim= tp/100; //Dimension of the space in which the element is defined.
    ref_nodes_coo.resize(num_nodes*dim,0);
    const std::deque<Pos3d> &nodes= gm.getReferenceNodesPositions(); size_t conta= 0;
    for(std::deque<Pos3d>::const_iterator i=nodes.begin();i!=nodes.end();i++)
      {
        ref_nodes_coo[conta]= i->x(); conta++;
        if(dim>1) 
          { ref_nodes_coo[conta]= i->y(); conta++;}
        if(dim>2) 
          { ref_nodes_coo[conta]= i->z(); conta++;}
      }
    coo_gauss_pt.resize(num_gauss_pt*dim,0);
    gauss_weights.resize(num_gauss_pt,0);
    const std::deque<GaussPoint> &pts_gauss= gm.getGaussPoints(); conta= 0; size_t iw= 0;
    for(std::deque<GaussPoint>::const_iterator i=pts_gauss.begin();i!=pts_gauss.end();i++)
      {
        coo_gauss_pt[conta]= i->r_coordinate(); conta++;
        if(dim>1) 
          { coo_gauss_pt[conta]= i->s_coordinate(); conta++; }
        if(dim>2) 
          { coo_gauss_pt[conta]= i->t_coordinate(); conta++; }
        gauss_weights[iw]= i->r_weight(); iw++;
      }
      
  }

XC::MEDGaussModel::med_gauss_model *XC::MEDGaussModel::getGaussModelMED(void) const
  { 
    if(!model)
      to_med();
    return model;
  }

//! @brief Returns the nombre del model.
const std::string &XC::MEDGaussModel::getNombre(void) const
  { return nombre; }

//! @brief Returns the element type the model.
const MED_EN::medGeometryElement &XC::MEDGaussModel::getType(void) const
  { return type; }

//! @brief Dumps the gauss model into MED.
void XC::MEDGaussModel::to_med(void) const
  {
    model= new med_gauss_model(nombre,type,num_gauss_pt,&ref_nodes_coo[0],&coo_gauss_pt[0],&gauss_weights[0]);
  }

