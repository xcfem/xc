// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

#ifndef UNIFORMGRID_H
#define UNIFORMGRID_H

#include "preprocessor/multi_block_topology/entities/EntMdlr.h"
#include "utility/geom/pos_vec/Pos3d.h"

namespace XC {

//!  @ingroup MultiBlockTopologyEnt
//!
//!  @brief Uniform mesh. Uniform node distributionn on x, y and z.
class UniformGrid: public EntMdlr
  {
  private:
    Pos3d org; //!< Grid origin.
    double Lx; //!< Grid spacing on x axis.
    double Ly; //!< Grid spacing on y axis.
    double Lz; //!< Grid spacing on z axis.
    size_t ndiv_x; //! Number of divisions on x axis.
    size_t ndiv_y; //! Number of divisions on y axis.
    size_t ndiv_z; //! Number of divisions on z axis.
  protected:

    void update_topology(void);
    void create_nodes(void);
  public:
    UniformGrid(Preprocessor *m,const size_t &i= 0);
    UniformGrid(const std::string &name= "",Preprocessor *m= nullptr,const size_t &i= 0);
    virtual bool operator==(const UniformGrid &) const;
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

    inline void setNDivX(const size_t &n)
      { ndiv_x= n; }
    inline size_t getNDivX(void) const
      { return ndiv_x; }
    inline void setNDivY(const size_t &n)
      { ndiv_y= n; }
    inline size_t getNDivY(void) const
      { return ndiv_y; }
    inline void setNDivZ(const size_t &n)
      { ndiv_z= n; }
    inline size_t getNDivZ(void) const
      { return ndiv_z; }

    virtual BND3d Bnd(void) const;
    Pos3d getCenter(void) const;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    virtual void genMesh(meshing_dir dm);
  };

} //end of XC namespace

#endif
