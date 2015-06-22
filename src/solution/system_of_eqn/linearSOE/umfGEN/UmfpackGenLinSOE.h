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
                                                                        
// $Revision: 1.2 $
// $Date: 2001/12/07 00:17:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// UmfpackGenLinSOE. It stores the sparse matrix A in a fashion
// required by the UmfpackLinSolver object.
//
// What: "@(#) UmfpackGenLinSOE.h, revA"


#ifndef UmfpackGenLinSOE_h
#define UmfpackGenLinSOE_h

#include <solution/system_of_eqn/linearSOE/FactoredSOEBase.h>
#include "utility/matrix/Vector.h"

namespace XC {
class UmfpackGenLinSolver;

//! @ingroup SOE
//
//! @brief Sistemas de ecuaciones que hacen uso
//! de la biblioteca
//! Umf Pack (http://www.cise.ufl.edu/research/sparse/umfpack).
class UmfpackGenLinSOE : public FactoredSOEBase
  {
  private:
    int nnz; //!< number of non-zeros in A
    Vector A; //!< 1d array containing coefficients of A.
    ID colA;
    ID rowStartA; // int arrays containing info about coeff's in A
    int lValue;
    ID index;   // keep only for UMFpack
  protected:
    bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    UmfpackGenLinSOE(SoluMethod *);        
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    void zeroA(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    friend class UmfpackGenLinSolver;

  };
inline SystemOfEqn *UmfpackGenLinSOE::getCopy(void) const
  { return new UmfpackGenLinSOE(*this); }
} // fin namespace XC


#endif

