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
//UnbalAndTangent.h
                                                                        
                                                                        
#ifndef UnbalAndTangent_h
#define UnbalAndTangent_h

#include "solution/analysis/UnbalAndTangentStorage.h"

namespace XC {

//! @ingroup Analisis
//
//! @brief Vector de fuerza desequilibrada y matriz de rigidez tangente.
class UnbalAndTangent
  {
  private:
    size_t nDOF;
    Vector *theResidual;
    Matrix *theTangent;
    UnbalAndTangentStorage &unbalAndTangentArray; //!< Reference to array of class wide vectors and matrices
    bool libera(void);
    void alloc(void);
    void copia(const UnbalAndTangent &otro);

  public:
    UnbalAndTangent(const size_t &,UnbalAndTangentStorage &);
    UnbalAndTangent(const UnbalAndTangent &otro);
    UnbalAndTangent &operator=(const UnbalAndTangent &otro);
    virtual ~UnbalAndTangent(void);

    inline const size_t &getNumDOF(void) const
      { return nDOF; }

    const Matrix &getTangent(void) const;
    Matrix &getTangent(void);
    const Vector &getResidual(void) const;
    Vector &getResidual(void);
  };
} // fin namespace XC

#endif


