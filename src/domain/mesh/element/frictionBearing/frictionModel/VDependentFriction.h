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
// $Source: /usr/local/cvs/OpenSees/SRC/element/frictionBearing/frictionModel/VDependentFriction.h,v $

#ifndef VDependentFriction_h
#define VDependentFriction_h         

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for the VDependentFriction
// friction model after Constantinou et al. (1990). In the velocity dependent
// friction model the friction force is given in terms of the friction coefficients
// at low and high velocities and a constant describing the rate of transition.
//
// What: "@(#) VDependentFriction.h, revA"

#include "CoulombFriction.h"

namespace XC {
class VDependentFriction: public CoulombFriction
  {
  protected:
    double muSlow; //!< coefficient of friction at low velocity
    double muFast; //!< coefficient of friction at high velocity
    double transRate; //!< transition rate from low to high velocity
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // constructor
    VDependentFriction(int classTag= FRN_TAG_VDependentFriction);
    VDependentFriction(int tag, double muSlow, double muFast, double transRate,int classTag= FRN_TAG_VDependentFriction);
    
    // public methods to set and obtain response
    int setTrial(double normalForce, double velocity = 0.0);
    double getDFFrcDNFrc(void);
    
    int revertToStart(void);
    
    FrictionModel *getCopy(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &, int flag = 0);
  };
} // end of XC namespace

#endif
