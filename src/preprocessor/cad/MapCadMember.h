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
//MapCadMember.h

#ifndef MAPCADMEMBER_H
#define MAPCADMEMBER_H

#include "preprocessor/cad/MapCadMemberBase.h"
#include <map>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "boost/python/list.hpp"

namespace XC {

//! @ingroup Cad
//
//! @brief Contenedor de entidades del modelo.
template <class T>
 class MapCadMember: public MapCadMemberBase, public std::map<MapCadMemberBase::Indice,T *>
  {
  public:
    typedef typename std::map<Indice,T *> map_base;
    typedef typename std::pair<Indice,T *> pair;
    typedef typename map_base::iterator iterator;
    typedef typename map_base::const_iterator const_iterator;

    MapCadMember(Cad *cad= NULL);

    T * busca(const Indice &);
    const T * busca(const Indice &) const;
    bool existe(const Indice &) const;
    T * get(const Indice &);
    boost::python::list getKeys(void) const;

    void clearAll(void);
    virtual ~MapCadMember(void);

    any_const_ptr GetProp(const std::string &cod) const;
  };

//! @brief Constructor.
template <class T>
MapCadMember<T>::MapCadMember(Cad *cad)
  : MapCadMemberBase(cad) {}

//! @brief Devuelve un puntero a la entidad cuyo identificador se pasa como parámetro.
template <class T>
T * MapCadMember<T>::busca(const Indice &id)
  {    T * retval= NULL;
    iterator i= this->find(id);
    if(i!= this->end()) //La entidad existe.
      retval= (*i).second;
    return retval;
  }

//! @brief Devuelve un puntero a la entidad cuyo nombre se pasa como parámetro.
template <class T>
const T * MapCadMember<T>::busca(const Indice &id) const
  {
    const T * retval= NULL;
    const_iterator i= this->find(id);
    if(i!= this->end()) //La entidad existe.
      retval= (*i).second;
    return retval;
  }

//! @brief Devuelve verdadero si la entidad ya está definida.
template <class T>
bool MapCadMember<T>::existe(const Indice &id) const
  { return (busca(id)!=NULL); }


//! @brief Returns the entity with tag iEnt.
template <class T>
T * MapCadMember<T>::get(const size_t &iEnt)
  {
    T *retval= busca(iEnt);
    if(!retval)
      std::cerr << "MapCadMember::get; no se encontró la entidad: '" 
                << iEnt << "'.\n";
    return retval;   
  }

//! @brief Borra todas las entidades.
template <class T>
void MapCadMember<T>::clearAll(void)
  {
    for(iterator i=this->begin();i!=this->end();i++)
      {
        T * tmp= (*i).second;
        if(tmp)
          delete tmp;
        tmp= NULL;
      }
    this->clear();
  }

//! @brief Returns container's keys.
template <class T>
boost::python::list MapCadMember<T>::getKeys(void) const
  {
    boost::python::list retval;
    for(const_iterator i=this->begin();i!=this->end();i++)
      retval.append((*i).first);
    return retval;
  }

//! @brief Destructor.
template <class T>
MapCadMember<T>::~MapCadMember(void)
  { clearAll(); }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
template <class T>
any_const_ptr MapCadMember<T>::GetProp(const std::string &cod) const
  {
    if(cod=="size")
      {
        tmp_gp_szt= this->size();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return MapCadMemberBase::GetProp(cod);
  }

} //end of XC namespace
#endif
