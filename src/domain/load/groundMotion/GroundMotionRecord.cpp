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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/25 23:32:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/groundMotion/GroundMotionRecord.cpp,v $
                                                                        
                                                                        
// File: ~/earthquake/GroundMotionRecord.C
// 
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class definition for 
// GroundMotionRecord. The units are (g).
//
// What: "@(#) GroundMotionRecord.C, revA"

#include "GroundMotionRecord.h"
#include <domain/load/pattern/time_series/PathSeries.h>
#include <domain/load/pattern/time_series/PathTimeSeries.h>
#include <cstdlib>
#include <cmath>
#include <classTags.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

XC::GroundMotionRecord::GroundMotionRecord(void)
  :GroundMotion(GROUND_MOTION_TAG_GroundMotionRecord), history(0.0) {}

XC::GroundMotionRecord::GroundMotionRecord(const std::string &fileNameAccel, double timeStep, double theFactor, double dT)
  :GroundMotion(GROUND_MOTION_TAG_GroundMotionRecord), history(dT)
  { history.loadAccelFile(fileNameAccel, timeStep, theFactor); }

XC::GroundMotionRecord::GroundMotionRecord(const std::string &fileNameAccel,const std::string &fileNameTime, double theFactor, double dT)
  :GroundMotion(GROUND_MOTION_TAG_GroundMotionRecord), history(dT)
  { history.loadAccelTimeFile(fileNameAccel, fileNameTime, theFactor); }

//! @brief Constructor virtual.
XC::GroundMotion *XC::GroundMotionRecord::getCopy(void) const
  { return new GroundMotionRecord(*this); }

double XC::GroundMotionRecord::getDuration(void) const
  { return history.getDuration(); }


double XC::GroundMotionRecord::getPeakAccel(void) const
  { return history.getPeakAccel(); }

double XC::GroundMotionRecord::getPeakVel(void) const
  { return history.getPeakVel(); }

double XC::GroundMotionRecord::getPeakDisp(void) const
  { return history.getPeakDisp(); }

double XC::GroundMotionRecord::getAccel(double time) const
  { return history.getAccel(time); }

double XC::GroundMotionRecord::getVel(double time) const
  { return history.getVel(time); }

double XC::GroundMotionRecord::getDisp(double time) const
  { return history.getDisp(time); }

int XC::GroundMotionRecord::sendSelf(CommParameters &cp)
  {
    std::cerr << "XC::GroundMotionRecord::sendSelf no implementada." << std::endl;
//   int dbTag = this->getDbTag();

//   static XC::ID idData(6);
  
//   if (theAccelTimeSeries != 0) {
//     idData(0) = theAccelTimeSeries->getClassTag();
//     int seriesDbTag = theAccelTimeSeries->getDbTag();
//     if (seriesDbTag == 0) {
//       seriesDbTag = theCh.getDbTag();
//       theAccelTimeSeries->setDbTag(seriesDbTag);
//     }
//     idData(1) = seriesDbTag;
//   } else
//     idData(0) = -1;

//   if (theVelTimeSeries != 0) {
//     idData(2) = theVelTimeSeries->getClassTag();
//     int seriesDbTag = theVelTimeSeries->getDbTag();
//     if (seriesDbTag == 0) {
//       seriesDbTag = theCh.getDbTag();
//       theVelTimeSeries->setDbTag(seriesDbTag);
//     }
//     idData(3) = seriesDbTag;
//   } else
//     idData(2) = -1;

//   if (theDispTimeSeries != 0) {
//     idData(4) = theDispTimeSeries->getClassTag();
//     int seriesDbTag = theDispTimeSeries->getDbTag();
//     if (seriesDbTag == 0) {
//       seriesDbTag = theCh.getDbTag();
//       theDispTimeSeries->setDbTag(seriesDbTag);
//     }
//     idData(5) = seriesDbTag;
//   } else
//     idData(4) = -1;

//   int res = cp.sendIdData(getDbTagData(),dbTag);
//   if (res < 0) {
//     std::cerr << "XC::GroundMotionRecord::sendSelf() - ch failed to send data\n";
//     return res;
//   }

//   // now send the series
//   if (theAccelTimeSeries != 0) {
//     res = theAccelTimeSeries->sendSelf(commitTag, theCh);
//     if (res < 0) {
//       std::cerr << "XC::GroundMotionRecord::sendSelf - failed to send accel series\n";
//       return res;
//     }
//   }

//   if (theVelTimeSeries != 0) {
//     res = theVelTimeSeries->sendSelf(commitTag, theCh);
//     if (res < 0) {
//       std::cerr << "XC::GroundMotionRecord::sendSelf - failed to send velocity series\n";
//       return res;
//     }
//   }

//   if (theDispTimeSeries != 0) {
//     res = theDispTimeSeries->sendSelf(commitTag, theCh);
//     if (res < 0) {
//       std::cerr << "XC::GroundMotionRecord::sendSelf - failed to send disp series\n";
//       return res;
//     }
//   }

  return 0;
}


int XC::GroundMotionRecord::recvSelf(const CommParameters &cp)
  {
    std::cerr << "XC::GroundMotionRecord::recvSelf no implementada." << std::endl;
//   int dbTag = this->getDbTag();
//   static XC::ID idData(6);
//   int res = cp.receiveIdData(dbTag);
//   if (res < 0) {
//     std::cerr << "XC::UniformExcitation::sendSelf() - ch failed to send data\n";
//     return res;
//   }

//   int seriesClassTag = idData(0);
//   if (seriesClassTag != -1) {
//     int seriesDbTag = idData(1);
//     if (theAccelTimeSeries == 0 || theAccelTimeSeries->getClassTag() != seriesClassTag) {
//       if (theAccelTimeSeries != 0)
//         delete theAccelTimeSeries;
//       theAccelTimeSeries = theBroker.getNewTimeSeries(seriesClassTag);
//       if (theAccelTimeSeries == 0) {
//         std::cerr << "XC::GroundMotionRecord::sendSelf - could not create a XC::TimeSeries object\n";
//         return -2;
//       }
//     }
//     theAccelTimeSeries->setDbTag(seriesDbTag);
//     res = theAccelTimeSeries->recvSelf(commitTag, theCh, theBroker);
//     if (res < 0) {
//       std::cerr << "XC::UniformExcitation::sendSelf() - accel series failed to send data\n";
//       return res;
//     }
//   }

//   seriesClassTag = idData(2);
//   if (seriesClassTag != -1) {
//     int seriesDbTag = idData(3);
//     if (theVelTimeSeries == 0 || theVelTimeSeries->getClassTag() != seriesClassTag) {
//       if (theVelTimeSeries != 0)
//         delete theVelTimeSeries;
//       theVelTimeSeries = theBroker.getNewTimeSeries(seriesClassTag);
//       if (theVelTimeSeries == 0) {
//         std::cerr << "XC::GroundMotionRecord::sendSelf - could not create a XC::TimeSeries object\n";
//         return -2;
//       }
//     }
//     theVelTimeSeries->setDbTag(seriesDbTag);
//     res = theVelTimeSeries->recvSelf(commitTag, theCh, theBroker);
//     if (res < 0) {
//       std::cerr << "XC::UniformExcitation::sendSelf() - accel series failed to send data\n";
//       return res;
//     }
//   }

//   seriesClassTag = idData(4);
//   if (seriesClassTag != -1) {
//     int seriesDbTag = idData(5);
//     if (theDispTimeSeries == 0 || theDispTimeSeries->getClassTag() != seriesClassTag) {
//       if (theDispTimeSeries != 0)
//         delete theDispTimeSeries;
//       theDispTimeSeries = theBroker.getNewTimeSeries(seriesClassTag);
//       if (theDispTimeSeries == 0) {
//         std::cerr << "XC::GroundMotionRecord::sendSelf - could not create a XC::TimeSeries object\n";
//         return -2;
//       }
//     }
//     theDispTimeSeries->setDbTag(seriesDbTag);
//     res = theDispTimeSeries->recvSelf(commitTag, theCh, theBroker);
//     if (res < 0) {
//       std::cerr << "XC::UniformExcitation::sendSelf() - accel series failed to send data\n";
//       return res;
//     }
//   }

  return 0;
}

// AddingSensitivity:BEGIN //////////////////////////////////////////
double XC::GroundMotionRecord::getAccelSensitivity(double time)
  { return history.getAccelSensitivity(time); }

int XC::GroundMotionRecord::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return history.setParameter(argv, param); }

int XC::GroundMotionRecord::updateParameter(int parameterID, Information &info)
  { return history.updateParameter(parameterID,info); }

int XC::GroundMotionRecord::activateParameter(int pparameterID)
  { return history.activateParameter(pparameterID); }
// AddingSensitivity:END ////////////////////////////////////

