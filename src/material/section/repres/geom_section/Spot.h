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
//Spot.h

#ifndef SPOT_H
#define SPOT_H

#include "EntGeomSection.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"

class BND2d;

namespace XC {
class Eje;

//! @ingroup MATSCCEntGeom
//
//! @brief Point object for section geometry definition.
class Spot: public EntGeomSection
  {
  private:
    Pos2d p; //!< Posición del punto.
    std::set<const Eje *> ejes_pt; //!< Ejes uno de cuyos extremos es este punto:
    friend class Eje;
    void inserta_linea(Eje *l);
    void borra_linea(Eje *l);
  protected:

    virtual void actualiza_topologia(void);

    friend class GeomSection;
    //! @brief Constructor.
    Spot(const Pos2d &pto= Pos2d());
    //! @brief Constructor.
    Spot(GeomSection *m)
      : EntGeomSection("",m) {}
    //! @brief Constructor.
    Spot(const std::string &nombre= "",GeomSection *m= nullptr)
      : EntGeomSection(nombre,m) {}

  public:
    //! @brief Return the dimensión del objeto.
    inline virtual unsigned short int GetDimension(void) const
      { return 0; }
    BND2d Bnd(void) const;

    //! @brief Return the posición del objeto.
    const Pos2d &GetPos(void) const
      { return p; }
    //! @brief Return the posición del objeto.
    Pos2d &GetPos(void)
      { return p; }
    inline void setPos(const Pos2d &pos)
      { p= pos; }
    Vector2d VectorPos(void) const;

    //! @brief Return the lista de líneas que tocan al punto.
    const std::set<const Eje *> &EjesTocan(void) const
      { return ejes_pt; }
    bool Toca(const Eje &l) const;
    inline size_t nLines(void) const
      { return ejes_pt.size(); }

    double DistanciaA(const Pos2d &pt) const;


  };

} //end of XC namespace

#endif
