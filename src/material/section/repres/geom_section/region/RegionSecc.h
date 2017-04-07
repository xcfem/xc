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
//RegionSecc.h
// Written by Remo M. de Souza
// December 1998

#ifndef RegionSecc_h 
#define RegionSecc_h 


#include <iostream>
#include "material/section/repres/DiscretBase.h" 
#include <material/section/repres/cell/VectorCells.h>

class Poligono2d;
class Semiplano2d;
class Pos2d;

namespace XC {
class Cell;
class VectorCells;
class Vector;
class Matrix;
class RgSccPoligono;

//! @ingroup MATSCCRepresGeom
//
//! @defgroup MATSCCRegiones Material filled regions of a section.
//
//! @ingroup MATSCCRegiones
//
//! @brief Region of a section that corresponds with a material.
class RegionSecc: public DiscretBase
  {
  protected:
    mutable VectorCells cells;

  public:
    RegionSecc(Material *);
    virtual ~RegionSecc(void) {}

    // inquiring functions
    //! @brief Returns the number of cells of the region.
    virtual int getNumCells(void) const= 0;
    //! @brief Returns the contenedor de celdas.
    virtual const VectorCells &getCells(void) const= 0;
    //! @brief Returns a copy of the region.
    virtual RegionSecc *getCopy(void) const= 0;

    virtual Poligono2d getPoligono(void) const;
    RgSccPoligono Interseccion(const Semiplano2d &sp) const;
    const Vector &Cdg(void) const;
    double Longitud(void) const;
    double Area(void) const;
    double Iy(void) const;
    double Iz(void) const;
    double Pyz(void) const;
    double Ix(void) const;
    double Theta_p(void) const;
    const Vector &DirEjeI_a(void) const;
    const Vector &DirEjeI_b(void) const;
    double getI1(void) const;
    double getI2(void) const;
    double getI(const unsigned short int &i,const unsigned short int &j) const;
    double getI(const unsigned short int i,const unsigned short int j,const Pos2d &o) const;
    double getI(const Pos2d &O,const Vector &e) const;
    double getIO(const Pos2d &o);
    Matrix &getI(void) const;
    Matrix &getI(const Pos2d &o) const;



    virtual void Print(std::ostream &s, int flag= 0) const =0;   
    friend std::ostream &operator<<(std::ostream &s, const RegionSecc &rg_scc);    
  };
} // end of XC namespace


#endif

