//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MEDGaussModel

#include "MEDGaussModel.h"
#include "MEDMeshing.h"

#include "domain/mesh/element/gauss_models/GaussModel.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "xc_basic/src/med_xc/MEDMEM_GaussLocalization.hxx"

//! Constructor.
XC::MEDGaussModel::MEDGaussModel(const std::string &nmb,const MED_EN::medGeometryElement &tp,const GaussModel &gm)
  : nombre(nmb),tipo(tp), num_nodos(gm.getNumNodosReferencia()),num_gauss_pt(gm.getNumGaussPoints()), model(nullptr)
  { 
    const size_t dim= tp/100; //Dimensión del espacio en que se define el elemento.
    coo_nodos_ref.resize(num_nodos*dim,0);
    const std::deque<Pos3d> &nodos= gm.getPosNodosReferencia(); size_t conta= 0;
    for(std::deque<Pos3d>::const_iterator i=nodos.begin();i!=nodos.end();i++)
      {
        coo_nodos_ref[conta]= i->x(); conta++;
        if(dim>1) 
          { coo_nodos_ref[conta]= i->y(); conta++;}
        if(dim>2) 
          { coo_nodos_ref[conta]= i->z(); conta++;}
      }
    coo_gauss_pt.resize(num_gauss_pt*dim,0);
    gauss_weights.resize(num_gauss_pt,0);
    const std::deque<GaussPoint> &pts_gauss= gm.getPuntosGauss(); conta= 0; size_t iw= 0;
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

//! @brief Devuelve el nombre del model.
const std::string &XC::MEDGaussModel::getNombre(void) const
  { return nombre; }

//! @brief Devuelve el tipo del elemento al que se refiere el modelo.
const MED_EN::medGeometryElement &XC::MEDGaussModel::getTipo(void) const
  { return tipo; }

//! @brief Vuelca la definición del modelo en MEDMEM
//! malla MED
void XC::MEDGaussModel::to_med(void) const
  {
    model= new med_gauss_model(nombre,tipo,num_gauss_pt,&coo_nodos_ref[0],&coo_gauss_pt[0],&gauss_weights[0]);
  }

