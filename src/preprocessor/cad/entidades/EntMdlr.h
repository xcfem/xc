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
//EntMdlr.h
//Multiblock topology entity.

#ifndef ENTMDLR_H
#define ENTMDLR_H

#include "preprocessor/set_mgmt/SetEstruct.h"
#include "preprocessor/cad/matrices/TritrizPtrNod.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/ParamMallado.h"

class BND3d;
class Pos3d;
class MatrizPos3d;
class TritrizPos3d;
class RangoIndice;
class RangoTritriz;

namespace XC {
class SetFilaI;
class SetFilaJ;
class SetFilaK;

//!  \ingroup Cad
//! 
//!  \brief Multiblock topology object (point, líne, face, block,...).
class EntMdlr: public SetEstruct
  {
  private:
    size_t idx; //!< @brief Índice del objeto (se emplea sólo para numerar entidades para VTK).
    bool genMalla; //!< Verdadero si el punto ha de mallarse (crear un nodo). Por ejemplo es falso cuando es un punto intermedio de una línea.
  protected:
    TritrizPtrNod nodos;
    TritrizPtrElem elementos;
    friend class Set;
    friend class SetMeshComp;
    friend class Cad;
    virtual void actualiza_topologia(void)= 0;
    void crea_nodos(const TritrizPos3d &);
    Node *crea_nodo(const Pos3d &pos,size_t i=1,size_t j=1, size_t k=1);
    bool crea_elementos(dir_mallado dm);
    Pnt *crea_punto(const Pos3d &);
    void crea_puntos(const MatrizPos3d &);
    SetEstruct *crea_set_fila(const RangoTritriz &,const std::string &);

    void clearAll(void);
  public:
    EntMdlr(Preprocessor *m,const size_t &i= 0);
    EntMdlr(const std::string &nombre= "",const size_t &i= 0,Preprocessor *m=NULL);
    EntMdlr(const EntMdlr &otro);
    EntMdlr &operator=(const EntMdlr &otro);

    virtual void set_indice(const size_t &i);
    //! @brief Devuelve el índice del objeto.
    inline size_t getIdx(void) const
      { return idx; }

    inline bool TieneNodos(void) const
      { return !nodos.empty(); }
    virtual size_t GetNumCapasNodos(void) const
      { return nodos.GetCapas(); }
    virtual size_t GetNumFilasNodos(void) const
      { return nodos.getNumFilas(); }
    virtual size_t GetNumColsNodos(void) const
      { return nodos.getNumCols(); }
    virtual size_t GetNumCapasElementos(void) const
      { return elementos.GetCapas(); }
    virtual size_t GetNumFilasElementos(void) const
      { return elementos.getNumFilas(); }
    virtual size_t GetNumColsElementos(void) const
      { return elementos.getNumCols(); }

    virtual Node *GetNodo(const size_t &i=1,const size_t &j=1,const size_t &k=1);
    virtual const Node *GetNodo(const size_t &i=1,const size_t &j=1,const size_t &k=1) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;
    ID getNodeIndices(const Node *) const;
    virtual Element *GetElemento(const size_t &i=1,const size_t &j=1,const size_t &k=1);
    virtual const Element *GetElemento(const size_t &i=1,const size_t &j=1,const size_t &k=1) const;
    Node *buscaNodo(const int &tag);
    const Node *buscaNodo(const int &tag) const;
    std::vector<int> getTagsNodos(void) const;

    Element *buscaElemento(const int &tag);
    const Element *buscaElemento(const int &tag) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;

    TritrizPtrNod &GetTtzNodos(void)
      { return nodos; }
    const TritrizPtrNod &GetTtzNodos(void) const
      { return nodos; }
    TritrizPtrElem &GetTtzElementos(void)
      { return elementos; }
    const TritrizPtrElem &GetTtzElementos(void) const
      { return elementos; }
    //! @brief Devuelve la dimensión del objeto.
    virtual unsigned short int GetDimension(void) const= 0;
    virtual BND3d Bnd(void) const= 0;

    SetFilaI GetVarRefFilaI(size_t f=1,size_t c=1,const std::string &nmb="tmp");
    SetFilaI GetVarRefFilaI(const RangoIndice &rango_capas,size_t f,size_t c,const std::string &nmb="tmp");
    SetFilaI GetVarRefFilaI(const RangoTritriz &rango,const std::string &nmb="tmp");

    SetFilaJ GetVarRefFilaJ(size_t capa=1,size_t c=1,const std::string &nmb="tmp");
    SetFilaJ GetVarRefFilaJ(size_t capa,const RangoIndice &rango_filas,size_t c,const std::string &nmb="tmp");
    SetFilaJ GetVarRefFilaJ(const RangoTritriz &rango,const std::string &nmb="tmp");

    SetFilaK GetVarRefFilaK(size_t capa=1,size_t f=1,const std::string &nmb="tmp");
    SetFilaK GetVarRefFilaK(size_t capa,size_t f,const RangoIndice &rango_cols,const std::string &nmb="tmp");
    SetFilaK GetVarRefFilaK(const RangoTritriz &rango,const std::string &nmb="tmp");

    void fix(const SFreedom_Constraint &);

    virtual int getMEDCellType(void) const;
    virtual int getVtkCellType(void) const;

    void setGenMalla(bool m);
    const bool &getGenMalla(void) const;

    virtual std::set<SetBase *> get_sets(void) const= 0;

    virtual double DistanciaA2(const Pos3d &pt) const;
    virtual double DistanciaA(const Pos3d &pt) const;

    Vector getSimpsonWeights(const std::string &,const std::string &,const size_t &f=1,const size_t &c=1, const size_t &n= 10) const;

    void BorraPtrNodElem(void);


    virtual ~EntMdlr(void);
  };

} //end of XC namespace

#endif
