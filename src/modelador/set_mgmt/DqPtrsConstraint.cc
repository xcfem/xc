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

#include "DqPtrsConstraint.h"
#include "domain/constraints/Constraint.h"
#include "modelador/cad/trf/TrfGeom.h"
#include "xc_basic/src/funciones/algebra/ExprAlgebra.h"


//! @brief Constructor.
XC::DqPtrsConstraint::DqPtrsConstraint(void)
  : DqPtrs<Constraint>() {}

//! @brief Constructor de copia.
XC::DqPtrsConstraint::DqPtrsConstraint(const std::deque<Constraint *> &ts)
  : DqPtrs<Constraint>(ts) {}

//! @brief Constructor de copia.
XC::DqPtrsConstraint::DqPtrsConstraint(const std::set<const Constraint *> &st)
  : DqPtrs<Constraint>()
  {
    std::set<const Constraint *>::const_iterator k;
    k= st.begin();
    for(;k!=st.end();k++)
      push_back(const_cast<Constraint *>(*k));
  }

//! @brief Devuelve, si lo encuentra, un puntero al elemento
//! cuyo tag se pasa como parámetro.
XC::Constraint *XC::DqPtrsConstraint::buscaConstrainto(const int &tag)
  {
    Constraint *retval= nullptr;
    Constraint *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          {
            if(tag == tmp->getTag())
              {
                retval= tmp;
                break;
              }
          }
      }
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero al elemento
//! cuyo tag se pasa como parámetro.
const XC::Constraint *XC::DqPtrsConstraint::buscaConstrainto(const int &tag) const
  {
    const Constraint *retval= nullptr;
    const Constraint *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          {
            if(tag == tmp->getTag())
              {
                retval= tmp;
                break;
              }
          }
      }
    return retval;
  }

//!  @brief Asigna índices a los objetos de la lista para poder emplearlos en VTK.
void XC::DqPtrsConstraint::numera(void)
  {
    size_t idx= 0;
    for(iterator i= begin();i!=end();i++,idx++)
      {
	Constraint *ptr= *i;
        ptr->set_indice(idx);
      }
  }

//! @brief Devuelve los tags de los elementos.
std::set<int> XC::DqPtrsConstraint::getTags(void) const
  {
    std::set<int> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }
