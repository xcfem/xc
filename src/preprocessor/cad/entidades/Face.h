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
//Face.h
//Objeto superficie del modelador.

#ifndef FACE_H
#define FACE_H

#include "CmbEdge.h"

namespace XC {
class Body;

//! \ingroup CadEnt
//!
//! @brief Superficie.
class Face: public CmbEdge
  {
    friend class Edge;
    friend class Body;
    std::set<const Body *> cuerpos_sup; //!< Cuerpos uno de cuyos límites es esta superficie.

    void inserta_body(Body *b);
    Node *GetNodo(const size_t &i);
    const Node *GetNodo(const size_t &i) const;
  protected:
    size_t ndivj; //Número de divisiones en el eje j.
    virtual const Edge *get_lado_homologo(const Edge *l) const= 0;
    bool procesa_comando(CmdStatus &status);
  public:
    Face(void);
    Face(Preprocessor *m,const size_t &ndivI= 4, const size_t &ndivJ= 4);
    Face(const std::string &nombre,Preprocessor *m,const size_t &ndivI= 4, const size_t &ndivJ= 4);
     //! @brief Devuelve la dimension del objeto.
    inline virtual unsigned short int GetDimension(void) const
      { return 2; }
    void actualiza_topologia(void);
    inline size_t NDivI(void) const
      { return ndiv; }
    virtual void SetNDivI(const size_t &ndi);
    inline size_t NDivJ(void) const
      { return ndivj; }
    virtual void SetNDivJ(const size_t &ndj);
    virtual void ConciliaNDivIJ(void)= 0;
    //! @brief Devuelve el número de vértices.
    size_t NumVertices(void) const
      { return NumEdges(); }
    virtual const Pnt *GetVertice(const size_t &i) const;
    //! @brief Devuelve la lista de cuerpos que tocan a la superficie.
    const std::set<const Body *> &CuerposTocan(void) const
      { return cuerpos_sup; }
    size_t BordeComun(const Face &otra) const;
    int SentidoBorde(const Edge *l,const Face &otra) const;
    bool Toca(const Body &b) const;
    virtual bool checkNDivs(void) const= 0;

    virtual Node *GetNodo(const size_t &i1,const size_t &j,const size_t &k);
    virtual const Node *GetNodo(const size_t &i,const size_t &j,const size_t &k) const;
    virtual Node *GetNodo(const size_t &i,const size_t &j);
    virtual const Node *GetNodo(const size_t &i,const size_t &j) const;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    any_const_ptr GetProp(const std::string &cod) const;
  };

std::set<const Face *> GetSupsTocan(const Edge &l);

} //fin namespace XC.

#endif
