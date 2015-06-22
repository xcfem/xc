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
//ElementEdges.h
                                                                        
#ifndef ElementEdges_h
#define ElementEdges_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "domain/mesh/element/ElementEdge.h"

namespace XC {
class Element;
class Domain;
class ID;
 

//! @ingroup ElemLoads
//
//! @brief Conjunto de bordes (o aristas) de elemento que
//! recibirán la carga.
class ElementEdges: public EntCmd
  {
  public:
    typedef std::vector<ElementEdge> vector_edges;
    typedef vector_edges::const_reference const_reference;
    typedef vector_edges::reference reference;
    typedef vector_edges::iterator iterator;
    typedef vector_edges::const_iterator const_iterator;

  private:
    vector_edges theEdges; //!< conjunto de bordes.
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    ElementEdges(void);

    virtual void setPtrs(Domain *theDomain, const ID &theElementTags,const ID &theEdgeIndexes);

    virtual size_t removeElement(const int &tag);

    inline void push_back(const ElementEdge &ee)
      { theEdges.push_back(ee); }
    inline size_t size(void) const
      { return theEdges.size(); }
    inline const_reference front() const
      { return theEdges.front(); }
    inline reference front()
      { return theEdges.front(); }
    inline const_reference back() const
      { return theEdges.back(); }
    inline reference back()
      { return theEdges.back(); }
    inline const_iterator begin() const
      { return theEdges.begin(); }
    inline iterator begin()
      { return theEdges.begin(); }
    inline const_iterator end() const
      { return theEdges.end(); }
    inline iterator end()
      { return theEdges.end(); }
    iterator find(const int &tag);
    const_iterator find(const int &tag) const;
    int getLocElement(const Element *) const;

    const_reference operator()(const size_t &i) const;
    reference operator()(const size_t &i);
    const_reference operator[](const size_t &i) const;
    reference operator[](const size_t &i);

    any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif

