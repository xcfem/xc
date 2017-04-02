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
//SisRef.h

#ifndef SISREF_H
#define SISREF_H

#include "preprocessor/EntMdlrBase.h"

class Vector3d;
class Pos3d;

namespace XC {

//! @ingroup Cad
//
//! @defgroup CadSR Reference systems.
//
//! @ingroup CadSR
//!
//! @brief Reference system to be used for make easier to define
//! point positions.
class SisRef: public EntMdlrBase
  {
  protected:

  public:
    //! @brief Constructor.
    SisRef(Preprocessor *m)
      : EntMdlrBase("",m) {}
    SisRef(const std::string &nombre= "",Preprocessor *m= nullptr)
      : EntMdlrBase(nombre,m) {}
    inline virtual ~SisRef(void)
      {}
    //! @brief Returns the i-th axis unit vector
    //! for the position being passed as parameter.
    virtual Vector3d GetVDirEje(const size_t &,const Pos3d &) const= 0;
    Vector3d GetI(const Pos3d &) const;
    Vector3d GetJ(const Pos3d &) const;
    Vector3d GetK(const Pos3d &) const;

    //! @brief Return the posicion del punto p expresado en locales
    //! expresada en coordenadas globales.
    virtual Pos3d GetPosGlobal(const Pos3d &p) const= 0;
    //! @brief Returns the vector v expresado en locales
    //! expresado en coordenadas globales.
    virtual Vector3d GetCooGlobales(const Vector3d &v) const= 0;
    //! @brief Return the posicion del punto p expresado en globales
    //! expresada en coordenadas locales.
    virtual Pos3d GetPosLocal(const Pos3d &p) const= 0;
    //! @brief Returns the vector v expresado en globales
    //! expresado en coordenadas locales.
    virtual Vector3d GetCooLocales(const Vector3d &v) const= 0;


  };

} //end of XC namespace

#endif
