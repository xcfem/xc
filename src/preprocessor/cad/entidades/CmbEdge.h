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
//Compound line.

#ifndef CMBEDGE_H
#define CMBEDGE_H

#include <deque>
#include "Edge.h"

class Polilinea3d;

namespace XC {
//! \ingroup CadEnt
//!
//! @brief Compound line.
class CmbEdge: public Edge
  {

  public:

    //! \ingroup Geom
    //!
    //! @brief Component of a compound line.
    //!
    //! A compound line is a sequence of pointers to lines (Linea) (we
    //! call them lados here), such that the end of one of them is the origin
    //! of the edge that follows. Since the orientation of the line defined
    //! in the model, can not coincide with the orientation needed here, each
    //! pointer to line is accompanied by a boolean that indicates if the 
    //! line must be taken with its reversed direction or not.
    class Lado: public EntCmd
      {
        Edge *edge; //!< Pointer to line.
        bool directo; //!< true: direct (P1->P2), false: reversed (P2->P1)
      protected:
    
      public:
        Lado(Edge *ptr= nullptr,const bool &s=true);
        Edge *getEdge(void);
        const Edge *getEdge(void) const;
        void SetEdge(Edge *l);
        const Pnt *P1(void) const;
        const Pnt *P2(void) const;
        bool ExtremosEn(const Pnt *,const Pnt *) const;
        const std::string &getName(void) const;
        inline bool esDirecto(void) const
          { return directo; }
        inline void reverse(void)
	  { directo= !directo; }
        void SetNDiv(const size_t &nd);
        size_t NDiv(void) const;
        size_t GetTag(void) const;
        MatrizPos3d get_posiciones(void) const;
        std::vector<int> getTagsNodesDir(void) const;
        std::vector<int> getTagsNodesInv(void) const;
        MatrizPos3d getNodePosDir(void) const;
        MatrizPos3d getNodePosInv(void) const;
        double getLongitud(void) const;
        Pos3d getCentroid(void) const;	
        bool In(const GeomObj3d &, const double &tol= 0.0) const;
        bool Out(const GeomObj3d &, const double &tol= 0.0) const;
        const Vector &getTang(const double &) const;
        Node *getNodeDir(const size_t &i);
        Node *getNodeInv(const size_t &i);
        Node *getNode(const size_t &i);
        const Node *getNode(const size_t &i) const;
        void genMesh(meshing_dir dm);
        friend bool operator==(const Lado &il1,const Lado &il2);
    
      };
  protected:
    std::deque<Lado> lineas; //!< Lines that compose the object.
    MatrizPos3d get_posiciones(void) const;

    Lado *primera_linea(void);
    const Lado *primera_linea(void) const;
    Lado *ultima_linea(void);
    const Lado *ultima_linea(void) const;


    const Pnt *first_point(void) const;
    const Pnt *last_point(void) const;

  protected:

    void create_nodes_lineas(void);
    void line_meshing(meshing_dir dm);
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

    //! @brief Return the number of edges.
    size_t NumEdges(void) const
      { return lineas.size(); }
    inline std::deque<Lado> &getLados(void)
      { return lineas; }
    void reverse(void);
    bool In(const GeomObj3d &, const double &tol= 0.0) const;
    bool Out(const GeomObj3d &, const double &tol= 0.0) const;

    double getLongitud(void) const;
    Pos3d getCentroid(void) const;	

    //! @brief Return the number of vertices.
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
    Polilinea3d getPolyline(void) const;

    BND3d Bnd(void) const;
    void genMesh(meshing_dir dm);

    friend bool operator==(const Lado &il1,const Lado &il2);
 

 };

bool operator==(const CmbEdge::Lado &il1,const CmbEdge::Lado &il2);

} //end of XC namespace

#endif
