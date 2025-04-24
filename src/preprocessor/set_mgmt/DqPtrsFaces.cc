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
//DqPtrsFaces.cc

#include "DqPtrsFaces.h"

XC::DqPtrsFaces::DqPtrsFaces(CommandEntity *owr)
  : DqPtrsEntities<Face>(owr) {}
XC::DqPtrsFaces::DqPtrsFaces(const DqPtrs<Face> &other)
  : DqPtrsEntities<Face>(other) {}
XC::DqPtrsFaces::DqPtrsFaces(const std::deque<Face *> &ts)
  : DqPtrsEntities<Face>(ts) {}
XC::DqPtrsFaces::DqPtrsFaces(const std::set<const Face *> &ts)
  : DqPtrsEntities<Face>(ts) {}

//! @brief Reverse the faces.
void XC::DqPtrsFaces::reverse(void)
  {
    for(iterator i= this->begin();i!= this->end();i++)
      {
        Face *t= (*i);
        assert(t);
	t->reverse();
      }
  }

//! @brief Return the average of the I vectors of the surfaces.
Vector3d XC::DqPtrsFaces::getIVectorAverage(void) const
  {
    Vector3d retval;
    for(const_iterator i= this->begin();i!= this->end();i++)
      {
        Face *t= (*i);
	retval+= t->getIVector();
      }
    retval*= 1.0/this->size();
    return retval;
  }

//! @brief Return the average of the J vectors of the surfaces.
Vector3d XC::DqPtrsFaces::getJVectorAverage(void) const
  {
    Vector3d retval;
    for(const_iterator i= this->begin();i!= this->end();i++)
      {
        Face *t= (*i);
	retval+= t->getJVector();
      }
    retval*= 1.0/this->size();
    return retval;
  }

//! @brief Return the average of the K vectors of the surfaces.
Vector3d XC::DqPtrsFaces::getKVectorAverage(void) const
  {
    Vector3d retval;
    for(const_iterator i= this->begin();i!= this->end();i++)
      {
        Face *t= (*i);
	retval+= t->getKVector();
      }
    retval*= 1.0/this->size();
    return retval;
  }

//! @brief Set the orientation of the face normals according to the vector
//! argument. If the dot product of the vector and the surface normal vector
//! is negative the method flips the surface.
//! @param v: orientation vector.
void XC::DqPtrsFaces::setKOrientation(const Vector3d &v)
  {
    for(iterator i= this->begin();i!= this->end();i++)
      {
        Face *t= (*i);
        assert(t);
	t->setKOrientation(v);
      }
  }

//! @brief -= (difference) operator.
XC::DqPtrsFaces &XC::DqPtrsFaces::operator-=(const DqPtrsFaces &other)
  {
    remove(other);
    return *this;
  }

//! @brief *= (intersection) operator.
XC::DqPtrsFaces &XC::DqPtrsFaces::operator*=(const DqPtrsFaces &other)
  {
    intersect(other);
    return *this;
  }

//! @brief Return a container with the entities that lie inside the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::DqPtrsFaces XC::DqPtrsFaces::pickSurfacesInside(const GeomObj3d &geomObj, const double &tol) const
  {
    DqPtrsEntities<Face> tmp= DqPtrsEntities<Face>::pickEntitiesInside(geomObj, tol);
    return DqPtrsFaces(tmp);
  }

//! @brief Return the edge closest to the given point.
XC::Face *XC::DqPtrsFaces::getNearest(const Pos3d &p)
  { return DqPtrsEntities<Face>::getNearest(p); }

//! @brief Return the edge closest to the given point.
const XC::Face *XC::DqPtrsFaces::getNearest(const Pos3d &p) const
  { return DqPtrsEntities<Face>::getNearest(p); }
