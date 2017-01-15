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
//DqMeshRegiones.h

#ifndef DqMeshRegion_h 
#define DqMeshRegion_h 

#include <deque>
#include <iostream>

namespace XC {

class MeshRegion;

//! @brief Vector de celdas.
class DqMeshRegion: protected std::deque<MeshRegion *>
  {
  public:
    typedef std::deque<MeshRegion *> dq_Reg;
    typedef dq_Reg::reference reference;
    typedef dq_Reg::const_reference const_reference;
    typedef dq_Reg::iterator iterator;
    typedef dq_Reg::const_iterator const_iterator;
  private:
    void libera(void);
    inline reference operator[](const size_t i)
      { return dq_Reg::operator[](i); }
  public:

    DqMeshRegion(void);
    DqMeshRegion(const DqMeshRegion  &otro);
    DqMeshRegion &operator=(const DqMeshRegion  &otro);
    ~DqMeshRegion(void);

    inline size_t size(void) const
      { return dq_Reg::size(); }

    void resize(const size_t sz);

    void add(const MeshRegion &c);
    inline const_reference operator[](const size_t i) const
      { return dq_Reg::operator[](i); }

    inline void clearAll(void)
      { libera(); }

    MeshRegion *getRegion(int tag);

    void Print(std::ostream &s) const;
  };

} // end of XC namespace


#endif
