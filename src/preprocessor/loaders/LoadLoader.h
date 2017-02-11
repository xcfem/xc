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
//LoadLoader.h

#ifndef LOADLOADER_H
#define LOADLOADER_H

#include "Loader.h"
#include "domain/load/pattern/MapLoadPatterns.h"
#include "domain/load/pattern/GrupoCombinaciones.h"

namespace XC {
class GroundMotion;
class Combinacion;

//!  \ingroup Ldrs
//! 
//! @brief Lee casos de carga desde archivo.
//! Se encarga del mantenimiento de la lista de timeseries.
class LoadLoader: public Loader
  {
    typedef std::map<std::string,GroundMotion *> map_ground_motions;
    map_ground_motions ground_motions; //!< Movimientos del terreno.

    MapLoadPatterns lpatterns; //!< Contenedor de hipótesis simples.
    int tag_lp; //!< Default identifier for load patterns or load combinations.

    GrupoCombinaciones combinaciones; //!< Load combination container.

  protected:
    friend class Combinacion;
    friend class Domain;
 
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    LoadLoader(Preprocessor *owr);
    ~LoadLoader(void);

    void addToDomain(const std::string &);
    void removeFromDomain(const std::string &);
    void removeAllFromDomain(void);

    inline int &getTagLP(void)
      { return tag_lp; }
    inline const int &getTagLP(void) const
      { return tag_lp; }
    inline void setTagLP(const int &tlp)
      { tag_lp= tlp; } 
    MapLoadPatterns &getLoadPatterns(void)
      { return lpatterns; }
    const MapLoadPatterns &getLoadPatterns(void) const
      { return lpatterns; }
    GrupoCombinaciones &getCombinaciones(void)
      { return combinaciones; }
    const GrupoCombinaciones &getCombinaciones(void) const
      { return combinaciones; }

    void clearAll(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

  };

} // end of XC namespace

#endif
