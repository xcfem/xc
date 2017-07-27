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
//Set.h

#ifndef SET_H
#define SET_H

#include "preprocessor/set_mgmt/SetMeshComp.h"
#include "DqPtrsNmb.h"
#include <set>

class Pos3d;

namespace XC {
class Pnt;
class Edge;
class Face;
class Body;
class UniformGrid;
class TrfGeom;
class SFreedom_Constraint;
class ID;
class Element;
class Node;
class Constraint;

//!  @ingroup Set
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
class Set: public SetMeshComp
  {
  public:
    void numera(void);
    typedef DqPtrsNmb<Pnt> lst_ptr_points; //!< Point set.
    typedef lst_ptr_points::iterator pnt_iterator; //!< point set iterator.
    typedef lst_ptr_points::const_iterator pnt_const_iterator; //!< point set const iterator.

    typedef DqPtrsNmb<Edge> lst_line_pointers; //!< Line set.
    typedef lst_line_pointers::iterator lin_iterator; //!< Line set iterator.
    typedef lst_line_pointers::const_iterator lin_const_iterator; //!< Line set const iterator.

    typedef DqPtrsNmb<Face> lst_surface_ptrs; //!< surface set.
    typedef lst_surface_ptrs::iterator sup_iterator; //!< surface set iterator.
    typedef lst_surface_ptrs::const_iterator sup_const_iterator; //!< surface set const iterator.

    typedef DqPtrsNmb<Body> lst_ptr_cuerpos; //!< body set.

    typedef DqPtrsNmb<UniformGrid> lst_ptr_uniform_grids; //!< Unifrom grid set.

  protected:
    std::string description; //!< set description.
    lst_ptr_points puntos; //!< point set.
    lst_line_pointers lineas; //!< line set.
    lst_surface_ptrs surfaces; //!< surface set.
    lst_ptr_cuerpos cuerpos; //!< body set.
    lst_ptr_uniform_grids uniform_grids; //! Uniform mesh set.

    void clearAll(void);
    void copia_listas(const Set &);
    void create_copy(const std::string &,const Vector3d &v);

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
    Set(const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    Set(const Set &otro);
    Set &operator=(const Set &otro);

    inline void setDescription(const std::string &d)
      { description= d; }
    const std::string &getDescription(void) const
      { return description; }

    void clear(void);

    //! @brief Returns a const reference to the point container.
    virtual const lst_ptr_points &GetPuntos(void) const
      { return puntos; }
    //! @brief Return a reference to the the point container.
    virtual lst_ptr_points &GetPuntos(void)
      { return puntos; }
    void sel_points_lista(const ID &);
    bool In(const Pnt *) const;

    //! @brief Return a const reference to the line container.
    virtual const lst_line_pointers &GetLineas(void) const
      { return lineas; }
    //! @brief Return a reference to the line container.
    virtual lst_line_pointers &GetLineas(void)
      { return lineas; }
    void sel_lineas_lista(const ID &);
    bool In(const Edge *) const;

    //! @brief Returns a const reference to the surface container.
    virtual const lst_surface_ptrs &getSurfaces(void) const
      { return surfaces; }
    //! @brief Returns a reference to the surface container.
    virtual lst_surface_ptrs &getSurfaces(void)
      { return surfaces; }
    void sel_surfaces_lst(const ID &);
    bool In(const Face *) const;

    //! @brief Return a const reference to the body container.
    virtual const lst_ptr_cuerpos &GetCuerpos(void) const
      { return cuerpos; }
    //! @brief Return a reference to the body container.
    virtual lst_ptr_cuerpos &GetCuerpos(void)
      { return cuerpos; }
    bool In(const Body *) const;

    //! @brief Return a const reference to the UniformGrids container.
    virtual const lst_ptr_uniform_grids &GetUniformGrids(void) const
      { return uniform_grids; }
    //! @brief Return a reference to the UniformGrids container.
    virtual lst_ptr_uniform_grids &GetUniformGrids(void)
      { return uniform_grids; }
    bool In(const UniformGrid *) const;

    void genMesh(meshing_dir dm);

    void extend_lists(const Set &);

    void CompletaHaciaArriba(void);
    void CompletaHaciaAbajo(void);

    void Transforma(const TrfGeom &trf);
    void Transforma(const size_t &indice_trf);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    Set &operator+=(const Set &);
    Set operator+(const Set &) const;

    ~Set(void);
  };
} //end of XC namespace
#endif
