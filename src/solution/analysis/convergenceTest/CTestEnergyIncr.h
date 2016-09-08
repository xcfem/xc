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

// $Revision: 1.4 $
// $Date: 2005/12/15 00:19:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/convergenceTest/CTestEnergyIncr.h,v $

// Written: fmk 
// Date: 09/98
// Modified: 05/05 ahs
//
// Purpose: This file contains the class definition for CTestEnergyIncr.
// A CTestEnergyIncr object tests for convergence using the energy increment,
// which is 0.5 times the absolute value of the product of the rhs and 
// the solution vector of the LinearSOE.

#ifndef CTestEnergyIncr_h
#define CTestEnergyIncr_h

#include <solution/analysis/convergenceTest/ConvergenceTestTol.h>
#include <utility/matrix/Vector.h>

namespace XC {

//! @ingroup CTest
//
//! @brief Especifica una tolerancia para el producto del vector de cargas no
//! equilibradas y el vector incremento de desplazamientos
//! en la iteración actual.
class CTestEnergyIncr: public ConvergenceTestTol
  {
  public:
    // constructors
    CTestEnergyIncr(EntCmd *owr=NULL);	    	
    CTestEnergyIncr(EntCmd *owr,double tol, int maxNumIter, int printFlag, int normType =2);

    ConvergenceTest *getCopy(void) const;
    virtual std::string getStatusMsg(const int &flag= 1) const;
    int test(void);
  };
} // end of XC namespace

#endif
