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
// $Date: 2005/12/06 22:06:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for ProfileSPDLinSOE
// ProfileSPDLinSOE is a subclass of LinearSOE. It uses the LAPACK Upper storage
// scheme to store the components of the A matrix.

// What: "@(#) ProfileSPDLinSOE.h, revA"

#ifndef ProfileSPDLinSOE_h
#define ProfileSPDLinSOE_h

#include <solution/system_of_eqn/linearSOE/FactoredSOEBase.h>
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"

namespace XC {
class ProfileSPDLinSolver;

//! @ingroup SOE
//
//! @brief Sistemas de ecuaciones con matriz
//! de tipo «perfil superior».
class ProfileSPDLinSOE : public FactoredSOEBase
  {
  protected:
    int profileSize;    
    Vector A;
    ID iDiagLoc;
    bool isAcondensed;
    int numInt;
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    ProfileSPDLinSOE(SoluMethod *);
    ProfileSPDLinSOE(SoluMethod *,int classTag);
    ProfileSPDLinSOE(SoluMethod *,int N, int *iLoc, ProfileSPDLinSolver *theSolver= NULL);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    
    virtual void zeroA(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    friend class ProfileSPDLinSolver;    
    friend class ProfileSPDLinDirectSolver;
    friend class ProfileSPDLinDirectBlockSolver;
    friend class ProfileSPDLinDirectThreadSolver;    
    friend class ProfileSPDLinDirectSkypackSolver;    
    friend class ProfileSPDLinSubstrSolver;
    friend class ProfileSPDLinSubstrThreadSolver;
  };
inline SystemOfEqn *ProfileSPDLinSOE::getCopy(void) const
  { return new ProfileSPDLinSOE(*this); }
} // fin namespace XC


#endif



