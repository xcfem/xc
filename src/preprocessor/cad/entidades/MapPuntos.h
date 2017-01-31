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
//MapPuntos.h

#ifndef MAPPUNTOS_H
#define MAPPUNTOS_H

#include "MapEnt.h"
#include <map>

class Pos3d;
class Recta3d;
class Plano3d;
class Vector3d;

namespace XC {

class Preprocessor;
class Cad;
class Pnt;
class SetEstruct;
class TrfGeom;

//! @ingroup Cad
//
//! @brief Point container.
class MapPuntos: public MapEnt<Pnt>
  {
  private:
    void UpdateSets(Pnt *) const;
  protected:


    Pnt *Crea(void);
  public:
    MapPuntos(Cad *cad= NULL);

    Vector3d getVector(const Indice &i,const Indice &j) const;
    Recta3d getRecta(const Indice &i,const Indice &j) const;
    Plano3d getPlano(const Indice &i,const Indice &j,const Indice &k) const;

    Pnt *Nuevo(void);
    Pnt *Nuevo(const Pos3d &);
    Pnt *Nuevo(const size_t &,const Pos3d &);

    Pnt *Copia(const Pnt *,const Vector3d &v);
    void Copia(const std::vector<MapPuntos::Indice> &);
    void Transforma(const TrfGeom &trf,const std::vector<MapPuntos::Indice> &);
    double Dist(const Indice &i,const Indice &j) const;


  };

} //end of XC namespace
#endif
