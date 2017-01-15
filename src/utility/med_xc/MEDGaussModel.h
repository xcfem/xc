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
//Información sobre la integración de Gauss de las celdas de la malla (para exportar archivos a «salome»).

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
//!  @brief Información sobre conjuntos de nodos y elementos.
class MEDGaussModel: public MEDObject
  {
  public:
    typedef MEDMEM::GAUSS_LOCALIZATION<> med_gauss_model;
  private:
    std::string nombre; //!< nombre del conjunto.
    MED_EN::medGeometryElement tipo; //!< Tipo de celda (MED_TRIA3,MED_QUAD4,MED_POINT1,...).
    int num_nodos;
    std::vector<double> coo_nodos_ref; //!< Coordenadas de los nodos de referencia.
    int num_gauss_pt;
    std::vector<double> coo_gauss_pt; //!< Coordenadas de los puntos de gauss.
    std::vector<double> gauss_weights; //!< Coordenadas de los puntos de gauss.
    mutable med_gauss_model *model; //!< Definición del modelo de Gauss en MEDMEM.
  public:
    MEDGaussModel(const std::string &,const MED_EN::medGeometryElement &, const GaussModel &);

    const std::string &getNombre(void) const;
    const MED_EN::medGeometryElement &getTipo(void) const;
    med_gauss_model *getGaussModelMED(void) const;

    void to_med(void) const;

  };
} // end of XC namespace
#endif
