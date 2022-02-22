// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//BND2d.h

#ifndef BND2D_H
#define BND2D_H

#include "GeomObj2d.h"
#include "../cgal_types.h"

class Segment2d;
class Ray2d;
class Polygon2d;
class Polyline2d;


//! @ingroup GEOM
//
//! @brief "boundary" en dos dimensiones.
class BND2d: public GeomObj2d
  {
    CGIsoRectangle_2 cgrectg;
    bool undefined;
    template <class inputIterator>
    bool Overlap(inputIterator begin, inputIterator end) const;
  protected:
    inline bool In(const CGPoint_2 &p) const
      { return In(Pos2d(p)); }
    inline bool Overlap(const CGPoint_2 &p) const
      { return Overlap(Pos2d(p)); }     
  public:
    BND2d(void);
    BND2d(const Pos2d &p_min,const Pos2d &p_max);
    virtual GeomObj *getCopy(void) const
      { return new BND2d(*this); }
    inline bool isUndefined(void) const
      { return undefined; }
    inline virtual unsigned short int Dimension(void) const
    //Return the dimension of the object 0, 1, 2 or 3.
      { return 2; }
    //! @brief Return object width (x axis).
    inline virtual GEOM_FT getWidth(void) const
      { return GetXMax()-GetXMin(); }
    //! @brief Return object height (z axis).
    inline GEOM_FT getHeight(void) const
      { return GetYMax()-GetYMin(); }

    Polygon2d getPolygon(void) const;
    //! @brief Return the object length.
    inline virtual GEOM_FT getLength(void) const
      { return 2*getWidth()+2*getHeight(); }
    Vector2d Diagonal(void) const;
    virtual GEOM_FT getArea(void) const;
    //! @brief Return the volume of the object.
    inline virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    inline virtual GEOM_FT Pxy(void) const
      { return 0.0; }

    void Update(const Pos2d &);
    void PutPMax(const Pos2d &);
    void PutPMin(const Pos2d &);
    void PutPMinMax(const Pos2d &,const Pos2d &);
    Pos2d getPMax(void) const;
    Pos2d getPMin(void) const;
    inline GEOM_FT GetMax(unsigned short int i) const
      { return cgrectg.max_coord(i-1); }
    inline GEOM_FT GetMin(unsigned short int i) const
      { return cgrectg.min_coord(i-1); }

    Pos2d getCenterOfMass(void) const;

    bool In(const Pos2d &) const;
    template <class inputIterator>
    bool In(inputIterator begin, inputIterator end) const;
    bool In(const Polyline2d &) const;
    bool In(const Polygon2d &) const;
    bool Overlap(const Pos2d &) const;
    bool Overlap(const Line2d &r) const;
    bool Overlap(const Ray2d &sr) const;
    bool Overlap(const Segment2d &sg) const;
    bool Overlap(const BND2d &) const;
    bool Overlap(const Polyline2d &) const;
    bool Overlap(const Polygon2d &) const;
    bool Overlap(const std::list<Polygon2d> &) const;
    bool Interseca(const Line2d &r) const;
    bool Interseca(const Ray2d &sr) const;
    bool Interseca(const Segment2d &sg) const;
    bool Interseca(const BND2d &) const;

    void Transform(const Trf2d &trf2d);

    BND2d &operator+=(const Pos2d &p);
    BND2d &operator+=(const BND2d &a);
    friend BND2d operator+(const BND2d &a, const BND2d &b);
    friend bool operator==(const BND2d &a,const BND2d &b);
    void Print(std::ostream &stream) const;
    void Plot(Plotter &) const;
  };

inline bool intersecan(const Line2d &r,const BND2d &bnd)
  { return bnd.Interseca(r); }
inline bool intersecan(const Ray2d &sr,const BND2d &bnd)
  { return bnd.Interseca(sr); }
inline bool intersecan(const Segment2d &sg,const BND2d &bnd)
  { return bnd.Interseca(sg); }
inline bool intersecan(const BND2d &bnd,const Line2d &r)
  { return bnd.Interseca(r); }
inline bool intersecan(const BND2d &bnd,const Ray2d &sr)
  { return bnd.Interseca(sr); }
inline bool intersecan(const BND2d &bnd,const Segment2d &sg)
  { return bnd.Interseca(sg); }

//!@brief Return verdadero si todos los objetos de la secuencia
//! están contenidos en el BND.
template <class inputIterator>
bool BND2d::In(inputIterator begin, inputIterator end) const
  {
    bool retval= true;
    for(inputIterator i= begin;i!=end;i++)
      if(!In(*i))
        {
          retval= false;
          break;
        }
    return retval;
  }

//!@brief Return verdadero si alguno de los objetos de la secuencia
//! está contenido total o parcialmente en el polígono.
template <class inputIterator>
bool BND2d::Overlap(inputIterator begin, inputIterator end) const
  {
    bool retval= false;
    for(inputIterator i= begin;i!=end;i++)
      if(Overlap(*i))
        {
          retval= true;
          break;
        }
    return retval;
  }

#endif
