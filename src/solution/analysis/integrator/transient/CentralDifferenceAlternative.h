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
// $Date: 2005/01/27 04:32:07 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CentralDifferenceAlternative.h,v $
                                                                        
#ifndef CentralDifferenceAlternative_h
#define CentralDifferenceAlternative_h

// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for CentralDifferenceAlternative.
// CentralDifferenceAlternative is an algorithmic class for performing a transient 
// analysis using the alternative form of the Central Differenceintegration scheme, 
// which is an explicit direct integration scheme as outlined in the book 'Concepts
// and Applications of Finite Element Analysis' by Cook, Malkus & Plesha.
//
// What: "@(#) CentralDifferenceAlternative.h, revA"

#include <solution/analysis/integrator/transient/CentralDifferenceBase.h>

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief CentralDifferenceAlternative is an algorithmic class
//! for performing a transient analysis using the alternative
//! form of the Central Differenceintegration scheme, 
//! which is an explicit direct integration scheme as
//! outlined in the book 'Concepts
//! and Applications of Finite Element Analysis' by Cook, Malkus & Plesha.
class CentralDifferenceAlternative : public CentralDifferenceBase
  {
  private:
    Vector Ut, Utp1;  // disp response quantities at time t and t + deltaT

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    CentralDifferenceAlternative(SoluMethod *);
    Integrator *getCopy(void) const;
  public:

    int domainChanged(void);    
    int update(const Vector &deltaU);

    int commit(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

  };
inline Integrator *CentralDifferenceAlternative::getCopy(void) const
  { return new CentralDifferenceAlternative(*this); }
} // end of XC namespace

#endif

