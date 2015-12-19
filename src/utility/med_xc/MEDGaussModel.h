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

    any_const_ptr GetProp(const std::string &) const;
  };
} // end of XC namespace
#endif
