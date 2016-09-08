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

// $Revision: 1.1 $
// $Date: 2005/12/15 00:13:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/convergenceTest/CTestFixedNumIter.h,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/05
// Revision: A

// Purpose: This file contains the class definition for CTestFixedNumIter.
// A CTestFixedNumIter object performs a fixed number of iterations without
// testing for convergence. This test is useful for hybrid simulation where
// the residual error is corrected for.

#ifndef CTestFixedNumIter_h
#define CTestFixedNumIter_h

#include <solution/analysis/convergenceTest/ConvergenceTest.h>

namespace XC {

//! @ingroup CTest
//
//! @brief Test de convergencia con un número fijo de
//! iteraciones.This test is useful for hybrid simulation where
//! the residual error is corrected for.
class CTestFixedNumIter: public ConvergenceTest
  {
  public:
    // constructors
    CTestFixedNumIter(EntCmd *owr);
    CTestFixedNumIter(EntCmd *owr,int maxNumIter, int printFlag, int normType=2);
    
    ConvergenceTest *getCopy(void) const;
    
    virtual std::string getStatusMsg(const int &flag= 1) const;
    std::string getEnergyProductMessage(void) const;
    int test(void);
  };
} // end of XC namespace

#endif
