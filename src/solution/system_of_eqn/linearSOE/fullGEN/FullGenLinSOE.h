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
// $Date: 2001/12/07 00:17:52 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h,v $
                                                                        
                                                                        
#ifndef FullGenLinSOE_h
#define FullGenLinSOE_h

// File: ~/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h
//
// Written: fmk 
// Created: 02/97
// Revision: A
//
// Description: This file contains the class definition for FullGenLinSOE
// FullGenLinSOE is a subclass of LinearSOE. It stores all the components
// of the linear system of equation in 1d arrays.
//
// What: "@(#) FullGenLinSOE.h, revA"

#include "solution/system_of_eqn/linearSOE/FactoredSOEBase.h"
#include "utility/matrix/Vector.h"


namespace XC {
class FullGenLinSolver;

//! @ingroup SOE
//
//! @brief Clase base para los sistemas de ecuaciones
//! con matriz densa.
class FullGenLinSOE : public FactoredSOEBase
  {
  private:
    Vector A;
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    FullGenLinSOE(SoluMethod *);        
    FullGenLinSOE(SoluMethod *,int N);        
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    void zeroA(void);
    
    friend class FullGenLinLapackSolver;    

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
inline SystemOfEqn *FullGenLinSOE::getCopy(void) const
  { return new FullGenLinSOE(*this); }
} // fin namespace XC


#endif

