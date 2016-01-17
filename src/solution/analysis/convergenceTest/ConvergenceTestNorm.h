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
//ConvergenceTestNorm.h

#ifndef ConvergenceTestNorm_h
#define ConvergenceTestNorm_h

#include <solution/analysis/convergenceTest/ConvergenceTestTol.h>

namespace XC {

//! @ingroup CTest
//
//! @brief Test de convergencia que emplea el valor inicial
//! de la norma de un vector (vector solución,...).
class ConvergenceTestNorm: public ConvergenceTestTol
  {
  protected:
    double norm0; //!< norm at first iteration of each step

  public:
    // constructors
    ConvergenceTestNorm(EntCmd *owr,int classTag);	    	
    ConvergenceTestNorm(EntCmd *owr,int cTag,double tl, int maxNIter, int printFlag, int normType, int sz);

    virtual int start(void);  
  };
} // end of XC namespace

#endif
