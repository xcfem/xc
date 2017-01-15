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
//CmbEdge.h
//Línea compuesta.

#ifndef CMBEDGE_H
#define CMBEDGE_H

#include <deque>
#include "Edge.h"


namespace XC {
//! \ingroup CadEnt
//!
//! @brief Línea compuesta.
class CmbEdge: public Edge
  {

  public:

    //! \ingroup Geom
    //!
    //! @brief Componente de una línea compuesta.
    //!
    //! Una Edge compuesta es una secuencia de punteros a líneas (Linea) (que
    //! aqui llamaremos lados), tal que el extremo de una de ellas es el
    //! origen de la siguiente. Como quiera que la orientación de la línea definida
    //! en el modelo, puede no coincidir con la necesaria para la secuencia, cada
    //! apuntador a línea se acompaña de un valor booleano que indica si debe
    //! interpretarse la línea con su sentido directo o con el inverso.
    class Lado: public EntCmd
      {
        Edge *edge; //!< Puntero a línea.
        bool directo; //!< true: directo (P1->P2), false: inverso (P2->P1)
      protected:
    
      public:
        Lado(Edge *ptr=NULL,const bool &s=true);
        Edge *Borde(void);
        const Edge *Borde(void) const;
        void SetEdge(Edge *l);
        const Pnt *P1(void) const;
        const Pnt *P2(void) const;
        bool ExtremosEn(const Pnt *,const Pnt *) const;
        const std::string &GetNombre(void) const;
        inline bool esDirecto(void) const
          { return directo; }
        inline void reverse(void)
	  { directo= !directo; }
        void SetNDiv(const size_t &nd);
        size_t NDiv(void) const;
        size_t GetTag(void) const;
        MatrizPos3d get_posiciones(void) const;
        std::vector<int> GetTagsNodosDir(void) const;
        std::vector<int> GetTagsNodosInv(void) const;
        MatrizPos3d GetPosNodosDir(void) const;
        MatrizPos3d GetPosNodosInv(void) const;
        double getLongitud(void) const;
        const Vector &getTang(const double &) const;
        Node *GetNodoDir(const size_t &i);
        Node *GetNodoInv(const size_t &i);
        Node *GetNodo(const size_t &i);
        const Node *GetNodo(const size_t &i) const;
        void Malla(dir_mallado dm);
        friend bool operator==(const Lado &il1,const Lado &il2);
    
      };
  protected:
    std::deque<Lado> lineas; //!< Líneas que componen el objeto.
    MatrizPos3d get_posiciones(void) const;

    Lado *primera_linea(void);
    const Lado *primera_linea(void) const;
    Lado *ultima_linea(void);
    const Lado *ultima_linea(void) const;


    const Pnt *primer_punto(void) const;
    const Pnt *ultimo_punto(void) const;

  protected:

    void crea_nodos_lineas(void);
    void malla_lineas(dir_mallado dm);
    Pnt *P1(void);
    Pnt *P2(void);
    void cierra(void); 
  public:
    CmbEdge(void);
    CmbEdge(Preprocessor *m,const size_t &nd= 4);
    CmbEdge(const std::string &nombre,Preprocessor *m,const size_t &nd);
    virtual SetEstruct *getCopy(void) const;
    virtual void inserta(Edge *l);
    void inserta(const size_t &i);
    Edge *NuevaLinea(Pnt *,Pnt *);
    Edge *NuevaLinea(Pnt *,Pnt *,Pnt *);
    void addPoints(const ID &);
    void addLines(const ID &);
    size_t NDiv(void) const;
    void SetNDiv(const size_t &nd);

    //! @brief Devuelve el número de lados.
    size_t NumEdges(void) const
      { return lineas.size(); }
    inline std::deque<Lado> &getLados(void)
      { return lineas; }
    void reverse(void);

    double getLongitud(void) const;

    //! @brief Devuelve el número de vértices.
    virtual size_t NumVertices(void) const
      { return NumEdges()+1; }

    const Pnt *P1(void) const;
    const Pnt *P2(void) const;

    size_t IndiceEdge(const Edge *l) const;
    const Lado *GetLado(const size_t &i) const;
    Lado *GetLado(const size_t &i);
    const Lado *GetLadoPorPuntos(const Pnt *,const Pnt *) const;
    Lado *GetLadoPorPuntos(const Pnt *,const Pnt *);
    const Lado *GetLadoPorPuntos(const size_t &,const size_t &) const;
    Lado *GetLadoPorPuntos(const size_t &,const size_t &);
    std::deque<Edge *> GetEdges(void);
    virtual const Pnt *GetVertice(const size_t &i) const;
    virtual void SetVertice(const size_t &,Pnt *);
    virtual ID getKPoints(void) const;

    BND3d Bnd(void) const;
    void Malla(dir_mallado dm);

    friend bool operator==(const Lado &il1,const Lado &il2);
 

 };

bool operator==(const CmbEdge::Lado &il1,const CmbEdge::Lado &il2);

} //end of XC namespace

#endif
