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
//DqPtrsNmb.h
//deque de punteros (se emplear en la clase Set).


#ifndef DQPTRSNMB_H
#define DQPTRSNMB_H

#include "DqPtrs.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "xc_utils/src/base/utils_any.h"


namespace XC {

template <class T>
class DqPtrsNmb: public DqPtrs<T>
  {
  public:
    typedef DqPtrs<T> dq_ptr;
    typedef typename dq_ptr::const_iterator const_iterator;
    typedef typename dq_ptr::iterator iterator;

    DqPtrsNmb(void)
      : DqPtrs<T>() {}
    DqPtrsNmb(const DqPtrs<T> &otro)
      : DqPtrs<T>(otro) {}
    explicit DqPtrsNmb(const std::deque<T *> &ts)
      : DqPtrs<T>(ts) {}
    explicit DqPtrsNmb(const std::set<const T *> &ts)
      : DqPtrs<T>(ts) {}

    T *BuscaNmb(const std::string &nmb);
    T *getNearest(const Pos3d &p);
    const T *getNearest(const Pos3d &p) const;
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

//! @brief Devuelve un puntero al objeto cuyo nombre se pasa como parámetro.
template <class T>
T *DqPtrsNmb<T>::BuscaNmb(const std::string &nmb)
  {
    for(const_iterator i= this->begin();i!=this->end();i++)
      if((*i)->GetNombre()==nmb) return *i;
    return NULL;
  }

//! @brief Devuelve el objeto más cercano a la posición que se pasa como parámetro.
template <class T>
T *DqPtrsNmb<T>::getNearest(const Pos3d &p)
  {
    T *retval= NULL;
    if(!this->empty())
      {
        const_iterator i= this->begin();
        double d2= (*i)->DistanciaA2(p);
        retval= *i; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i)->DistanciaA2(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= *i;
              }
          }
      }
    return retval;
  }

//! @brief Devuelve el objeto más cercano a la posición que se pasa como parámetro.
template <class T>
const T *DqPtrsNmb<T>::getNearest(const Pos3d &p) const
  {
    const T *retval= NULL;
    if(!this->empty())
      {
        const_iterator i= this->begin();
        double d2= (*i)->DistanciaA2(p);
        retval= *i; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i)->DistanciaA2(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= *i;
              }
          }
      }
    return retval;
  }

//! @brief Devuelve la propiedad del objeto cuyo nombre (de la propiedad) se pasa como parámetro.
template <class T>
any_const_ptr DqPtrsNmb<T>::GetProp(const std::string &cod) const
  {
    if(cod=="getTagNearest")
      {
        double x= 0.0;
        double y= 0.0;
        double z= 0.0;
        if(InterpreteRPN::Pila().size()>2)
          {
            z= convert_to_double(InterpreteRPN::Pila().Pop());
            y= convert_to_double(InterpreteRPN::Pila().Pop());
            x= convert_to_double(InterpreteRPN::Pila().Pop());
          }
        else
          this->err_num_argumentos(std::cerr,3,"GetProp",cod);
        const T *tmp= getNearest(Pos3d(x,y,z));
        DqPtrsNmb<T>::tmp_gp_int= tmp->GetTag();
        return any_const_ptr(DqPtrsNmb<T>::tmp_gp_int);
      }
    else
      return DqPtrs<T>::GetProp(cod);    
  }

} //end of XC namespace

#endif

