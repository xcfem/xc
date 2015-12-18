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
                                                                        
                                                                        
#ifndef ArrayCommMetaData_h
#define ArrayCommMetaData_h

#include "PtrCommMetaData.h"
#include <cstddef>

namespace XC {

//! @brief Datos acerca de la posición, tamaño,... del
//! objeto a transmitir.
class ArrayCommMetaData: public PtrCommMetaData
  {
  protected:
    std::size_t posSz; //!< Posición del tamaño del array ó tamaño del array.
  public:
    ArrayCommMetaData(const int &,const int &,const size_t &);

    //! @brief Devuelve la posición del tamaño del array.
    inline const std::size_t &getPosSize(void) const
      { return posSz; }

    //! @brief Devuelve el tamaño del array.
    inline const std::size_t &getSize(void) const
      { return posSz; }
  };

} // end of XC namespace

#endif
