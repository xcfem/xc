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
//Cad.h

#ifndef CAD_H
#define CAD_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include <map>
#include "boost/lexical_cast.hpp"
#include "preprocessor/cad/entidades/MapPuntos.h"
#include "preprocessor/cad/entidades/MapLineas.h"
#include "preprocessor/cad/entidades/MapSuperficies.h"
#include "preprocessor/cad/entidades/MapCuerpos.h"
#include "preprocessor/cad/entidades/MapUniformGrids.h"
#include "preprocessor/cad/trf/MapTrfGeom.h"
#include "preprocessor/cad/MapSisRef.h"
#include "preprocessor/cad/matrices/MapEsquemas2d.h"
#include "preprocessor/cad/matrices/MapEsquemas3d.h"

class Pos3d;
class Recta3d;
class Plano3d;
class Vector3d;

namespace XC {

class Preprocessor;
class Pnt;
class UniformGrid;
class SetEstruct;
class SisRef;

//! \ingroup Preprocessor
//
//! \defgroup Cad Generación de la geometría del modelo.
//
//! @ingroup Cad
//
//! @brief Objeto encargado de generar la geometría del
//! modelo y gestionar las entidades de tipo CAD:
//! puntos, lineas, superficies, sólidos, etc.
//!
//! Mantiene las listas de estas entidades y crea, para
//! cada una de ellas un conjunto destinado a alojar un puntero a la
//! propia entidad, así como punteros a los nodos y elementos contenidos
//! en la entidad.
class Cad: public EntCmd
  {
  private:
    Preprocessor *preprocessor; //!< Preprocessor.

    template <class L>
    static void numera_lista(L &l);
    void numera(void);
  private:
    MapSisRef sistemas_referencia; //!< Lista de sistemas de referencia.
    MapTrfGeom transformaciones_geometricas; //!< Lista de transformaciones geométricas.

    MapPuntos puntos; //!< Lista de puntos.
    MapLineas edges; //!< Lista de edges.
    MapSuperficies faces; //!< Lista de superficies.
    MapCuerpos cuerpos; //!< Lista de cuerpos.
    MapUniformGrids unif_grid; //!< Lista de mallas uniformes.

    MapEsquemas2d esquemas2d; //!< Lista de esquemas bidimensionales.

    MapEsquemas3d esquemas3d; //!< Lista de esquemas tridimensionales.

  protected:

    friend class Preprocessor;
    SetEstruct *busca_set_estruct(const MapUniformGrids::Indice &nmb);
  public:
    //! @brief Constructor.
    Cad(Preprocessor *mod= NULL);
    inline Preprocessor *getPreprocessor(void)
      { return preprocessor; } 
    inline const Preprocessor *getPreprocessor(void) const
      { return preprocessor; } 

    Edge *busca_edge_extremos(const MapPuntos::Indice &,const MapPuntos::Indice &);
    const Edge *busca_edge_extremos(const MapPuntos::Indice &,const MapPuntos::Indice &) const;
    
    void conciliaNDivs(void);

    void clearAll(void);
    //! @brief Destructor.
    virtual ~Cad(void);

    inline const MapPuntos &getPuntos(void) const
      { return puntos; }
    inline MapPuntos &getPuntos(void)
      { return puntos; }
    inline const MapLineas &getLineas(void) const
      { return edges; }
    inline MapLineas &getLineas(void)
      { return edges; }
    inline const MapSuperficies &getSuperficies(void) const
      { return faces; }
    inline MapSuperficies &getSuperficies(void)
      { return faces; }
    inline const MapTrfGeom &getTransformacionesGeometricas(void) const
      { return transformaciones_geometricas; }
    inline MapTrfGeom &getTransformacionesGeometricas(void)
      { return transformaciones_geometricas; }
    inline const MapSisRef &getSistemasReferencia(void) const
      { return sistemas_referencia; }
    inline MapSisRef &getSistemasReferencia(void)
      { return sistemas_referencia; }
    inline const MapEsquemas2d &getEsquemas2d(void) const
      { return esquemas2d; }
    inline MapEsquemas2d &getEsquemas2d(void)
      { return esquemas2d; }
    inline const MapEsquemas3d &getEsquemas3d(void) const
      { return esquemas3d; }
    inline MapEsquemas3d &getEsquemas3d(void)
      { return esquemas3d; }
    inline const MapUniformGrids &getUniformGrids(void) const
      { return unif_grid; }
    inline MapUniformGrids &getUniformGrids(void)
      { return unif_grid; }
  };


} //end of XC namespace
#endif
