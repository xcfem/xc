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
                                                                        
                                                                        
#ifndef MatrixCommMetaData_h
#define MatrixCommMetaData_h

#include "ArrayCommMetaData.h"

namespace XC {

//! @brief Datos acerca de la posición, tamaño,... del
//! objeto a transmitir.
class MatrixCommMetaData: public ArrayCommMetaData
  {
  private:
    std::size_t posNc; //!< Número de columnas de la matriz.
  public:
    MatrixCommMetaData(const int &pos,const int &flag,const size_t &nf,const size_t &nc);

    //! @brief Devuelve la posición del número de filas.
    inline const size_t &getPosNumFilas(void) const
      { return posSz; }
    //! @brief Devuelve la posición del número de columnas.
    inline const size_t &getPosNumCols(void) const
      { return posNc; }
    //! @brief Devuelve el número de filas.
    inline const size_t &getNumFilas(void) const
      { return posSz; }
    //! @brief Devuelve el número de columnas.
    inline const size_t &getNumCols(void) const
      { return posNc; }
  };

} // fin namespace XC

#endif
