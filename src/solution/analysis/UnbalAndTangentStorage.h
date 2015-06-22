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
//UnbalAndTangentStorage.h
                                                                        
                                                                        
#ifndef UnbalAndTangentStorage_h
#define UnbalAndTangentStorage_h

#include <vector>
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {

//! @ingroup Analisis
//
//! @brief Vector de fuerza desequilibrada y matriz de rigidez tangente.
class UnbalAndTangentStorage
  {
  private:
    std::vector<Matrix> theMatrices; //!< array of matrices
    std::vector<Vector> theVectors;  //!< array of vectors

  public:
    UnbalAndTangentStorage(const size_t &);    

    Vector *setUnbalance(const size_t &);
    Matrix *setTangent(const size_t &);

    inline size_t size(void) const
      { return theMatrices.size(); }

    const Matrix &getTangent(const size_t &) const;
    Matrix &getTangent(const size_t &);
    const Vector &getUnbalance(const size_t &) const;
    Vector &getUnbalance(const size_t &);
  };
} // fin namespace XC

#endif

