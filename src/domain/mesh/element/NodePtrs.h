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
//NodePtrs.h
                                                                        
#ifndef NodePtrs_h
#define NodePtrs_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include <list>

class Pos3d;
class SemiEspacio3d;
class Plano3d;

namespace XC {
class Element;
class Node;
class Domain;
class ID;
class Matrix;
class Vector; 

//! \ingroup Elem
//
//! @brief Punteros a nodos.
class NodePtrs: public EntCmd
  {
  public:
    typedef std::vector<Node *> vector_ptr_nodes;
    typedef vector_ptr_nodes::const_reference const_reference;
    typedef vector_ptr_nodes::reference reference;
    typedef vector_ptr_nodes::iterator iterator;
    typedef vector_ptr_nodes::const_iterator const_iterator;

  private:
    vector_ptr_nodes theNodes; //!< conjunto de Nodos.
  protected:
    void desconecta(void);
    void setPtrs(Domain *theDomain, const ID &theNodeTags);

  public:
    NodePtrs(Element *owr,const size_t &sz);
    NodePtrs(const NodePtrs &otro);
    NodePtrs &operator=(const NodePtrs &otro);
    ~NodePtrs(void);

    void inic(void);

    inline size_t size(void) const
      { return theNodes.size(); }
    inline const_reference front() const
      { return theNodes.front(); }
    inline reference front()
      { return theNodes.front(); }
    inline const_reference back() const
      { return theNodes.back(); }
    inline reference back()
      { return theNodes.back(); }
    inline const_iterator begin() const
      { return theNodes.begin(); }
    inline iterator begin()
      { return theNodes.begin(); }
    inline const_iterator end() const
      { return theNodes.end(); }
    inline iterator end()
      { return theNodes.end(); }
    iterator find(const int &);
    const_iterator find(const int &) const;
    int find(const Node *) const;

    Node *getNodePtr(const size_t &);
    const_reference operator()(const size_t &i) const;
    const_reference operator[](const size_t &i) const;
    void set_node(const size_t &i,Node *);

    bool hasNull(void) const;

    ID getNumDOFs(void) const;
    int getTotalDOFs(void) const;
    Vector getDistributionFactor(int mode) const;
    Matrix getNodeVectors(const Vector &) const;
    double MaxCooNod(int i) const;
    double MinCooNod(int i) const;
    std::vector<int> getIdx(void) const;
    const Matrix &getCoordinates(void) const;
    Pos3d getPosNodo(const size_t &i,bool initialGeometry= true) const;
    std::list<Pos3d> getPosiciones(bool initialGeometry= true) const;
    Pos3d getPosCdg(bool initialGeometry= true) const;
    Node *getNearestNode(const Pos3d &p,bool initialGeometry= true);
    const Node *getNearestNode(const Pos3d &p,bool initialGeometry= true) const;
    bool In(const SemiEspacio3d &semiEsp,const double &tol= 0.0,bool initialGeometry= true) const;
    bool Out(const SemiEspacio3d &semiEsp,const double &tol= 0.0,bool initialGeometry= true) const;
    bool Corta(const Plano3d &,bool initialGeometry= true) const;

    void resetTributarias(void) const;
    void vuelcaTributarias(const std::vector<double> &) const;

    const std::vector<int> &getTags(void) const;
    int getIndiceNodo(const Node *) const;


  };
} // end of XC namespace

#endif

