// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ObjMap.h

#ifndef OBJMAP
#define OBJMAP

#include <map>
#include <string>
#include <iostream>

//! @ingroup UTILS
//
//! @brief Contenedor de objetos de la bibioteca vtk.
template <class T>
class ObjMap: private std::map<std::string,T *>
  {
  public:
    typedef std::map<std::string,T*> map_ptr;
    typedef typename map_ptr::iterator iterator;
    typedef typename map_ptr::const_iterator const_iterator;
  private:
    void clear(void);
    ObjMap(const ObjMap &);
    const ObjMap &operator=(const ObjMap &);
  protected:
    inline iterator find(const std::string &s)
      { return std::map<std::string,T *>::find(s); }
  public:

    ObjMap(void);
    void Borra(const std::string &);
    T *insert(const std::string &,const T &);
    T *Busca(const std::string &);
    const T *Busca(const std::string &) const;
    inline const_iterator find(const std::string &s) const
      { return std::map<std::string,T *>::find(s); }
    ~ObjMap(void);
  };

//! @brief Constructor.
template <class T>
ObjMap<T>::ObjMap(void)
  : map_ptr() {}

//! @brief Constructor de copia.
template <class T>
ObjMap<T>::ObjMap(const ObjMap &other)
  : map_ptr()
  {
    for(const_iterator i=other.begin();i!=other.end();i++)
      insert(i->first,*(i->second));
  }

//! @brief Operador de asignación.
template <class T>
const ObjMap<T> &ObjMap<T>::operator=(const ObjMap &other)
  {
    clear();
    for(const_iterator i=other.begin();i!=other.end();i++)
      insert(i->first,*(i->second));
    return *this;
  }

//! @brief Libera la memoria ocupada por elemento cuya clave se pasa como parámetro.
template <class T>
void ObjMap<T>::Borra(const std::string &clave)
  {
    T *previo= Busca(clave);
    if(previo) //Si existe un elemento con esta clave lo borramos.
      {
        delete previo;
        map_ptr::erase(clave);
      } 
  }

//! @brief Libera la memoria ocupada por todos los objetos del contenedor.
template <class T>
void ObjMap<T>::clear(void)
  {
    for(iterator i=ObjMap<T>::begin();i!=ObjMap<T>::end(); i++)
      {
        delete i->second;
        i->second=nullptr;
      }
    map_ptr::clear();
  }

//! @brief Return un puntero al objeto cuya clave se pasa como parámetro.
template <class T>
T *ObjMap<T>::Busca(const std::string &clave)
  {
    iterator i= ObjMap<T>::find(clave);
    if(i!=ObjMap<T>::end())
      return i->second;
    else
      return nullptr;
  }

//! @brief Return un puntero al objeto cuya clave se pasa como parámetro.
template <class T>
const T *ObjMap<T>::Busca(const std::string &clave) const
  {
    const_iterator i= ObjMap<T>::find(clave);
    if(i!=ObjMap<T>::end())
      return i->second;
    else
      return nullptr;
  }

//! @brief Insert the object in the container usen the key argument.
template <class T>
T *ObjMap<T>::insert(const std::string &clave,const T &c)
  {
    T *previo= Busca(clave);
    if(previo) //If an object with the same key already exists: erase it.
      {
	std::cerr << this->getClassName() << "::" << __FUNCTION__
	          << "; object with key: '" << clave
		  << "' already exists." << std::endl;
        delete previo;
        map_ptr::erase(clave);
      } 
    T *retval= c.Copia();
    map_ptr::insert(make_pair(clave,retval));
    return retval;
  }

//! @brief Destructor.
template <class T>
ObjMap<T>::~ObjMap(void)
  { clear(); }

#endif
