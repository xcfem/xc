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
// $Date: 2005/02/22 22:21:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CentralDifferenceNoDamping.h,v $
                                                                        
#ifndef CentralDifferenceNoDamping_h
#define CentralDifferenceNoDamping_h

// Written: fmk 
// Created: 02/05
// Revision: A
//
// Description: This file contains the class definition for CentralDifferenceNoDamping.
// CentralDifferenceNoDamping is an algorithmic class for performing a transient 
// analysis using the Central Difference Scheme as implemented in Dyna
//       An = M(-1) (Pn - Fn)
//       Vn+1/2 = Vn-1/2 + dT * An
//       Dn+1   = Dn + deltaT * Vn+1/2
// which is an explicit direct integration scheme as outlined in the paper:
// Goudreau, G.L. and J.O. Hallquist, "Recent Developments in Large Scale Finite Element Lagrangian 
// Hydrocode Technology", Journal of Computer Methods in Applied Mechanics and Engineering, 30, 1982.
//
// What: "@(#) CentralDifferenceNoDamping.h, revA"

#include <solution/analysis/integrator/transient/CentralDifferenceBase.h>
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief CentralDifferenceNoDamping is an algorithmic class
//! for performing a transient analysis using the Central
//! Difference Scheme as implemented in Dyna.
class CentralDifferenceNoDamping: public CentralDifferenceBase
  {
  private:
    ResponseQuantities U; //!< response quantities at time t + deltaT

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    CentralDifferenceNoDamping(SoluMethod *);
    Integrator *getCopy(void) const;
  public:

    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleResidual(FE_Element *theEle);
    int formNodUnbalance(DOF_Group *theDof);    

    int domainChanged(void);    
    int update(const Vector &deltaU);

    int commit(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
inline Integrator *CentralDifferenceNoDamping::getCopy(void) const
  { return new CentralDifferenceNoDamping(*this); }
} // end of XC namespace

#endif

