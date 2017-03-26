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
//Pnt.h
//Point entities (similar a los K points de ANSYS)

#ifndef PNT_H
#define PNT_H

#include "EntMdlr.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

namespace XC {
class Edge;
class Face;
class Body;
class TrfGeom;

//! @ingroup Cad
//
//! @defgroup CadEnt Geometric entities.
//
//! @ingroup CadEnt
//!
//! @brief Punto (KPoint).
class Pnt: public EntMdlr
  {
    friend class Edge;
  private:
    Pos3d p; //!< Position of the point.
    mutable std::set<const Edge *> lineas_pt; //!< Lines that begin or end in this point (topology).
  protected:

    virtual void actualiza_topologia(void);
    void crea_nodos(void);
  public:
    Pnt(Preprocessor *m,const Pos3d &pto= Pos3d());
    Pnt(const std::string &nombre= "",Preprocessor *m= nullptr,const Pos3d &pto= Pos3d());

    SetEstruct *getCopy(void) const;
    //! @brief Return the object dimension (0, 1, 2 or 3).
    inline virtual unsigned short int GetDimension(void) const
      { return 0; }
    BND3d Bnd(void) const;
    bool tieneNodo(void) const;
    int getTagNode(void) const;
    Node *getNode(void);
    virtual void genMesh(meshing_dir dm);

    //! @brief Returns the object position.
    const Pos3d &GetPos(void) const
      { return p; }
    //! @brief Returns the object position.
    Pos3d &Pos(void)
      { return p; }
    //! @brief Sets the object position.
    void setPos(const Pos3d &pos)
      { p= pos; }
    Vector3d VectorPos(void) const;

    void inserta_linea(Edge *l) const;
    void borra_linea(Edge *l) const;

    //! @brief Return the lista de líneas que tocan al punto.
    const std::set<const Edge *> &EdgesTocan(void) const
      { return lineas_pt; }
    const size_t getNLines(void) const
      { return EdgesTocan().size(); }
    std::set<const Edge *> EdgesExtremo(void) const;
    //! @brief Returns the names of the surfaces that touch the line (neighbors).
    const std::string &NombresEdgesTocan(void) const;
    bool Toca(const Edge &l) const;
    bool Extremo(const Edge &l) const;
    bool Toca(const Face &s) const;
    bool Toca(const Body &b) const;
    double DistanciaA2(const Pos3d &pt) const;

    void Mueve(const Vector3d &);
    void Transforma(const TrfGeom &trf);
    void Transforma(const size_t &indice_trf);

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);


  };

Vector &operator-(const Pnt &b,const Pnt &a);
const Edge *busca_edge_const_ptr_toca(const Pnt &,const Pnt &);
const Edge *busca_edge_const_ptr_toca(const Pnt &,const Pnt &,const Pnt &);
Edge *busca_edge_ptr_toca(const Pnt &,const Pnt &);
Edge *busca_edge_ptr_toca(const Pnt &,const Pnt &,const Pnt &);

Edge *busca_edge_ptr_extremos(const Pnt &pA,const Pnt &pB);
Edge *busca_edge_ptr_extremos(const Pnt &,const Pnt &,const Pnt &);
const Edge *busca_edge_const_ptr_extremos(const Pnt &pA,const Pnt &pB);
const Edge *busca_edge_const_ptr_extremos(const Pnt &,const Pnt &,const Pnt &);

} //end of XC namespace

#endif
