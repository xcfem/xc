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
//SisRefScc.h

#ifndef SISREFSCC_H
#define SISREFSCC_H

#include "EntGeomSection.h"

class Vector2d;
class Pos2d;

namespace XC {

//! @ingroup MATSCCEntGeom
//
//! @brief Reference system used for make easier definining point positions.
class SisRefScc: public EntGeomSection
  {
  protected:

  public:
    //! @brief Constructor.
    SisRefScc(GeomSection *m)
      : EntGeomSection("",m) {}
    SisRefScc(const std::string &nombre= "",GeomSection *m= NULL)
      : EntGeomSection(nombre,m) {}

    //! @brief Devuelve el vector unitario en la dirección del eje que se
    //! pasa como parámetro expresado en el sistema global 
    //! para la posición being passed as parameter.
    virtual Vector2d GetVDirEje(const size_t &,const Pos2d &) const= 0;
    Vector2d GetI(const Pos2d &) const;
    Vector2d GetJ(const Pos2d &) const;

    //! @brief Devuelve la posicion del punto p expresado en locales
    //! expresada en coordenadas globales.
    virtual Pos2d GetPosGlobal(const Pos2d &p) const= 0;
    //! @brief Devuelve el vector v expresado en locales
    //! expresado en coordenadas globales.
    virtual Vector2d GetCooGlobales(const Vector2d &v) const= 0;
    //! @brief Devuelve la posicion del punto p expresado en globales
    //! expresada en coordenadas locales.
    virtual Pos2d GetPosLocal(const Pos2d &p) const= 0;
    //! @brief Devuelve el vector v expresado en globales
    //! expresado en coordenadas locales.
    virtual Vector2d GetCooLocales(const Vector2d &v) const= 0;


  };

} //end of XC namespace

#endif
