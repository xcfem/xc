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
//Pnt.h
//Objeto punto del modelador (similar a los K points de ANSYS)

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
//! @defgroup CadEnt Entidades geométricas.
//
//! @ingroup CadEnt
//!
//! @brief Punto (KPoint).
class Pnt: public EntMdlr
  {
    friend class Edge;
  private:
    Pos3d p; //!< Posición del punto.
    mutable std::set<const Edge *> lineas_pt; //!< Líneas uno de cuyos extremos es este punto:
  protected:
    virtual bool procesa_comando(CmdStatus &status);
    virtual void actualiza_topologia(void);
    void crea_nodos(void);
  public:
    Pnt(Preprocessor *m,const Pos3d &pto= Pos3d());
    Pnt(const std::string &nombre= "",Preprocessor *m= NULL,const Pos3d &pto= Pos3d());

    SetEstruct *getCopy(void) const;
    //! @brief Devuelve la dimensión del objeto.
    inline virtual unsigned short int GetDimension(void) const
      { return 0; }
    BND3d Bnd(void) const;
    bool tieneNodo(void) const;
    int getTagNode(void) const;
    Node *getNode(void);
    virtual void Malla(dir_mallado dm);

    //! @brief Devuelve la posición del objeto.
    const Pos3d &GetPos(void) const
      { return p; }
    //! @brief Devuelve la posición del objeto.
    Pos3d &Pos(void)
      { return p; }
    //! @brief Asigna la posición del objeto.
    void setPos(const Pos3d &pos)
      { p= pos; }
    Vector3d VectorPos(void) const;

    void inserta_linea(Edge *l) const;
    void borra_linea(Edge *l) const;

    //! @brief Devuelve la lista de líneas que tocan al punto.
    const std::set<const Edge *> &EdgesTocan(void) const
      { return lineas_pt; }
    const size_t getNLines(void) const
      { return EdgesTocan().size(); }
    std::set<const Edge *> EdgesExtremo(void) const;
    //! @brief Devuelve los nombres de las superficies que tocan a a línea.
    const std::string &NombresEdgesTocan(void) const;
    bool Toca(const Edge &l) const;
    bool Extremo(const Edge &l) const;
    bool Toca(const Face &s) const;
    bool Toca(const Body &b) const;
    double DistanciaA2(const Pos3d &pt) const;

    void Mueve(const std::vector<ExprAlgebra> &);
    void Transforma(const TrfGeom &trf);
    void Transforma(const size_t &indice_trf);

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    any_const_ptr GetProp(const std::string &cod) const;
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

} //fin namespace XC

#endif
