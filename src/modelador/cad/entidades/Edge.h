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
//! @brief Clase base para los objetos unidimensionales del modelador.
class Edge: public EntMdlr
  {
    friend class CmbEdge;
    friend class Face;
  private:
    size_t ndiv; //!< Número de divisiones
    std::set<const Face *> sups_linea; //!< Superficies uno de cuyos bordes es esta línea (topología).
  protected:

    void inserta_surf(Face *s);

    virtual const Node *get_node_ptr_from_stack(const std::string &cod) const;
    virtual const Element *get_element_ptr_from_stack(const std::string &cod) const;
    virtual bool procesa_comando(CmdStatus &status);
    virtual Pnt *P1(void);
    virtual Pnt *P2(void);
    void crea_nodos_en_extremos(void);
  public:
    Edge(Modelador *m,const size_t &nd= 4);
    Edge(const std::string &nombre= "",Modelador *m= NULL,const size_t &nd= 4);
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
    virtual void Malla(dir_mallado dm);

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

    any_const_ptr GetProp(const std::string &cod) const;
  };

std::set<const Edge *> GetLineasTocan(const Pnt &p);
size_t calcula_ndiv_lados(const std::set<const XC::Edge *> &);

} //fin namespace XC
#endif
