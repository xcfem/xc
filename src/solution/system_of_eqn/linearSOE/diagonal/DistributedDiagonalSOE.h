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
// $Date: 2005/05/25 23:33:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.h,v $
                                                                        
// Written: fmk 
// Created: 05/05
//
// Description: This file contains the class definition for DistributedDiagonalSOE
// DistributedDiagonalSOE is a subclass of LinearSOE. It stores a diagonal system
// of equation, i.e. just the diagonal

// What: "@(#) DistributedDiagonalSOE.h, revA"

#ifndef DistributedDiagonalSOE_h
#define DistributedDiagonalSOE_h

#include <solution/system_of_eqn/linearSOE/FactoredSOEBase.h>
#include "solution/system_of_eqn/linearSOE/DistributedLinSOE.h"
#include <utility/matrix/ID.h>
#include "utility/matrix/Vector.h"

namespace XC {
class DistributedDiagonalSolver;

//! @ingroup SOE
//
//! @brief Clase base para los sistemas de ecuaciones
//! con matriz diagonal distribuída.
class DistributedDiagonalSOE : public FactoredSOEBase, public DistributedLinSOE
  {
  private:
    Vector A;

    int numProcesses;

    ID myDOFs;
    ID myDOFsShared;
    int numShared;
    Vector dataShared;
    Vector vectShared;

    friend class SoluMethod;
    DistributedDiagonalSOE(SoluMethod *);
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    void zeroA(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    friend class DistributedDiagonalSolver;
  };
inline SystemOfEqn *DistributedDiagonalSOE::getCopy(void) const
  { return new DistributedDiagonalSOE(*this); }
} // end of XC namespace


#endif



