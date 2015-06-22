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
// $Date: 2005/12/06 21:56:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h,v $
                                                                        
                                                                        
#ifndef BandGenLinSOE_h
#define BandGenLinSOE_h

// Written: fmk 
//
// Description: This file contains the class definition for BandGenLinSOE
// BandGenLinSOE is a subclass of LinearSOE. It uses the LAPACK storage
// scheme to store the components of the A matrix, which is a banded 
// unsymmetric matrix.
//
// What: "@(#) BandGenLinSOE.h, revA"


#include <solution/system_of_eqn/linearSOE/FactoredSOEBase.h>
#include "utility/matrix/Vector.h"

namespace XC {
class BandGenLinSolver;

//! @ingroup SOE
//
//! @brief Clase base para los sistemas de ecuaciones
//! con matriz en banda no simétrica.
class BandGenLinSOE : public FactoredSOEBase
  {
  protected:
    int numSuperD, numSubD;    
    Vector A;
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    BandGenLinSOE(SoluMethod *);
    BandGenLinSOE(SoluMethod *,int classTag);
    BandGenLinSOE(SoluMethod *,int N, int numSuperDiagonals, int numSubDiagonal,BandGenLinSolver *theSolver= NULL);        
    SystemOfEqn *getCopy(void) const;
  public:

    
    virtual int setSize(Graph &theGraph);
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);

    virtual void zeroA(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    friend class BandGenLinLapackSolver;
  };
inline SystemOfEqn *BandGenLinSOE::getCopy(void) const
  { return new BandGenLinSOE(*this); }
} // fin namespace XC


#endif

