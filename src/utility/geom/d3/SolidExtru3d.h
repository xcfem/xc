// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//SolidExtru3d.h

#ifndef SOLIDEXTRU3D_H
#define SOLIDEXTRU3D_H

#include "Solid3d.h"
#include "BND3d.h"
#include "../FT_matrix.h"
#include "../pos_vec/Pos3d.h"


//! @ingroup GEOM
//
//! @brief SolidExtru3d, sólidos de extrusión.
//! La base inferior está a (z=-l/2)
//! y la base superior (z=+l/2).
template<class S>
class SolidExtru3d : public Solid3d
  {
  protected:
    S scc; //!< Surface that create the solid.
    GEOM_FT l; //!< Lentgh of the extruded solid (negative if undefined).
  public:
    SolidExtru3d(void): Solid3d(), scc(),l(1.0) {}
    SolidExtru3d(const S &secc,const GEOM_FT &lng): scc(secc), l(lng) {}
    explicit SolidExtru3d(const SolidExtru3d<S> &se)
      : Solid3d(se), scc(se.scc), l(se.l) {}
    SolidExtru3d<S> &operator=(const SolidExtru3d<S> &se)
      {
	Solid3d::operator=(se);
        scc=se.scc;
        l= se.l;
        return *this;
      }
    virtual GeomObj *clon(void) const
      { return new SolidExtru3d<S>(*this); }
    void Offset(const GEOM_FT &offset)
      //Similar al offset de AutoCad.
      {
        scc.Offset(offset);
        l+=2.0*offset;
      }
    SolidExtru3d<S> GetOffset(const GEOM_FT &offset) const
      {
        SolidExtru3d<S> retval(this);
        retval.Offset(offset);
        return retval;
      }
    //! @brief Return the extruded section.
    const S &Section(void) const
      { return scc; }
    //! @brief Return the extruded section.
    S &Section(void)
      { return scc; }
    //! @brief Return the object area.
    inline GEOM_FT getArea(void) const
      {
        GEOM_FT area= 2*scc.getArea();
        area+= scc.getLength()*l;
        return area;
      }
    //! @brief Return object length.
    inline GEOM_FT getLength(void) const
      { return l; }
    //! @brief Return the object volume
    inline GEOM_FT getVolume(void) const
      { return scc.getArea() * l; }
    inline virtual GEOM_FT Ix(void) const
      { return scc.Ix()*l + scc.getArea()*l*l*l/12; }
    inline virtual GEOM_FT Iy(void) const
      { return scc.Iy()*l + scc.getArea()*l*l*l/12; }
    inline virtual GEOM_FT Iz(void) const
      { return (scc.Ix()+scc.Iy())*l; }
/*     inline Pos3d getCenterOfMassOfBase(unsigned short int i) const */
/*     //Return the center of mass of one of the bases. */
/*     //0 bottom base (z=-l/2) and 1 top */
/*     //base (z=+l/2). */
/*       {  */
/*         if(i) */
/*           return scc.getGlobalPosition(Pos3d(0,0,l/2.0)); */
/*         else */
/*           return scc.getGlobalPosition(Pos3d(0,0,-l/2.0)); */
/*       } */
/*     inline Pos3d getCenterOfMassOfTop(void) const */
/*       { return getCenterOfMassOfBase(1); } */
/*     inline Pos3d getCenterOfMassOfBottom(void) const */
/*       { return getCenterOfMassOfBase(0); } */
/*     inline S GetBase(unsigned short int i) const */
/*     //0 la base inferior (z=-l/2) y 1 la */
/*     //base superior (z=+l/2). */
/*       {  */
/*         Pos3d p= getCenterOfMassOfBase(i); */
/*         S secc= scc; */
/*         secc.setOrg(p); */
/*         return secc; */
/*       } */
    //! @brief Return the maximum value of the i-th coordinate.
    inline virtual GEOM_FT GetMax(unsigned short int i) const
      { 
        //return max(GetBase(0).GetMax(i),GetBase(1).GetMax(i));
	std::cerr << "SolidExtru3d::" << __FUNCTION__
	          << "not implemented." << std::endl;
        return 0.0;
      }
    //! @brief Return the minimum value of the i-th coordinate.
    inline virtual GEOM_FT GetMin(unsigned short int i) const
      {
        //return min(GetBase(0).GetMin(i),GetBase(1).GetMin(i));
	std::cerr << "SolidExtru3d::" << __FUNCTION__
	          << "not implemented." << std::endl;
        return 0.0;
      }
    BND3d Bnd(void) const
    //Return la extension of the object.
      { return GeomObj3d::Bnd(); }
    FT_matrix I(void) const
      { return GeomObj3d::I(); }
    Pos3d getCenterOfMass(void) const
      { return scc.getCenterOfMass(); }
  };

#endif






