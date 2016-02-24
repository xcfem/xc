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
//DqPtrs.h
//deque de punteros (se emplear en la clase Set).


#ifndef DQPTRS_H
#define DQPTRS_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/nucleo/EntPropSorter.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include <deque>
#include <set>
#include "utility/actor/actor/MovableID.h"
#include <boost/iterator/indirect_iterator.hpp>



namespace XC {

//!  \ingroup Set
//! 
//!  \brief Contenedor de punteros a objetos (nodos, elementos, puntos, lineas,...).
//! 
//!  Un objeto DqPtrs contiene un conjunto de 0 o más punteros a objetos de UNO de estos tipos:
//!  - Nodos.
//!  - Elementos finitos.
//!  - Bordes/aristas de elementos finitos.
//!  - Caras de elementos finitos.
//!  - Puntos.
//!  - Líneas.
//!  - Superficies.
//!  - Cuerpos.
template <class T>
class DqPtrs: public EntCmd, protected std::deque<T *>
  {
  public:
    typedef typename std::deque<T *> lst_ptr;
    typedef typename lst_ptr::const_iterator const_iterator;
    typedef typename lst_ptr::iterator iterator;
    typedef typename lst_ptr::reference reference;
    typedef typename lst_ptr::const_reference const_reference;
    typedef typename lst_ptr::size_type size_type;
    typedef boost::indirect_iterator<iterator> indIterator;
  public:
    DqPtrs(EntCmd *owr= nullptr);
    DqPtrs(const DqPtrs &otro);
    explicit DqPtrs(const std::deque<T *> &ts);
    explicit DqPtrs(const std::set<const T *> &ts);
    DqPtrs &operator=(const DqPtrs &otro);
    void agrega(const DqPtrs &otro);
    void agrega_cond(const DqPtrs &otro,const std::string &cond);
    bool push_back(T *);
    bool push_front(T *);
    inline bool empty(void) const
      { return lst_ptr::empty(); }
    inline iterator begin(void)
      { return lst_ptr::begin(); }
    const_iterator begin(void) const
      { return lst_ptr::begin(); }
    iterator end(void)
      { return lst_ptr::end(); }
    const_iterator end(void) const
      { return lst_ptr::end(); }
    inline indIterator indBegin(void)
      { return indIterator(lst_ptr::begin()); }
    inline indIterator indEnd(void)
      { return indIterator(lst_ptr::end()); }
    void clear(void);
    void clearAll(void);
    inline size_type size(void) const
      { return lst_ptr::size(); }
    bool in(const T *) const;
    void sort_on_prop(const std::string &cod,const bool &ascending= true);

    const ID &getTags(void) const;
    template <class InputIterator>
    void insert(iterator pos, InputIterator f, InputIterator l)
      { lst_ptr::insert(pos,f,l); }

    
    int sendTags(int posSz,int posDbTag,DbTagData &dt,CommParameters &cp);
    const ID &receiveTags(int posSz,int pDbTg,DbTagData &dt,const CommParameters &cp);

  };

//! @brief Constructor.
template <class T>
DqPtrs<T>::DqPtrs(EntCmd *owr)
  : EntCmd(owr),lst_ptr() {}

//! @brief Constructor de copia.
template <class T>
DqPtrs<T>::DqPtrs(const DqPtrs<T> &otro)
  : EntCmd(otro), lst_ptr(otro)
  {}

//! @brief Constructor de copia.
template <class T>
DqPtrs<T>::DqPtrs(const std::deque<T *> &ts)
  : EntCmd(), lst_ptr(ts)
  {}

//! @brief Constructor de copia.
template <class T>
DqPtrs<T>::DqPtrs(const std::set<const T *> &st)
  : EntCmd(), lst_ptr()
  {
    typename std::set<const T *>::const_iterator k;
    k= st.begin();
    for(;k!=st.end();k++)
      lst_ptr::push_back(const_cast<T *>(*k));
  }

//! @brief Operator asignación.
template <class T>
DqPtrs<T> &DqPtrs<T>::operator=(const DqPtrs &otro)
  {
    EntCmd::operator=(otro);
    lst_ptr::operator=(otro);
    return *this;
  }

//! @brief Agrega a ésta lista los elementos de la que se le pasa como parámetro.
template <class T>
void DqPtrs<T>::agrega(const DqPtrs &otro)
  {
    for(register const_iterator i= otro.begin();i!=otro.end();i++)
      push_back(*i);
  }

//! @brief Agrega a ésta lista los elementos de la que se le pasa como parámetro,
//! si cumplen la condición.
template <class T>
void DqPtrs<T>::agrega_cond(const DqPtrs &otro,const std::string &cond)
  {
    bool result= false;
    for(register const_iterator i= otro.begin();i!=otro.end();i++)
      {
        result= (*i)->interpretaBool(cond);
        if(result)
	  push_back(*i);
      }
  }


//! @brief Vacía la lista de punteros.
template<class T>
void DqPtrs<T>::clear(void)
  { lst_ptr::clear(); }

//! @brief Vacía la lista de punteros y elimina las propiedades que pudiera tener el objeto.
template<class T>
void DqPtrs<T>::clearAll(void)
  {
    clear();
    EntCmd::clearProps();
  }

//! @brief Devuelve verdadero si el puntero está en el contenedor.
template<class T>
bool DqPtrs<T>::in(const T *ptr) const
  {
    bool retval= false;
    for(const_iterator i= begin();i!= end();i++)
      if(*i==ptr)
        {
          retval= true;
          break;
        }
    return retval;
  }


template <class T>
bool DqPtrs<T>::push_back(T *t)
  {
    bool retval= false;
    if(t)
      {
        if(find(begin(),end(),t) == end()) //El elemento es nuevo.
          {
            lst_ptr::push_back(t);
            retval= true;
          }
      }
    else
      std::cerr << "DqPtrs::push_back; se intentó insertar un puntero nulo." << std::endl;
    return retval;
  }

template <class T>
bool DqPtrs<T>::push_front(T *t)
  {
    bool retval= false;
    if(t)
      {
        if(find(begin(),end(),t) == end()) //El elemento es nuevo.
          {
            lst_ptr::push_front(t);
            retval= true;
          }
      }
    else
      std::cerr << "DqPtrs::push_front; se intentó insertar un puntero nulo." << std::endl;
    return retval;
  }

//! @brief Ordena los elementos del contenedor por
//! el valor de la propiedad que se pasa como parámetro.
template <class T>
void DqPtrs<T>::sort_on_prop(const std::string &cod,const bool &ascending)
  {
    EntProp::set_sorter_cod(cod);
    if(ascending)
      std::sort(begin(),end(),EntProp::menor_ent_prop);
    else
      std::sort(begin(),end(),EntProp::mayor_ent_prop);
  }

//! @brief Devuelve los Tags de los objetos.
template <class T>
const ID &DqPtrs<T>::getTags(void) const
  {
    static ID retval;
    const int sz= size();
    if(sz>0)
      {
        retval.resize(sz);
        int loc =0;
        // loop over objs in deque adding their dbTag to the ID
        for(const_iterator i= begin();i!=end();i++)
          {
            retval(loc)= (*i)->getTag();
            loc++;
          }
      }
    return retval;
  }

//! @brief Envía los dbTags de los conjuntos través del canal que se pasa como parámetro.
template <class T>
int DqPtrs<T>::sendTags(int posSz,int posDbTag,DbTagData &dt,CommParameters &cp)
  {
    const int sz= size();
    int res= cp.sendInt(sz,dt,CommMetaData(posSz));
    if(sz>0)
      {
        const ID &tags= getTags();
        res+= cp.sendID(tags,dt,CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << "DqPtrs<T>::sendDbTags - failed to send the IDs.\n";
    return res;
  }

//! @brief Recibe los dbTags de los conjuntos a través del canal que se pasa como parámetro.
template <class T>
const ID &DqPtrs<T>::receiveTags(int posSz,int posDbTag,DbTagData &dt,const CommParameters &cp)
  {
    static ID retval;
    int sz= 0;
    int res= cp.receiveInt(sz,dt,CommMetaData(posSz));
    if(sz>0)
      {
        retval.resize(sz);
        res= cp.receiveID(retval,dt,CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << "DqPtrs<T>::receiveTags - failed to receive the IDs.\n";
    return retval;
  }

} //end of XC namespace


#endif

