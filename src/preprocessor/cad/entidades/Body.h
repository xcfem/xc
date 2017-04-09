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
//Body.h
//Class for block entities. (XXX: rename as Block?)

#ifndef BODY_H
#define BODY_H

#include "EntMdlr.h"
#include "CmbEdge.h"


class Pos3d;

namespace XC {

class Face;
class Pnt;

//! \ingroup CadEnt
//!
//! @brief Orden en de los lados de una cara del hexaedro.
struct SecuenciaLados
  {
    size_t l1; //!< Índice lado 1.
    size_t l2; //!< Índice lado 2.
    size_t l3; //!< Índice lado 3.
    size_t l4; //!< Índice lado 4.
    bool dirt; //!< Directo o inverso.

    SecuenciaLados(const size_t primero= 1,const bool &directo= true);
    //! @brief Returns true if the edge sequence is direct (edge1 -> edge4).
    const bool &Directo(void) const
      { return dirt; }
  };

//! \ingroup CadEnt
//!
//! @brief Six faces solid.
class Body: public EntMdlr
  {
  protected:
    //! \ingroup Geom
    //!
    //! @brief Surface that limits the body (face as seen by the body).
    class BodyFace
      {
        Face *surface; //!< Face geometry.
        SecuenciaLados sec_lados; //!< Edge sequence.
      public:
        BodyFace(Face *ptr= nullptr,const size_t &p=1,const bool &d=true);
	Face *Surface(void);
        const Face *Surface(void) const;
        void SetSurf(Face *s);

        const std::string &GetNombre(void) const;
        bool Vacia(void) const;
        size_t NumLineas(void) const;
        size_t NumVertices(void) const;
        const CmbEdge::Lado *GetLado(const size_t &) const;
        CmbEdge::Lado *GetLado(const size_t &);
        const Pnt *GetVertice(const size_t &) const;
        Pnt *GetVertice(const size_t &);
        virtual MatrizPos3d get_posiciones(void) const;
        void create_nodes(void);
        bool checkNDivs(void) const;
        Node *GetNodo(const size_t &,const size_t &);
      };

    void set_surf(Face *s);

    virtual BodyFace *GetFace(const size_t &i)= 0;
  public:
    Body(Preprocessor *m,const std::string &nombre= "");
    //! @brief Return the object dimension.
    inline virtual unsigned short int GetDimension(void) const
      { return 3; }
    //! @brief Returns the number of lines of the object.
    virtual size_t NumLineas(void) const= 0;
    //! @brief Returns the number of vertices of the object.
    virtual size_t NumVertices(void) const= 0;
    //! @brief Returns the number of vertices of the object.
    virtual size_t NumFaces(void) const= 0;
    virtual std::set<const Face *> getSurfaces(void)= 0;
    virtual const BodyFace *GetFace(const size_t &i) const= 0;
    virtual const CmbEdge::Lado *GetArista(const size_t &i) const= 0;
    virtual const Pnt *GetVertice(const size_t &i) const= 0;
    std::vector<int> getIndicesVertices(void) const;
    virtual BND3d Bnd(void) const;

    virtual bool checkNDivs(void) const= 0;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);


  };

std::set<const Body *> GetCuerposTocan(const Face &s);

} //end of XC namespace

#endif
