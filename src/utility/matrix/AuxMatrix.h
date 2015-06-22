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
//AuxMatrix.h
                                                                        
                                                                        
#ifndef AuxMatrix_h
#define AuxMatrix_h

#include "Vector.h"
#include "ID.h"

namespace XC {

#define MATRIX_VERY_LARGE_VALUE 1.0e213

class AuxMatrix: public EntCmd
  {
  private:
    size_t sizeDoubleWork;
    size_t sizeIntWork;
    Vector matrixWork;
    ID intWork;


    AuxMatrix(const AuxMatrix &);
    AuxMatrix &operator=(const AuxMatrix &);
  public:
    // constructors and destructor
    AuxMatrix(const size_t &,const size_t &);
 
    void resize(const size_t &,const size_t &);

    const size_t &getSizeDoubleWork(void) const;
    const size_t &getIntDoubleWork(void) const;
    const double *getMatrixWork(void) const;
    double *getMatrixWork(void);
    const int *getIntWork(void) const;
    int *getIntWork(void);
  };

} // fin namespace XC

#endif




