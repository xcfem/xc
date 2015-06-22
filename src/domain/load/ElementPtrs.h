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
//ElementPtrs.h
                                                                        
#ifndef ElementPtrs_h
#define ElementPtrs_h

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
class Element;
class Domain;
class ID;
 

//! @ingroup ElemLoads
//
//! @brief Punteros al conjunto de elementos que
//! recibirán la carga.
class ElementPtrs: public EntCmd
  {
  public:
    typedef std::vector<Element *> vector_ptr_elements;
    typedef vector_ptr_elements::const_reference const_reference;
    typedef vector_ptr_elements::reference reference;
    typedef vector_ptr_elements::iterator iterator;
    typedef vector_ptr_elements::const_iterator const_iterator;

  private:
    vector_ptr_elements theElements; //!< conjunto de elementos.
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    ElementPtrs(void);
    ~ElementPtrs(void);

    virtual void setPtrs(Domain *theDomain, const ID &theElementTags);

    virtual size_t removeElement(const int &tag);

    inline size_t size(void) const
      { return theElements.size(); }
    inline const_reference front() const
      { return theElements.front(); }
    inline reference front()
      { return theElements.front(); }
    inline const_reference back() const
      { return theElements.back(); }
    inline reference back()
      { return theElements.back(); }
    inline const_iterator begin() const
      { return theElements.begin(); }
    inline iterator begin()
      { return theElements.begin(); }
    inline const_iterator end() const
      { return theElements.end(); }
    inline iterator end()
      { return theElements.end(); }
    iterator find(const int &tag);
    const_iterator find(const int &tag) const;
    Element *findPtr(const int &tag);
    const Element *findPtr(const int &tag) const;

    const_reference operator()(const size_t &i) const;
    reference operator()(const size_t &i);
    const_reference operator[](const size_t &i) const;
    reference operator[](const size_t &i);

    any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif

