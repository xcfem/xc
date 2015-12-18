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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              July 2004
//# UPDATE HISTORY:
//#
//===============================================================================
#ifndef FDEPState_H
#define FDEPState_H

#include "utility/matrix/nDarray/straint.h"
#include "utility/matrix/nDarray/stresst.h"

namespace XC {
//! @ingroup FDNDMat
//
//! @brief ??.
class FDEPState
  {    
  private:
    straintensor FpInVar;                  // Plastic F;
    double StressLikeInVar;                // Scalar stress like evolution variable
    double StrainLikeInVar;                // Scalar strain like evolution variable
    stresstensor StressLikeKiVar;          // Tensor stress like evolution variable
    straintensor StrainLikeKiVar;          // Tensor strain like evolution variable


    straintensor CommitedFpInVar;          // Plastic F;
    double CommitedStressLikeInVar;        // Scalar stress like evolution variable
    double CommitedStrainLikeInVar;        // Scalar strain like evolution variable
    stresstensor CommitedStressLikeKiVar;  // Tensor stress like evolution variable
    straintensor CommitedStrainLikeKiVar;  // Tensor strain like evolution variable
  public:
      FDEPState();
      FDEPState ( const straintensor& xFpInVar,
              double xStressLikeInVar,
              double xStrainLikeInVar,
    	  const stresstensor& xStressLikeKiVar,
    	  const straintensor& xStrainLikeKiVar,
    	  const straintensor& xCommitedFpInVar,
              double xCommitedStressLikeInVar,
              double xCommitedStrainLikeInVar,
              const stresstensor& xCommitedStressLikeKiVar,
              const straintensor& xCommitedStrainLikeKiVar);	
    FDEPState *newObj();
    FDEPState( const FDEPState& fdeps );
    
    straintensor getFpInVar() const;
    double getStressLikeInVar() const;
    double getStrainLikeInVar() const;
    stresstensor getStressLikeKiVar() const;
    straintensor getStrainLikeKiVar() const;
    
    straintensor getCommitedFpInVar() const;
    double getCommitedStressLikeInVar() const;
    double getCommitedStrainLikeInVar() const;
    stresstensor getCommitedStressLikeKiVar() const;
    straintensor getCommitedStrainLikeKiVar() const;
    
    void setFpInVar(const straintensor& xFpInVar);
    void setStressLikeInVar(double xStressLikeInVar);
    void setStrainLikeInVar(double xStrainLikeInVar);
    void setStressLikeKiVar(const stresstensor& xStressLikeKiVar);
    void setStrainLikeKiVar(const straintensor& xStrainLikeKiVar);
    
    void setCommitedFpInVar(const straintensor& xCommitedFpInVar);
    void setCommitedStressLikeInVar(double xCommitedStressLikeInVar);
    void setCommitedStrainLikeInVar(double xCommitedStrainLikeInVar);
    void setCommitedStressLikeKiVar(const stresstensor& xCommitedStressLikeKiVar);
    void setCommitedStrainLikeKiVar(const straintensor& xCommitedStrainLikeKiVar);

    int commitState(void) ;
    int revertToLastCommit(void) ;
    int revertToStart(void) ;
  };
} // end of XC namespace


#endif
