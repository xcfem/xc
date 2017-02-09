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
//DqPtrsElem.h
//deque de punteros a elemento.


#ifndef DQPTRSELEM_H
#define DQPTRSELEM_H

#include "DqPtrs.h"
#include "domain/mesh/element/utils/KDTreeElements.h"

class Polilinea3d;

namespace XC {
class TrfGeom;

//!  \ingroup Set
//! 
//!  \brief Contenedor de punteros a elementos.
//! 
class DqPtrsElem: public DqPtrs<Element>
  {
    KDTreeElements kdtreeElements; //!< space-partitioning data structure for organizing elements (element centroids).
    inline iterator begin(void)
      { return DqPtrs<Element>::begin(); }
    inline iterator end(void)
      { return DqPtrs<Element>::end(); }
  protected:
    void crea_arbol(void);
  public:
    typedef DqPtrs<Element>::const_iterator const_iterator;
    typedef DqPtrs<Element>::iterator iterator;
    typedef DqPtrs<Element>::reference reference;
    typedef DqPtrs<Element>::const_reference const_reference;
    typedef DqPtrs<Element>::size_type size_type;

    DqPtrsElem(EntCmd *owr= nullptr);
    DqPtrsElem(const DqPtrsElem &otro);
    explicit DqPtrsElem(const std::deque<Element *> &ts);
    explicit DqPtrsElem(const std::set<const Element *> &ts);
    DqPtrsElem &operator=(const DqPtrsElem &otro);
    void agrega(const DqPtrsElem &otro);
    //void agrega_cond(const DqPtrsElem &otro,const std::string &cond);
    bool push_back(Element *);
    bool push_front(Element *);
    void clearAll(void);
    inline const_iterator begin(void) const
      { return DqPtrs<Element>::begin(); }
    inline const_iterator end(void) const
      { return DqPtrs<Element>::end(); }

    size_t getNumLiveElements(void) const;
    size_t getNumDeadElements(void) const;
    void kill_elements(void);
    void alive_elements(void);

    std::set<int> getTags(void) const;

    void calc_resisting_force(void);

    Element *buscaElemento(const int &tag);
    const Element *buscaElemento(const int &tag) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;
    std::deque<Polilinea3d> getContours(bool undeformedGeometry= true) const;

    void numera(void);
  };

} //end of XC namespace
#endif

