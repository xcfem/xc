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
//RegionContainer.h

#ifndef RegionContainer_h 
#define RegionContainer_h 

#include <list>
#include "material/section/repres/SectionMassProperties.h"

class Pos2d;
class BND2d;
class Polygon2d;
class HalfPlane2d;

namespace XC {

class SectRegion;
class QuadSectRegion;
class CircularSectRegion;
class MaterialHandler;
class Vector;
class Matrix;

//! @ingroup MATSCCRegions
//
//! @brief Section composed of some regions.
class RegionContainer: protected std::list<SectRegion *>, public SectionMassProperties
  {
  public:
    typedef std::list<SectRegion *> l_reg;
    typedef l_reg::reference reference;
    typedef l_reg::const_reference const_reference;
    typedef l_reg::iterator iterator;
    typedef l_reg::const_iterator const_iterator;
  private:
    void free_mem(void);
    void free_mem(const size_t i);
    void copy(const RegionContainer &other);
  protected:

    MaterialHandler *material_handler; //!< Material definition handler (searching,...).
  public:

    RegionContainer(MaterialHandler *ml);
    RegionContainer(const RegionContainer  &);
    RegionContainer &operator=(const RegionContainer  &);
    ~RegionContainer(void);

    XC::SectRegion *push_back(const SectRegion &reg);
    inline size_t size(void) const
      { return l_reg::size(); }
    inline bool empty(void) const
      { return l_reg::empty(); }

    void clear(void);

    const_iterator begin(void) const;
    const_iterator end(void) const;
    iterator begin(void);
    iterator end(void);

    QuadSectRegion *newQuadRegion(const std::string &);
    CircularSectRegion *newCircularRegion(const std::string &);

    size_t getNumCells(void) const;

    std::list<Polygon2d> getRegionsContours(void) const;
    std::list<Polygon2d> getContours(void) const;
    BND2d getBnd(void) const;
    RegionContainer Intersection(const HalfPlane2d &) const;

    double getAreaGrossSection(void) const;
    Vector getCenterOfMassGrossSection(void) const;
    double getIyGrossSection(void) const;
    double getIzGrossSection(void) const;
    double getPyzGrossSection(void) const;

    Vector getCenterOfMassHomogenizedSection(const double &E0) const;
    double getAreaHomogenizedSection(const double &E0) const;
    double getIyHomogenizedSection(const double &E0) const;
    double getIzHomogenizedSection(const double &E0) const;
    double getPyzHomogenizedSection(const double &E0) const;

    void Print(std::ostream &s) const;
  };

std::ostream &operator<<(std::ostream &,const RegionContainer &);

} // end of XC namespace


#endif
