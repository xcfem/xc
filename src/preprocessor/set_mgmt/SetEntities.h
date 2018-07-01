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
//SetEntities.h

#ifndef SETENTITIES_H
#define SETENTITIES_H

#include "preprocessor/PreprocessorContainer.h"
#include "DqPtrsEntities.h"
#include "utility/actor/actor/MovableObject.h"
#include "preprocessor/set_mgmt/SetMeshComp.h"

class Pos3d;
class GeomObj3d;
class BND3d;

namespace XC {
class Pnt;
class Edge;
class Face;
class Body;
class UniformGrid;
class TrfGeom;
class SFreedom_Constraint;
class ID;

//!  @ingroup SetEntities
//! 
//!  @brief Object set.
//! 
//!  A set object contains 0 or more:
//!  - Nodes.
//!  - Finite elements.
//!  - Points.
//!  - Lines.
//!  - Surfaces.
//!  - Bodies.
//!  - Uniform meshes.
class SetEntities: public PreprocessorContainer, public MovableObject
  {
  public:
    typedef DqPtrsEntities<Pnt> lst_ptr_points; //!< Point set.
    typedef lst_ptr_points::iterator pnt_iterator; //!< point set iterator.
    typedef lst_ptr_points::const_iterator pnt_const_iterator; //!< point set const iterator.

    typedef DqPtrsEntities<Edge> lst_line_pointers; //!< Line set.
    typedef lst_line_pointers::iterator lin_iterator; //!< Line set iterator.
    typedef lst_line_pointers::const_iterator lin_const_iterator; //!< Line set const iterator.

    typedef DqPtrsEntities<Face> lst_surface_ptrs; //!< surface set.
    typedef lst_surface_ptrs::iterator sup_iterator; //!< surface set iterator.
    typedef lst_surface_ptrs::const_iterator sup_const_iterator; //!< surface set const iterator.

    typedef DqPtrsEntities<Body> lst_ptr_cuerpos; //!< body set.

    typedef DqPtrsEntities<UniformGrid> lst_ptr_uniform_grids; //!< Unifrom grid set.

  protected:
    lst_ptr_points points; //!< point set.
    lst_line_pointers lines; //!< line set.
    lst_surface_ptrs surfaces; //!< surface set.
    lst_ptr_cuerpos bodies; //!< body set.
    lst_ptr_uniform_grids uniform_grids; //! Uniform mesh set.

    friend class Set;
    void clearAll(void);
    void copy_lists(const SetEntities &);
    SetEntities create_copy(const std::string &,const Vector3d &v);
    void extend_lists(const SetEntities &);
    void substract_lists(const SetEntities &);
    void intersect_lists(const SetEntities &);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    //Mesh generation.
    void point_meshing(meshing_dir dm);
    void line_meshing(meshing_dir dm);
    void surface_meshing(meshing_dir dm);
    void body_meshing(meshing_dir dm);
    void uniform_grid_meshing(meshing_dir dm);

    void mueve(const Vector3d &);
  public:
    SetEntities(Preprocessor *preprocessor= nullptr);
    SetEntities(const SetEntities &);
    SetEntities &operator=(const SetEntities &);

    void clear(void);

    //! @brief Returns a const reference to the point container.
    virtual const lst_ptr_points &getPoints(void) const
      { return points; }
    //! @brief Return a reference to the the point container.
    virtual lst_ptr_points &getPoints(void)
      { return points; }
    //! @brief Assigns the points set.
    void setPoints(const lst_ptr_points &pts)
      { points= pts; }
    void sel_points_lista(const ID &);
    bool In(const Pnt *) const;
    SetEntities pickPointsInside(const GeomObj3d &, const double &tol= 0.0) const;

    //! @brief Return a const reference to the line container.
    virtual const lst_line_pointers &getLines(void) const
      { return lines; }
    //! @brief Return a reference to the line container.
    virtual lst_line_pointers &getLines(void)
      { return lines; }
    //! @brief Assigns the edge set.
    void setLines(const lst_line_pointers &lns)
      { lines= lns; }
    void sel_lines_list(const ID &);
    bool In(const Edge *) const;
    SetEntities pickLinesInside(const GeomObj3d &, const double &tol= 0.0) const;

    //! @brief Returns a const reference to the surface container.
    virtual const lst_surface_ptrs &getSurfaces(void) const
      { return surfaces; }
    //! @brief Returns a reference to the surface container.
    virtual lst_surface_ptrs &getSurfaces(void)
      { return surfaces; }
    //! @brief Assigns the surface set.
    void setSurfaces(const lst_surface_ptrs &sfs)
      { surfaces= sfs; }
    void sel_surfaces_lst(const ID &);
    bool In(const Face *) const;
    SetEntities pickSurfacesInside(const GeomObj3d &, const double &tol= 0.0) const;

    //! @brief Return a const reference to the body container.
    virtual const lst_ptr_cuerpos &getBodies(void) const
      { return bodies; }
    //! @brief Return a reference to the body container.
    virtual lst_ptr_cuerpos &getBodies(void)
      { return bodies; }
    //! @brief Assigns the bodies set.
    void setBodies(const lst_ptr_cuerpos &bds)
      { bodies= bds; }
    bool In(const Body *) const;
    SetEntities pickBodiesInside(const GeomObj3d &, const double &tol= 0.0) const;

    //! @brief Return a const reference to the UniformGrids container.
    virtual const lst_ptr_uniform_grids &getUniformGrids(void) const
      { return uniform_grids; }
    //! @brief Return a reference to the UniformGrids container.
    virtual lst_ptr_uniform_grids &getUniformGrids(void)
      { return uniform_grids; }
    bool In(const UniformGrid *) const;

    BND3d Bnd(void) const;
    SetEntities pickEntitiesInside(const GeomObj3d &, const double &tol= 0.0) const;

    void fillUpwards(const SetMeshComp &);
    void fillDownwards(SetMeshComp &);

    void Transforma(const TrfGeom &trf);
    void Transforma(const size_t &indice_trf);

    void genMesh(const std::string &, meshing_dir dm);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    SetEntities &operator+=(const SetEntities &);
    SetEntities &operator-=(const SetEntities &);
    SetEntities &operator*=(const SetEntities &);
    
    SetEntities operator+(const SetEntities &) const;
    SetEntities operator-(const SetEntities &) const;
    SetEntities operator*(const SetEntities &) const;

    ~SetEntities(void);
  };
} //end of XC namespace
#endif
