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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/MinUnbalDispNorm.h,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/MinUnbalDispNorm.h
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Description: This file contains the class definition for MinUnbalDispNorm.
// MinUnbalDispNorm is an algorithmic class for perfroming a static analysis
// using the minimum unbalanced displacement norm (Chan IJNME 26(2657:2669)1988
//
// What: "@(#) MinUnbalDispNorm.h, revA"

#ifndef MinUnbalDispNorm_h
#define MinUnbalDispNorm_h

#include "DispBase.h"

namespace XC {

#define SIGN_LAST_STEP      1
#define CHANGE_DETERMINANT  2

//! @ingroup StaticIntegrator
//
//! @brief Specifies the incremental load factor such that the residual displacement norm in minimized.
class MinUnbalDispNorm: public DispBase
  {
  private:
    mutable double dLambda1LastStep; //!< dLambda1 at step (i-1)

    mutable int signLastDeltaLambdaStep; //!< sign of dLambda(i-1)
    double dLambda1min, dLambda1max; //!< min & max values for dlambda1 at step (i) 
    mutable double signLastDeterminant;
    int signFirstStepMethod;
  protected:
    double getDLambdaNewStep(void) const;
    double getDLambdaUpdate(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);

    friend class SoluMethod;
    MinUnbalDispNorm(SoluMethod *,double lambda1, int specNumIterStep, 
		     double dlambda1min, double dlambda1max,
		     int signFirstStepMethod = SIGN_LAST_STEP);
    Integrator *getCopy(void) const;
  public:

    int newStep(void);    
    int update(const Vector &deltaU);
    int domainChanged(void);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

  };
inline Integrator *MinUnbalDispNorm::getCopy(void) const
  { return new MinUnbalDispNorm(*this); }
} // end of XC namespace

#endif

