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


#ifndef DQPTRSELEM_H
#define DQPTRSELEM_H

#include "DqPtrsKDTree.h"
#include "domain/mesh/element/utils/KDTreeElements.h"

class Polyline3d;
class GeomObj3d;
class BND3d;

namespace XC {
class TrfGeom;

//!  @ingroup Set
//! 
//!  \brief Pointer to element container.
//! 
class DqPtrsElem: public DqPtrsKDTree<Element,KDTreeElements>
  {
    KDTreeElements kdtreeElements; //!< space-partitioning data structure for organizing elements (element centroids).
  public:
    DqPtrsElem(CommandEntity *owr= nullptr);
    DqPtrsElem(const DqPtrsElem &);
    explicit DqPtrsElem(const std::deque<Element *> &ts);
    explicit DqPtrsElem(const std::set<const Element *> &ts);
    DqPtrsElem &operator=(const DqPtrsElem &);

    size_t getNumLiveElements(void) const;
    size_t getNumDeadElements(void) const;
    void kill_elements(void);
    void alive_elements(void);

    std::set<int> getTags(void) const;

    void calc_resisting_force(void);

    Element *findElement(const int &);
    const Element *findElement(const int &) const;
    BND3d Bnd(const double &) const;    
    std::deque<Polyline3d> getContours(const double &factor= 0.0) const;
    DqPtrsElem pickElemsInside(const GeomObj3d &, const double &tol= 0.0);
    std::set<std::string> getMaterialNames(void) const;
    boost::python::list getMaterialNamesPy(void) const;
    std::set<std::string> getTypes(void) const;
    boost::python::list getTypesPy(void) const;
    std::set<size_t> getDimensions(void) const;
    boost::python::list getDimensionsPy(void) const;
    DqPtrsElem pickElemsOfType(const std::string &);
    DqPtrsElem pickElemsOfDimension(const size_t &);
    DqPtrsElem pickElemsOfMaterial(const std::string &);
    void numera(void);
    double getAverageSize(bool initialGeometry= true) const;
    
    void createInertiaLoads(const Vector &);
  };

DqPtrsElem operator+(const DqPtrsElem &a,const DqPtrsElem &b);
DqPtrsElem operator-(const DqPtrsElem &a,const DqPtrsElem &b);
DqPtrsElem operator*(const DqPtrsElem &a,const DqPtrsElem &b);

} //end of XC namespace
#endif

