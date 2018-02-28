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
//VectorReinfBars.h

#ifndef VectorReinfBar_h 
#define VectorReinfBar_h 

#include <vector>
#include <iostream>
#include "material/section/repres/SectionMassProperties.h"

class Pos2d;

namespace XC {

class ReinfBar;
class Vector;
class Matrix;

//! @ingroup MATSCCArmaduras
//
//! @brief Vector de barras de armadura.
class VectorReinfBar: public std::vector<ReinfBar *>, public SectionMassProperties
  {
  public:
    typedef std::vector<ReinfBar *> v_rfBar;
    typedef v_rfBar::reference reference;
    typedef v_rfBar::const_reference const_reference;
    typedef v_rfBar::iterator iterator;
    typedef v_rfBar::const_iterator const_iterator;
  private:
    void free_mem(void);
    void free_mem(const size_t i);
    void alloc(const size_t i,const ReinfBar &c);
    inline reference operator[](const size_t i)
      { return v_rfBar::operator[](i); }
  public:
    VectorReinfBar(const size_t &sz= 0);
    VectorReinfBar(const VectorReinfBar  &otro);
    VectorReinfBar &operator=(const VectorReinfBar  &otro);
    ~VectorReinfBar(void);

    inline size_t size(void) const
      { return v_rfBar::size(); }
    inline size_t empty(void) const
      { return v_rfBar::empty(); }
    inline iterator begin(void)
      { return v_rfBar::begin(); }
    inline iterator end(void)
      { return v_rfBar::end(); }
    inline const_iterator begin(void) const
      { return v_rfBar::begin(); }
    inline const_iterator end(void) const
      { return v_rfBar::end(); }

    double getAreaGrossSection(void) const;
    Vector getCdgGrossSection(void) const;
    double getIyGrossSection(void) const;
    double getIzGrossSection(void) const;
    double getPyzGrossSection(void) const;

    double getAreaHomogenizedSection(const double &E0) const;
    Vector getCdgHomogenizedSection(const double &E0) const;
    double getIyHomogenizedSection(const double &E0) const;
    double getIzHomogenizedSection(const double &E0) const;
    double getPyzHomogenizedSection(const double &E0) const;

    void resize(const size_t sz);

    void put(const size_t i,const ReinfBar &c);
    inline const_reference operator[](const size_t i) const
      { return v_rfBar::operator[](i); }

    void Print(std::ostream &s) const;

  };

} // end of XC namespace


#endif
