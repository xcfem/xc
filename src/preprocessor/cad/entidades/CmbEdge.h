//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

    BND3d Bnd(void) const;
    void Malla(dir_mallado dm);

    friend bool operator==(const Lado &il1,const Lado &il2);
 

 };

bool operator==(const CmbEdge::Lado &il1,const CmbEdge::Lado &il2);

} //end of XC namespace

#endif
