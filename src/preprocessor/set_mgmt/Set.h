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
#include "preprocessor/set_mgmt/SetEntities.h"

class Pos3d;

namespace XC {
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

  protected:
    std::string description; //!< set description.
    SetEntities entities; //!< points, lines, surfaces, bodies, ...

    void clearAll(void);
    void create_copy(const std::string &,const Vector3d &v);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);


    void mueve(const Vector3d &);
  public:
    Set(const std::string &nmb="",Preprocessor *preprocessor= nullptr);

    inline void setDescription(const std::string &d)
      { description= d; }
    const std::string &getDescription(void) const
      { return description; }

    void clear(void);

    //! @brief Returns a const reference to the entities container.
    virtual const SetEntities::lst_ptr_points &getPoints(void) const
      { return entities.getPoints(); }
    //! @brief Return a reference to the the point container.
    virtual SetEntities::lst_ptr_points &getPoints(void)
      { return entities.getPoints(); }
    //! @brief Assigns the points set.
    void setPoints(const SetEntities::lst_ptr_points &pts)
      { entities.setPoints(pts); }
    void sel_points_lista(const ID &);
    bool In(const Pnt *) const;
    Set pickPointsInside(const std::string &, const GeomObj3d &, const double &tol= 0.0);

    //! @brief Return a const reference to the line container.
    virtual const SetEntities::lst_line_pointers &getLines(void) const
      { return entities.getLines(); }
    //! @brief Return a reference to the line container.
    virtual SetEntities::lst_line_pointers &getLines(void)
      { return entities.getLines(); }
    //! @brief Assigns the edge set.
    void setLines(const SetEntities::lst_line_pointers &lns)
      { entities.setLines(lns); }
    void sel_lineas_lista(const ID &);
    bool In(const Edge *) const;
    Set pickLinesInside(const std::string &, const GeomObj3d &, const double &tol= 0.0);

    //! @brief Returns a const reference to the surface container.
    virtual const SetEntities::lst_surface_ptrs &getSurfaces(void) const
      { return entities.getSurfaces(); }
    //! @brief Returns a reference to the surface container.
    virtual SetEntities::lst_surface_ptrs &getSurfaces(void)
      { return entities.getSurfaces(); }
    //! @brief Assigns the surface set.
    void setSurfaces(const SetEntities::lst_surface_ptrs &sfs)
      { entities.setSurfaces(sfs); }
    void sel_surfaces_lst(const ID &);
    bool In(const Face *) const;
    Set pickSurfacesInside(const std::string &, const GeomObj3d &, const double &tol= 0.0);

    //! @brief Return a const reference to the body container.
    virtual const SetEntities::lst_ptr_cuerpos &getBodies(void) const
      { return entities.getBodies(); }
    //! @brief Return a reference to the body container.
    virtual SetEntities::lst_ptr_cuerpos &getBodies(void)
      { return entities.getBodies(); }
    //! @brief Assigns the bodies set.
    void setBodies(const SetEntities::lst_ptr_cuerpos &bds)
      { entities.setBodies(bds); }
    bool In(const Body *) const;
    Set pickBodiesInside(const std::string &, const GeomObj3d &, const double &tol= 0.0);

    //! @brief Return a const reference to the UniformGrids container.
    virtual const SetEntities::lst_ptr_uniform_grids &getUniformGrids(void) const
      { return entities.getUniformGrids(); }
    //! @brief Return a reference to the UniformGrids container.
    virtual SetEntities::lst_ptr_uniform_grids &getUniformGrids(void)
      { return entities.getUniformGrids(); }
    bool In(const UniformGrid *) const;

    void genMesh(meshing_dir dm);


    void fillUpwards(void);
    void fillDownwards(void);

    void Transforma(const TrfGeom &trf);
    void Transforma(const size_t &indice_trf);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    Set &operator+=(const Set &);
    Set &operator-=(const Set &);
    Set &operator*=(const Set &);
    
    Set operator+(const Set &) const;
    Set operator-(const Set &) const;
    Set operator*(const Set &) const;

    ~Set(void);
  };
} //end of XC namespace
#endif
