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

//!  \ingroup Set
//! 
//!  \brief Conjunto de objetos.
//! 
//!  A set object contains 0 or more:
//!  - Nodes.
//!  - Finite elements.
//!  - Points.
//!  - Línes.
//!  - Surfaces.
//!  - Bodies.
//!  - Uniform meshes.
class Set: public SetMeshComp
  {
  public:
    void numera(void);
    typedef DqPtrsNmb<Pnt> lst_ptr_puntos; //!< Point set.
    typedef lst_ptr_puntos::iterator pnt_iterator; //!< point set iterator.
    typedef lst_ptr_puntos::const_iterator pnt_const_iterator; //!< point set const iterator.

    typedef DqPtrsNmb<Edge> lst_ptr_lineas; //!< Conjunto de líneas.
    typedef lst_ptr_lineas::iterator lin_iterator; //!< line set iterator.
    typedef lst_ptr_lineas::const_iterator lin_const_iterator; //!< line set const iterator.

    typedef DqPtrsNmb<Face> lst_ptr_superficies; //!< surface set.
    typedef lst_ptr_superficies::iterator sup_iterator; //!< surface set iterator.
    typedef lst_ptr_superficies::const_iterator sup_const_iterator; //!< surface set const iterator.

    typedef DqPtrsNmb<Body> lst_ptr_cuerpos; //!< body set.

    typedef DqPtrsNmb<UniformGrid> lst_ptr_uniform_grids; //!< Unifrom grid set.

  protected:
    lst_ptr_puntos puntos; //!< point set.
    lst_ptr_lineas lineas; //!< line set.
    lst_ptr_superficies superficies; //!< surface set.
    lst_ptr_cuerpos cuerpos; //!< body set.
    lst_ptr_uniform_grids uniform_grids; //! Uniform mesh set.

    void clearAll(void);
    void copia_listas(const Set &);
    //void agrega_listas_cond(const Set &,const std::string &);
    void crea_copia(const std::string &,const Vector3d &v);

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
    Set(const std::string &nmb="",Preprocessor *preprocessor= NULL);
    Set(const Set &otro);
    Set &operator=(const Set &otro);

    void clear(void);

    //! @brief Devuelve la lista de puntos del conjunto.
    virtual const lst_ptr_puntos &GetPuntos(void) const
      { return puntos; }
    //! @brief Devuelve la lista de puntos del conjunto.
    virtual lst_ptr_puntos &GetPuntos(void)
      { return puntos; }
    void sel_puntos_lista(const ID &);
    bool In(const Pnt *) const;

    //! @brief Devuelve la lista de lineas del conjunto.
    virtual const lst_ptr_lineas &GetLineas(void) const
      { return lineas; }
    //! @brief Devuelve la lista de lineas del conjunto.
    virtual lst_ptr_lineas &GetLineas(void)
      { return lineas; }
    void sel_lineas_lista(const ID &);
    bool In(const Edge *) const;

    //! @brief Devuelve la lista de superficies del conjunto.
    virtual const lst_ptr_superficies &GetSuperficies(void) const
      { return superficies; }
    //! @brief Devuelve la lista de superficies del conjunto.
    virtual lst_ptr_superficies &GetSuperficies(void)
      { return superficies; }
    void sel_superficies_lista(const ID &);
    bool In(const Face *) const;

    //! @brief Devuelve la lista de cuerpos del conjunto.
    virtual const lst_ptr_cuerpos &GetCuerpos(void) const
      { return cuerpos; }
    //! @brief Devuelve la lista de cuerpos del conjunto.
    virtual lst_ptr_cuerpos &GetCuerpos(void)
      { return cuerpos; }
    bool In(const Body *) const;

    //! @brief Devuelve la lista de UniformGrids del conjunto.
    virtual const lst_ptr_uniform_grids &GetUniformGrids(void) const
      { return uniform_grids; }
    //! @brief Devuelve la lista de UniformGrids del conjunto.
    virtual lst_ptr_uniform_grids &GetUniformGrids(void)
      { return uniform_grids; }
    bool In(const UniformGrid *) const;

    void genMesh(meshing_dir dm);

    void agrega_listas(const Set &);

    void CompletaHaciaArriba(void);
    void CompletaHaciaAbajo(void);

    void Transforma(const TrfGeom &trf);
    void Transforma(const size_t &indice_trf);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);


    ~Set(void);
  };
} //end of XC namespace
#endif
