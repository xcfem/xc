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
// $Date: 2002/06/08 16:17:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/itpack/ItpackLinSOE.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/ItpackGenLinSOE.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// ItpackGenLinSOE. It stores the sparse matrix A in a fashion
// required by the ItpackLinSolver object.
//
// What: "@(#) ItpackGenLinSOE.h, revA"


#ifndef ItpackGenLinSOE_h
#define ItpackGenLinSOE_h

#include <solution/system_of_eqn/linearSOE/LinearSOEData.h>
#include "utility/matrix/Vector.h"

namespace XC {
class ItpackLinSolver;

//! @ingroup SOE
//
//! @brief Sistemas de ecuaciones que
//! hacen uso de la biblioteca ITPACK
//! (http://rene.ma.utexas.edu/CNA/ITPACK) .
class ItpackLinSOE : public LinearSOEData
  {
  private:
    int nnz; //!< number of non-zeros in A
    Vector A; //!< 1d arrays containing coefficients of A, B and X
    ID colA;
    ID rowStartA; // int arrays containing info about coeff's in A
    
    bool Aformed;        // false when zeroA() is called
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    ItpackLinSOE(SoluMethod *);
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    void zeroA(void);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    friend class ItpackLinSolver;
  };
inline SystemOfEqn *ItpackLinSOE::getCopy(void) const
  { return new ItpackLinSOE(*this); }
} // end of XC namespace

#endif
