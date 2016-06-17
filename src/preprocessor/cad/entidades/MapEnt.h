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
//MapEnt.h

#ifndef MAPENT_H
#define MAPENT_H

#include "preprocessor/cad/MapCadMember.h"
#include "EntMdlr.h"
#include <map>

namespace XC {

//! @ingroup Cad
//
//! @brief Contenedor de entidades de CAD (puntos, líneas, superficies,...).
template <class T>
class MapEnt: public MapCadMember<T>
  {
  protected:

  public:
    typedef typename MapCadMember<T>::iterator iterator;
    typedef typename MapCadMember<T>::const_iterator const_iterator;

    MapEnt(Cad *cad= NULL);

    T *getNearest(const Pos3d &p);
    const T *getNearest(const Pos3d &p) const;
    void numera(void);
  };

//! @brief Constructor.
template <class T>
MapEnt<T>::MapEnt(Cad *cad)
  : MapCadMember<T>(cad) {}

//! @brief Devuelve el objeto más cercano a la posición being passed as parameter.
template <class T>
T *MapEnt<T>::getNearest(const Pos3d &p)
  {
    //El código comentado da problemas (recursión infinita) si
    //se llama a la función desde boost.python
    /* MapEnt<T> *this_no_const= const_cast<MapEnt<T> *>(this); */
    /* return const_cast<T *>(this_no_const->getNearest(p)); */
    T *retval= NULL;
    if(!this->empty())
      {
	iterator i= this->begin();
        double d2= (*i).second->DistanciaA2(p);
        retval= (*i).second; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i).second->DistanciaA2(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= (*i).second;
              }
          }
      }
    return retval;
  }

//! @brief Devuelve el objeto más cercano a la posición being passed as parameter.
template <class T>
const T *MapEnt<T>::getNearest(const Pos3d &p) const
  {
    const T *retval= NULL;
    if(!this->empty())
      {
	const_iterator i= this->begin();
        double d2= (*i).second->DistanciaA2(p);
        retval= (*i).second; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i).second->DistanciaA2(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= (*i).second;
              }
          }
      }
    return retval;
  }

//!  @brief Asigna índices a los objetos de la lista para poder emplearlos en VTK.
template <class T>
void MapEnt<T>::numera(void)
  {
    size_t idx= 0;
    for(iterator i= this->begin();i!=this->end();i++,idx++)
      {
        EntMdlr *ptr= (*i).second;
        ptr->set_indice(idx);
      }
  }

} //end of XC namespace
#endif
