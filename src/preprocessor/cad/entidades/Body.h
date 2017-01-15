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
    //! @brief Devuelve verdadero si la secuencia de lados es directa.
    const bool &Directo(void) const
      { return dirt; }
  };

//! \ingroup CadEnt
//!
//! @brief Representación de un sólido de seis caras.
class Body: public EntMdlr
  {
  protected:
    //! \ingroup Geom
    //!
    //! @brief Esta clase sirve para almacenar cada una de las superficies que limitan el cuerpo.
    class Cara
      {
        Face *superficie; //!< Puntero a superficie.
        SecuenciaLados sec_lados; //!< Secuencia de lados.
      public:
        Cara(Face *ptr=NULL,const size_t &p=1,const bool &d=true);
	Face *Superficie(void);
        const Face *Superficie(void) const;
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
        void crea_nodos(void);
        bool checkNDivs(void) const;
        Node *GetNodo(const size_t &,const size_t &);
      };

    void set_surf(Face *s);

    virtual Cara *GetCara(const size_t &i)= 0;
  public:
    Body(Preprocessor *m,const std::string &nombre= "");
    //! @brief Devuelve la dimensión del objeto
    inline virtual unsigned short int GetDimension(void) const
      { return 3; }
    //! @brief Devuelve el número de líneas del objeto.
    virtual size_t NumLineas(void) const= 0;
    //! @brief Devuelve el número de vértices del objeto.
    virtual size_t NumVertices(void) const= 0;
    //! @brief Devuelve el número de vértices del objeto.
    virtual size_t NumCaras(void) const= 0;
    virtual std::set<const Face *> GetSuperficies(void)= 0;
    virtual const Cara *GetCara(const size_t &i) const= 0;
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
