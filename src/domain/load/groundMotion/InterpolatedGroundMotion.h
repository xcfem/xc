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
// $Date: 2000/12/12 07:32:38 $
// $Source: domain/load/groundMotion/InterpolatedGroundMotion.h,v $
                                                                        
// File: ~/earthquake/InterpolatedGroundMotion.h
// 
// Written: fmk
// Created: 11/00
// Revision: A
//
// Description: This file contains the class definition for 
// an InterpolatedGroundMotion, which inetrploates between 2
// motions to determine a motion.
//
// What: "@(#) InterpolatedGroundMotion.h, revA"

#ifndef InterpolatedGroundMotion_h
#define InterpolatedGroundMotion_h

#include "GroundMotion.h"
#include "MotionHistory.h"
#include <vector>

namespace XC {
  class Vector;

//! @ingroup GMotion
//
//! @brief ??.
class InterpolatedGroundMotion: public GroundMotion
  {
  public:
    typedef std::vector<MotionHistory> vector_motions;
  private:
    vector_motions theMotions;
    Vector *factors;
    
    double deltaPeak;  // increment for determining a peak response
    void libera(void);
    void copia(const Vector *);
  public:
    InterpolatedGroundMotion(void);
    InterpolatedGroundMotion(const InterpolatedGroundMotion &);
    InterpolatedGroundMotion(const vector_motions &groundMotions, const Vector &fact, double deltaPeak = 0.01);

    virtual ~InterpolatedGroundMotion(void);
    InterpolatedGroundMotion &operator=(const InterpolatedGroundMotion &);
    virtual GroundMotion *getCopy(void) const;

    virtual double getDuration(void) const;

    virtual double getPeakAccel(void) const;
    virtual double getPeakVel(void) const;
    virtual double getPeakDisp(void) const;

    virtual double getAccel(double time) const;
    virtual double getVel(double time) const;
    virtual double getDisp(double time) const;
    virtual const  Vector &getDispVelAccel(double time) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual double getAccelSensitivity(double time);
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif
