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
//FiberData.h
		    
#ifndef FiberData_h 
#define FiberData_h 

#include <vector>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <material/section/repres/section/fiber_list.h>

namespace XC {

class Material;
class VectorCells;
class VectorReinfBar;
class RegionContainer;
class ListReinfLayer;
 class GeomSection;

//! @ingroup MATSCCRepresGeom
//
//! @brief Fiber data.
class FiberData
  {
    std::vector<Material *> fibersMaterial; //!< Material pointers for cells and rebars.
    Matrix fibersPosition; //!< Cell and rebar positions.
    Vector fibersArea; //!< Cell and rebar areas.
    size_t put_cells(const size_t &,const VectorCells &,Material *);
    size_t put_reinf_bars(const size_t &,const VectorReinfBar &,Material *);

    inline Material *getPtrMaterial(const int &i) const
      { return fibersMaterial[i]; }
    inline const double &Position(const int &i,const int &j) const
      { return fibersPosition(i,j); }
    inline double &Position(const int &i,const int &j)
      { return fibersPosition(i,j); }
    //! @brief Return a const reference to the area of the i-th fiber.
    inline const double &Area(const int &i) const
      { return fibersArea(i); }
    //! @brief Return a reference to the area of the i-th fiber.
    inline double &Area(const int &i)
      { return fibersArea(i); }

  public:
    FiberData(const int &sz);
    FiberData(const GeomSection &);

    inline void setPtrMaterial(const int &i,Material *mat)
      { fibersMaterial[i]= mat; }

    inline size_t size(void) const
      { return fibersArea.Size(); }

    size_t PutCells(const size_t &,const RegionContainer &);
    size_t putReinforcementLayers(const size_t &,const ListReinfLayer &);
    void get2DFibers(fiber_list &) const;
    void get3DFibers(fiber_list &) const;
  };

} // end of XC namespace
#endif

