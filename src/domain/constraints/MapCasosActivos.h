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
//MapCasosActivos.h
                                                                        
                                                                        
#ifndef MapCasosActivos_h
#define MapCasosActivos_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include <map>


namespace XC {
template<class T>
class MapCasosActivos: public EntCmd, public std::map<int,T *>
  {
  public:
    typedef std::map<int, T *> t_map;
    typedef typename t_map::iterator iterator;
    typedef typename t_map::const_iterator const_iterator;
    typedef typename t_map::reference reference;
    typedef typename t_map::const_reference const_reference;
    typedef typename t_map::value_type value_type;
    MapCasosActivos(EntCmd *);
    void Print(std::ostream &s, int flag =0);
  };

//! @brief Constructor
template<class T>
MapCasosActivos<T>::MapCasosActivos(EntCmd *owr)

  : EntCmd(owr), t_map(*this) {}

template<class T>
void MapCasosActivos<T>::Print(std::ostream &s, int flag)
  {
    // go through the array invoking Print on non-zero entries
    const_iterator p = this->begin();
    while(p!=this->end())
      {
	((*p).second)->Print(s, flag);
	p++;
      }
  }

} // end of XC namespace

#endif



