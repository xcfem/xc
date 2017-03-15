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
//ListRegiones.h

#ifndef ListRegiones_h 
#define ListRegiones_h 

#include <list>
#include "material/section/repres/SeccionInerte.h"

class Pos2d;
class BND2d;
class Poligono2d;
class Semiplano2d;

namespace XC {

class RegionSecc;
class RgSccQuad;
class RgSccCirc;
class MaterialLoader;
class Vector;
class Matrix;

//! @ingroup MATSCCRegiones
//
//! @brief Lista de regiones.
class ListRegiones: protected std::list<RegionSecc *>, public SeccionInerte
  {
  public:
    typedef std::list<RegionSecc *> l_reg;
    typedef l_reg::reference reference;
    typedef l_reg::const_reference const_reference;
    typedef l_reg::iterator iterator;
    typedef l_reg::const_iterator const_iterator;
  private:
    void libera(void);
    void libera(const size_t i);
    void copia(const ListRegiones &otra);
  protected:

    MaterialLoader *material_loader; //!< Material definition handler (searching,...).
  public:

    ListRegiones(MaterialLoader *ml);
    ListRegiones(const ListRegiones  &otro);
    ListRegiones &operator=(const ListRegiones  &otro);
    ~ListRegiones(void);

    XC::RegionSecc *push_back(const RegionSecc &reg);
    inline size_t size(void) const
      { return l_reg::size(); }
    inline bool empty(void) const
      { return l_reg::empty(); }

    void clear(void);

    const_iterator begin(void) const;
    const_iterator end(void) const;
    iterator begin(void);
    iterator end(void);

    RgSccQuad *newQuadRegion(const std::string &);
    RgSccCirc *newCircularRegion(const std::string &);

    size_t getNumCells(void) const;

    std::list<Poligono2d> getRegionsContours(void) const;
    std::list<Poligono2d> getContours(void) const;
    BND2d getBnd(void) const;
    ListRegiones Interseccion(const Semiplano2d &) const;

    //void Cumplen(const std::string &,ListRegiones &,bool );

    double getAreaGrossSection(void) const;
    Vector getCdgGrossSection(void) const;
    double getIyGrossSection(void) const;
    double getIzGrossSection(void) const;
    double getPyzGrossSection(void) const;

    Vector getCdgHomogenizedSection(const double &E0) const;
    double getAreaHomogenizedSection(const double &E0) const;
    double getIyHomogenizedSection(const double &E0) const;
    double getIzHomogenizedSection(const double &E0) const;
    double getPyzHomogenizedSection(const double &E0) const;

    void Print(std::ostream &s) const;
  };

 std::ostream &operator<<(std::ostream &os,const ListRegiones &lr);

} // end of XC namespace


#endif
