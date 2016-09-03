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
// $Date: 2003/02/14 23:00:57 $
// $Source: domain/load/groundMotion/InterpolatedGroundMotion.cpp,v $
                                                                        
                                                                        
// File: ~/earthquake/InterpolatedGroundMotion.C
// 
// Written: fmk 
// Created: 11/00
// Revision: A
//
// Description: This file contains the class definition for 
// InterpolatedGroundMotion. 
//
// What: "@(#) InterpolatedGroundMotion.C, revA"

#include "InterpolatedGroundMotion.h"
#include <classTags.h>
#include <utility/matrix/Vector.h>

void XC::InterpolatedGroundMotion::libera(void)
  {
    if(factors)
      {
        delete factors;
        factors= nullptr;
      }
  }

void XC::InterpolatedGroundMotion::copia(const Vector *v)
  {
    libera();
    if(v)
      factors= new Vector(*v);
  }

XC::InterpolatedGroundMotion::InterpolatedGroundMotion()
:GroundMotion(GROUND_MOTION_TAG_InterpolatedGroundMotion),
 theMotions(0), factors(nullptr), deltaPeak(0.0) {}

XC::InterpolatedGroundMotion::InterpolatedGroundMotion(const InterpolatedGroundMotion &otro)
  :GroundMotion(otro),theMotions(otro.theMotions), factors(nullptr), deltaPeak(otro.deltaPeak)
  { copia(otro.factors); }

XC::InterpolatedGroundMotion::InterpolatedGroundMotion(const vector_motions &groundMotions, const Vector &fact, double dT)
:GroundMotion(GROUND_MOTION_TAG_InterpolatedGroundMotion),
 theMotions(groundMotions), factors(nullptr), deltaPeak(dT)
  {
    assert(size_t(fact.Size()) == groundMotions.size());
    copia(&fact);
  }

XC::InterpolatedGroundMotion &XC::InterpolatedGroundMotion::operator=(const InterpolatedGroundMotion &otro)
  {
    GroundMotion::operator=(otro);
    theMotions= otro.theMotions;
    copia(otro.factors);
    deltaPeak= otro.deltaPeak;
    return *this;
  }


//! @brief Constructor virtual.
XC::GroundMotion *XC::InterpolatedGroundMotion::getCopy(void) const
  { return new InterpolatedGroundMotion(*this); }


XC::InterpolatedGroundMotion::~InterpolatedGroundMotion(void)
  { libera(); }

double XC::InterpolatedGroundMotion::getDuration(void) const
  {
    double value = 0.0;
    int numMotions = factors->Size();
    for(int i=0; i<numMotions; i++)
      {
        const double motionValue = theMotions[i].getDuration();
        if(motionValue > value)
          value = motionValue;
      }
    return value;
  }

double XC::InterpolatedGroundMotion::getPeakAccel(void) const
  {
    double value = 0.0;
    const double duration = this->getDuration();
    double time = 0.0;
    while(time < duration)
      {
        const double accel = this->getAccel(time);
        if(accel > value)
          value = accel;
        time+= deltaPeak;
      }
    return value;
  }

double XC::InterpolatedGroundMotion::getPeakVel(void) const
  {
    double value = 0.0;
    const double duration = this->getDuration();
    double time = 0.0;
    while(time < duration)
      {
        const double vel = this->getVel(time);
        if(vel>value)
          value= vel;
        time+= deltaPeak;
      }
    return value;
  }

double XC::InterpolatedGroundMotion::getPeakDisp(void) const
  {
    double value= 0.0;
    const double duration = this->getDuration();
    double time= 0.0;
    while(time < duration)
      {
        const double disp= this->getDisp(time);
        if(disp > value)
          value= disp;
        time+= deltaPeak;
      }
    return value;
  }

double XC::InterpolatedGroundMotion::getAccel(double time) const
  {
    if(time < 0.0)
      return 0.0;

    double value= 0.0;
    const int numMotions = factors->Size();
    for(int i=0; i<numMotions; i++)
      { value += (*factors)(i) * theMotions[i].getAccel(time); }
    return value;
  }

double XC::InterpolatedGroundMotion::getVel(double time) const
  {
    if(time < 0.0)
      return 0.0;
    double value = 0.0;
    const int numMotions = factors->Size();
    for(int i=0; i<numMotions; i++)
      { value += (*factors)(i) * theMotions[i].getVel(time); }
    return value;
  }

double XC::InterpolatedGroundMotion::getDisp(double time) const
  {
    if(time < 0.0) return 0.0;

    double value = 0.0;
    const int numMotions = factors->Size();
    for(int i=0; i<numMotions; i++)
      { value += (*factors)(i) * theMotions[i].getDisp(time); }
    return value;
  }

const XC::Vector &XC::InterpolatedGroundMotion::getDispVelAccel(double time) const
  {
    if(time < 0.0)
      {
        data(0) = 0.0;
        data(1) = 0.0;
        data(2) = 0.0;
        return data;
      }

    data.Zero();
    static XC::Vector motionData(3);

    const int numMotions = factors->Size();
    for(int i=0; i<numMotions; i++)
      {
        motionData= theMotions[i].getDispVelAccel(motionData,time);
        motionData*= (*factors)(i);
        data+= motionData;
      }
    return data;
  }


int XC::InterpolatedGroundMotion::sendSelf(CommParameters &cp)
  {
    std::cerr << "XC::InterpolatedGroundMotion::sendSelf() -- not yet implemented" << std::endl;
    return -1;
  }


int XC::InterpolatedGroundMotion::recvSelf(const CommParameters &cp)
  {
    std::cerr << "XC::InterpolatedGroundMotion::recvSelf() -- not yet impelemented" << std::endl;
    return -1;
  }

// AddingSensitivity:BEGIN //////////////////////////////////////////
double XC::InterpolatedGroundMotion::getAccelSensitivity(double time)
  {
    double value= 0.0;
    const int numMotions = factors->Size();
    for(int i=0; i<numMotions; i++)
      { value += (*factors)(i) * theMotions[i].getAccelSensitivity(time); }
    return value;
  }
int XC::InterpolatedGroundMotion::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= 0;
    const int numMotions = factors->Size();
    for(register int i=0; i<numMotions; i++)
      retval= theMotions[i].setParameter(argv,param);
    return retval;
  }

int XC::InterpolatedGroundMotion::updateParameter(int parameterID, Information &info)
  {
    int retval= 0;
    const int numMotions = factors->Size();
    for(register int i=0; i<numMotions; i++)
      retval= theMotions[i].updateParameter(parameterID,info);
    return retval;
  }

int XC::InterpolatedGroundMotion::activateParameter(int pparameterID)
  {
    int retval= 0;
    const int numMotions = factors->Size();
    for(register int i=0; i<numMotions; i++)
      retval= theMotions[i].activateParameter(pparameterID);
    return retval;
  }

// AddingSensitivity:END ////////////////////////////////////


