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
//UniformGrid.h
//Malla con los nodos equiespaciados en x, y, y z.

#ifndef UNIFORMGRID_H
#define UNIFORMGRID_H

#include "EntMdlr.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

namespace XC {

//!  @ingroup CadEnt
//!
//!  @brief Malla con los nodos equiespaciados en x, y, y z.
class UniformGrid: public EntMdlr
  {
  private:
    Pos3d org; //!< Posición del origen de la malla.
    double Lx; //!< Dimensión de la malla paralela al eje x.
    double Ly; //!< Dimensión de la malla paralela al eje y.
    double Lz; //!< Dimensión de la malla paralela al eje z.
    size_t ndiv_x; //! Número de divisiones en el eje x.
    size_t ndiv_y; //! Número de divisiones en el eje y.
    size_t ndiv_z; //! Número de divisiones en el eje z.
  protected:

    void actualiza_topologia(void);
    void crea_nodos(void);
  public:
    UniformGrid(Preprocessor *m,const size_t &i= 0);
    UniformGrid(const std::string &nombre= "",Preprocessor *m= NULL,const size_t &i= 0);
    SetEstruct *getCopy(void) const;
    virtual unsigned short int GetDimension(void) const;


    inline void setOrg(const Pos3d &p)
      { org= p; }
    inline const Pos3d &getOrg(void) const
      { return org; }

    inline void setLx(const double &l)
      { Lx= l; }
    inline double getLx(void) const
      { return Lx; }
    inline void setLy(const double &l)
      { Ly= l; }
    inline double getLy(void) const
      { return Ly; }
    inline void setLz(const double &l)
      { Lz= l; }
    inline double getLz(void) const
      { return Lz; }

    inline void setNDivX(const size_t &l)
      { ndiv_x= l; }
    inline size_t getNDivX(void) const
      { return Lx; }
    inline void setNDivY(const size_t &l)
      { ndiv_y= l; }
    inline size_t getNDivY(void) const
      { return Ly; }
    inline void setNDivZ(const size_t &l)
      { ndiv_z= l; }
    inline size_t getNDivZ(void) const
      { return Lz; }

    virtual BND3d Bnd(void) const;
    Pos3d GetCentro(void) const;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    virtual void Malla(dir_mallado dm);
  };

} //end of XC namespace

#endif
