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
//Edge.h

#ifndef EDGE_H
#define EDGE_H

#include "EntMdlr.h"


class BND3d;
class MatrizPos3d;

namespace XC {
class Pnt;
class Face;
class Body;

//! \ingroup CadEnt
//!
//! @brief Clase base para los objetos unidimensionales del preprocesador.
class Edge: public EntMdlr
  {
    friend class CmbEdge;
    friend class Face;
  private:
    size_t ndiv; //!< Número de divisiones
    std::set<const Face *> sups_linea; //!< Superficies uno de cuyos bordes es esta línea (topología).
  protected:

    void inserta_surf(Face *s);

    virtual Pnt *P1(void);
    virtual Pnt *P2(void);
    void crea_nodos_en_extremos(void);
  public:
    Edge(Preprocessor *m,const size_t &nd= 4);
    Edge(const std::string &nombre= "",Preprocessor *m= NULL,const size_t &nd= 4);
    //! @brief Devuelve la dimensión del objeto.
    inline virtual unsigned short int GetDimension(void) const
      { return 1; }

    virtual const Pnt *P1(void) const;
    virtual const Pnt *P2(void) const;
    bool ExtremosEn(const Pnt *,const Pnt *) const;
    //! @brief Devuelve el número de vértices.
    virtual size_t NumVertices(void) const= 0;

    virtual double getLongitud(void) const= 0;

    virtual const Pnt *GetVertice(const size_t &i) const= 0;
    virtual void SetVertice(const size_t &,Pnt *)= 0;
    std::vector<int> getIndicesVertices(void) const;
    virtual ID getKPoints(void) const;

    virtual void SetNDiv(const size_t &);
    inline virtual size_t NDiv(void) const
      { return ndiv; }
    void SetElemSize(const double &sz);
    std::set<const XC::Edge *> GetLadosHomologos(const std::set<const XC::Edge *> &) const;
    void actualiza_topologia(void);

    //! @brief Devuelve la lista de superficies que tocan a a línea.
    const std::set<const Face *> &SupsTocan(void) const
      { return sups_linea; }
    //! @brief Devuelve los nombres de las superficies que tocan a a línea.
    const std::string &NombresSupsTocan(void) const;
    bool Toca(const Face &s) const;
    bool Toca(const Body &b) const;
    bool Extremo(const Pnt &) const;
    virtual BND3d Bnd(void) const= 0;
    virtual void crea_nodos(void);
    virtual void genMesh(meshing_dir dm);

    virtual MatrizPos3d get_posiciones(void) const= 0;
    virtual MatrizPos3d get_pos_nodos(void) const;

    virtual Node *GetNodo(const size_t &i1,const size_t &j,const size_t &k=1);
    virtual const Node *GetNodo(const size_t &i,const size_t &j,const size_t &k=1) const;
    virtual Node *GetNodo(const size_t &i);
    virtual const Node *GetNodo(const size_t &i) const;
    Node *GetNodoDir(const size_t &i);
    const Node *GetNodoDir(const size_t &i) const;
    Node *GetNodoInv(const size_t &i);
    const Node *GetNodoInv(const size_t &i) const;
    Node *GetPrimerNodo(void);
    const Node *GetPrimerNodo(void) const;
    Node *GetUltimoNodo(void);
    const Node *GetUltimoNodo(void) const;

    std::vector<int> GetTagsNodosDir(void) const;
    std::vector<int> GetTagsNodosInv(void) const;
    MatrizPos3d GetPosNodosDir(void) const;
    MatrizPos3d GetPosNodosInv(void) const;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    virtual const Vector &getTang(const double &) const;
    void divide(void);

  };

std::set<const Edge *> GetLineasTocan(const Pnt &p);
size_t calcula_ndiv_lados(const std::set<const XC::Edge *> &);

} //end of XC namespace
#endif
