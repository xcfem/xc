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
// $Date: 2009/04/17 23:02:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/frictionBearing/frictionModel/VPDependentFriction.h,v $

#ifndef VPDependentFriction_h
#define VPDependentFriction_h         

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for the VPDependentFriction
// friction model after Constantinou et al. (1996). In the velocity and pressure
// dependent friction model the friction force is given in terms of the friction
// coefficients at low and high velocities with the latter one being a function of
// pressure.
//
// What: "@(#) VPDependentFriction.h, revA"

#include "VDependentFriction.h"

namespace XC {
class VPDependentFriction: public VDependentFriction
  {
  private:
    double A; //!< nominal contact area
    double deltaMu; //!< pressure parameter
    double alpha; //!< pressure parameter
    inline double muFast0(void)
      { return muFast; }
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // constructor
    VPDependentFriction(void);
    VPDependentFriction(int tag, double muSlow, double muFast0, double A,
        double deltaMu, double alpha, double transRate);
    
    // public methods to set and obtain response
    int setTrial(double normalForce, double velocity = 0.0);
    double getDFFrcDNFrc(void);
    
    FrictionModel *getCopy(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);
  };
} // fin namespace XC

#endif
