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
//ConvergenceTestTol.h

#ifndef ConvergenceTestTol_h
#define ConvergenceTestTol_h

#include <solution/analysis/convergenceTest/ConvergenceTest.h>

namespace XC {

//!  @ingroup Analisis
//! 
//! @defgroup CTest Test de convergencia.
//
//! @ingroup CTest
//
//! @brief Test de convergencia dotado de tolerancia.
class ConvergenceTestTol: public ConvergenceTest
  {
  protected:
    double tol; //!< the tol on the energy used to test for convergence

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors
    ConvergenceTestTol(EntCmd *owr,int classTag);	    	
    ConvergenceTestTol(EntCmd *owr,int classTag,double tol, int maxNumIter, int printFlag, int normType,int sz_norms= 1);

    void setTolerance(double newTol);
    double getTolerance(void) const;
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif
