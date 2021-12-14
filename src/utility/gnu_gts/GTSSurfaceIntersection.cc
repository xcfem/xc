//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//GTSSurfaceIntersection.cc

#include "GTSSurfaceIntersection.h"
#include "GTSSurface.h"

using namespace std;
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

void GTSSurfaceIntersection::borra(void)
  {
    if(!borrar) return;
    if(si) gts_object_destroy(GTS_OBJECT (si));
    si= nullptr;
    borrar= false;
  }

GTSSurfaceIntersection::GTSSurfaceIntersection(GtsSurfaceInter *f)
  : si(f), borrar(false) {}

GTSSurfaceIntersection::GTSSurfaceIntersection(GtsSurfaceInterClass *klass,
                                               GTSSurface &s1,
                                               GTSSurface &s2)
  : si(nullptr), borrar(true)
  {
    /* build bounding box tree for first surface */
    GNode *tree1 = s1.BBoxTree();

    /* build bounding box tree for second surface */
    GNode *tree2 = s2.BBoxTree();

    si= gts_surface_inter_new( klass,s1.get_ptr(),s2.get_ptr(),
                               tree1,tree2,s1.IsOpen(),s2.IsOpen());
    /* destroy bounding box trees (including bounding boxes) */
    gts_bb_tree_destroy (tree1, TRUE);
    gts_bb_tree_destroy (tree2, TRUE);
  }
GTSSurfaceIntersection::GTSSurfaceIntersection(const GTSSurfaceIntersection &other)
  : si(other.si), borrar(false)
  {}
GTSSurfaceIntersection &GTSSurfaceIntersection::operator=(const GTSSurfaceIntersection &other)
  {
    si= other.si;
    borrar= false;
    return *this;
  }
GTSSurfaceIntersection::~GTSSurfaceIntersection(void)
  { borra(); }

bool GTSSurfaceIntersection::Check(bool &closed) const
// closed: is set to TRUE if si->edges is a closed curve, FALSE otherwise.
// Returns: TRUE if the curve described by si is an orientable manifold, FALSE otherwise.
  { 
    bool retval;
    gboolean c;
    retval= gts_surface_inter_check(si,&c);
    closed= c;
    return retval;
  }

//! @brief Append to the surface argument the part defined by THIS and
//! the op.
//! 
//! @param op: can be one of the following:
//!            - GTS_1_OUT_2: identifies the part of the first surface which
//!                           lies outside the second surface.
//!            - GTS_1_IN_2: identifies the part of the first surface which
//!                          lies inside the second surface.
//!            - GTS_2_OUT_1: identifies the part of the second surface which
//!                           lies outside the first surface.
//!            - GTS_2_IN_1: identifies the part of the second surface which
//!                          lies inside the first surface.
void GTSSurfaceIntersection::InterBoolean(GTSSurface &surface,GtsBooleanOperation op) const
  { gts_surface_inter_boolean(si,surface.get_ptr(),op); }


GTSSurface GTSSurfaceIntersection::bool_op(const std::string &str_op,GTSSurface &s2) const

  {
    GTSSurface retval;
    if (str_op=="union")
      {
        InterBoolean(retval, GTS_1_OUT_2);
        InterBoolean(retval, GTS_2_OUT_1);
      }
    else 
      if (str_op=="inter")
        {
          InterBoolean(retval, GTS_1_IN_2);
          InterBoolean(retval, GTS_2_IN_1);
        }
      else
        if (str_op=="diff")
          {
            InterBoolean(retval, GTS_1_OUT_2);
            InterBoolean(retval, GTS_2_IN_1);
            gts_surface_foreach_face (si->s2, (GtsFunc) gts_triangle_revert, nullptr);
            gts_surface_foreach_face (s2.get_ptr(), (GtsFunc) gts_triangle_revert, nullptr);
          }
        else
          {
	    std::cerr << "GTSSurfaceIntersection: la operación: " << str_op
		      << " es desconocida." << std::endl;
          }
    return retval;
  }

static void write_edge (GtsSegment * s, FILE * fp)
  {
    fprintf (fp, "VECT 1 2 0 2 0 %g %g %g %g %g %g\n",
	     GTS_POINT (s->v1)->x,
	     GTS_POINT (s->v1)->y,
             GTS_POINT (s->v1)->z,
	     GTS_POINT (s->v2)->x,
	     GTS_POINT (s->v2)->y,
	     GTS_POINT (s->v2)->z);
  }

void GTSSurfaceIntersection::Write(FILE *fp)
  {
    fprintf(fp,"LIST {\n");
    g_slist_foreach (si->edges, (GFunc) write_edge, fp);
    fprintf(fp,"}\n");
  }


GTSSurface BoolOp(const GTSSurfaceIntersection &si,GTSSurface &s1, GTSSurface &s2,const std::string &str_op,bool check_self_intersection,bool verbose)
  {
    GTSSurface retval;

    /* check that the surfaces are orientable manifolds */
    if (!s1.IsOrientable())
      {
        cerr << __FUNCTION__ << ": s1 surface is not orientable." << endl; 
        return retval;
      }
    if (!s2.IsOrientable())
      {
        cerr << __FUNCTION__ << ": s2 surface is not orientable." << endl; 
        return retval;
      }

    /* check that the surfaces are not self-intersecting */
    if (check_self_intersection)
      {
        if(s1.CheckSelfIntersection(verbose)) return retval;
        if(s2.CheckSelfIntersection(verbose)) return retval;
      }
    
    bool closed;
    g_assert(si.Check(closed));
    if(!closed)
      {
        cerr << __FUNCTION__
	     << "; the intersection of s1 and s2 is not a closed curve."
	     << endl;
        return retval;
      }
    retval= si.bool_op(str_op,s2);
  
    /* check that the resulting surface is not self-intersecting */
    if(check_self_intersection) retval.CheckSelfIntersection(verbose);

    return retval;
  }
