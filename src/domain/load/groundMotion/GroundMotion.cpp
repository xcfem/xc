//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
                                                                        
// $Revision: 1.9 $
// $Date: 2004/07/07 00:51:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/groundMotion/GroundMotion.cpp,v $
                                                                        
// Written: fmk 
// Created: 05/98
// Revision: A


#include "GroundMotion.h"
#include <domain/load/pattern/TimeSeriesIntegrator.h>
#include <domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.h>

#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/matrix/ID.h>




XC::GroundMotion::GroundMotion(int theClassTag)
 :MovableObject(theClassTag), data(3) {}

// void XC::GroundMotion::setIntegrator(TimeSeriesIntegrator *integrator)
//   {
//     if(theIntegrator)
//       delete theIntegrator;
//     theIntegrator = integrator;
//   }

// XC::TimeSeries *XC::GroundMotion::integrate(TimeSeries *theSeries, double delta)
//   {
//   // check that an integrator & accel series exist
//     if(!theIntegrator)
//       {
//         std::cerr << "WARNING:XC::GroundMotion::integrate() - no XC::TimeSeriesIntegrator provided - will use Trapezoidal. \n";
//         theIntegrator = new TrapezoidalTimeSeriesIntegrator();
//         if(theIntegrator == 0)
//           {
//             std::cerr << "WARNING:XC::GroundMotion::integrate() - no XC::TimeSeriesIntegrator provided - failed to create a Trapezoidal .. memory problems! \n";
//             return 0;
//           }
//       }
//     if(!theSeries)
//       {
//         std::cerr << "XC::GroundMotion::integrate - no XC::TimeSeries specified\n";
//         return 0;
//       }

//     // integrate the series, if no vel series exists set it to new one
//     TimeSeries *theNewSeries = theIntegrator->integrate(theSeries, delta);

//     if(!theNewSeries)
//       {
//         std::cerr << "XC::GroundMotion::integrate - no XC::TimeSeriesIntegrator failed to integrate\n";
//         return 0;
//       }
//     return theNewSeries;
//   }


const XC::Vector &XC::GroundMotion::getDispVelAccel(double time) const
  {
    if(time < 0.0)
      {
        data(0) = 0.0;
        data(1) = 0.0;
        data(2) = 0.0;
        return data;
      }
    else
      {
        data(2) = this->getAccel(time);
        data(1) = this->getVel(time);
        data(0) = this->getDisp(time);
      }
    return data;
  }


int  XC::GroundMotion::sendSelf(Communicator &comm)
  {
    std::cerr << "XC::GroundMotion::sendSelf not implemented." << std::endl;


  return 0;
}


int XC::GroundMotion::recvSelf(const Communicator &comm)
  {
    std::cerr << "XC::GroundMotion::recvSelf not implemented." << std::endl;


    return 0;
  }

//! @brief Send a pointer to «ground motion» through the communicator argument.
//! @param posClassTag: Index for the class identifier of the object (in ID).
//! @param posDbTag: Index for the dbTag of the object (in ID).
int XC::sendGroundMotionPtr(GroundMotion *ptr,DbTagData &dt,Communicator &comm,const BrokedPtrCommMetaData &md)
  {
    int res= 0;
    if(ptr)
      {
        dt.setDbTagDataPos(md.getPosClassTag(),ptr->getClassTag());
        res= comm.sendMovable(*ptr,dt,md);
      }
    if(res < 0)
      std::cerr << __FUNCTION__ << "; WARNING"
                << " failed to send ground motion.\n";
    return res;
  }

//! @brief Receive a pointer a «ground motion» through the communicator argument.
//! @param posClassTag: Index for the class identifier of the object (in ID).
//! @param posDbTag: Index for the dbTag of the object (in ID).
XC::GroundMotion *XC::receiveGroundMotionPtr(GroundMotion* ptr,DbTagData &dt,const Communicator &comm,const BrokedPtrCommMetaData &md)
  {
    GroundMotion *retval= nullptr;
    const int matClass= dt.getDbTagDataPos(md.getPosClassTag());
    if(ptr && (ptr->getClassTag() == matClass))
      retval= ptr;
    else 
      {

        // check if we have a ground motion object already &
        // if we do if of right type
        if(ptr) // if old one .. delete it
          delete ptr;
        ptr= nullptr;
        retval= comm.getBrokedGroundMotion(ptr,dt,md);
      }
    if(retval)
      {
        int res= comm.receiveMovable(*retval,dt,CommMetaData(md.getPosDbTag()));
        if(res<0)
          std::cerr << __FUNCTION__ << "; WARNING"
		    << " failed to receive ground motion.\n";
      }
    else
      std::cerr << __FUNCTION__ << "; WARNING"
                << " failed to get a blank ground motion of type: "
                << matClass << std::endl; 
    return retval;
  }
