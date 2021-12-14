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
//GTSSurfaceIntersection.h
//C++ wrapper para la clase GtsFace de la biblioteca GTS.

#ifndef GTS_SURFACEINTERSECTION_H
#define GTS_SURFACEINTERSECTION_H

#include "gts.h"
#include <string>

class GTSSurface;

class GTSSurfaceIntersection
  {
    GtsSurfaceInter *si;

    bool borrar;

    void borra(void);

  protected:
    GtsSurfaceInter const *get_const_ptr(void) const
      { return si; }
    GtsSurfaceInter *get_ptr(void)
      { return si; }
    GTSSurface bool_op(const std::string &operation,GTSSurface &s2) const;

  public:

    GTSSurfaceIntersection(GtsSurfaceInter *f= nullptr);
    GTSSurfaceIntersection(GtsSurfaceInterClass *klass,
                           GTSSurface &s1, GTSSurface &s2);
    GTSSurfaceIntersection(const GTSSurfaceIntersection &);
    GTSSurfaceIntersection &operator=(const GTSSurfaceIntersection &);

    void Write(FILE *fp);

    bool Check(bool &closed) const;

    void InterBoolean(GTSSurface &surface,GtsBooleanOperation op) const;

    ~GTSSurfaceIntersection(void);

    friend GTSSurface BoolOp(const GTSSurfaceIntersection &si,GTSSurface &s1, GTSSurface &s2,const std::string &str_op,bool check_self_intersection,bool verbose);
  };

GTSSurface BoolOp(const GTSSurfaceIntersection &,GTSSurface &, GTSSurface &,const std::string &,bool check_self_intersection,bool verbose= false);

#endif
