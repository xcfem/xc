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
//DqPtrsElem.h
//deque de punteros a elemento.


#ifndef DQPTRSELEM_H
#define DQPTRSELEM_H

#include "DqPtrs.h"
#include "domain/mesh/element/KDTreeElements.h"

class ExprAlgebra;

namespace XC {
class TrfGeom;

//!  \ingroup Set
//! 
//!  \brief Contenedor de punteros a elementos.
//! 
class DqPtrsElem: public DqPtrs<Element>
  {
    KDTreeElements kdtreeElements; //!< KDTree para acelerar la búsqueda de elementos por coordenadas.
    inline iterator begin(void)
      { return DqPtrs<Element>::begin(); }
    inline iterator end(void)
      { return DqPtrs<Element>::end(); }
  protected:
    void crea_arbol(void);
  public:
    typedef DqPtrs<Element>::const_iterator const_iterator;
    typedef DqPtrs<Element>::iterator iterator;
    typedef DqPtrs<Element>::reference reference;
    typedef DqPtrs<Element>::const_reference const_reference;
    typedef DqPtrs<Element>::size_type size_type;

    DqPtrsElem(EntCmd *owr= nullptr);
    DqPtrsElem(const DqPtrsElem &otro);
    explicit DqPtrsElem(const std::deque<Element *> &ts);
    explicit DqPtrsElem(const std::set<const Element *> &ts);
    DqPtrsElem &operator=(const DqPtrsElem &otro);
    void agrega(const DqPtrsElem &otro);
    //void agrega_cond(const DqPtrsElem &otro,const std::string &cond);
    bool push_back(Element *);
    bool push_front(Element *);
    void clearAll(void);
    inline const_iterator begin(void) const
      { return DqPtrs<Element>::begin(); }
    inline const_iterator end(void) const
      { return DqPtrs<Element>::end(); }

    size_t getNumLiveElements(void) const;
    size_t getNumDeadElements(void) const;
    void kill_elements(void);
    void alive_elements(void);

    std::set<int> getTags(void) const;

    void calc_resisting_force(void);

    Element *buscaElemento(const int &tag);
    const Element *buscaElemento(const int &tag) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;

    void numera(void);
  };

} //end of XC namespace
#endif

