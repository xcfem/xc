//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2003/10/30 22:46:56 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/DiscretizedRandomProcessSeries.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu), February 2002
// Revised: 
//

#include <domain/load/pattern/time_series/DiscretizedRandomProcessSeries.h>
#include "domain/component/Parameter.h"
#include <reliability/domain/modulatingFunction/ModulatingFunction.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/filter/Filter.h>
#include <classTags.h>
#include "domain/mesh/element/utils/Information.h"

//! @brief Constructor.
//!
//! @param p_mean: The mean argument is used to specify a constant mean which
//!                is added to the generated process.
//! @param p_maxStdv: The maxStdv argument is used to specify the target
//!                   maximum standard deviation over all time points with
//!                   random pulses.
//! @param num: Number of modulating functions.
//! @param theModFuncs: Pointers to modulating functions.
XC::DiscretizedRandomProcessSeries::DiscretizedRandomProcessSeries(int num, const std::vector<ModulatingFunction *> &theModFuncs,
                                                               double p_mean,
                                                               double p_maxStdv)
  :TimeSeries(TSERIES_TAG_DiscretizedRandomProcessSeries), numModFuncs(num), c(0.0), mean(p_mean), maxStdv(p_maxStdv), theModulatingFunctions(theModFuncs)
  {}

//! @brief Virtual constructor.
XC::TimeSeries *XC::DiscretizedRandomProcessSeries::getCopy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return nullptr;
  }

//! @brief Return load factor at specified time.
double XC::DiscretizedRandomProcessSeries::getFactor(double time) const
  {
    if(time == 0.0)
      { return 0.0; }
    else if(kickInTimes.Size() != randomVariables.Size() )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << ";ERROR : " << std::endl
                  << " number of random variables is not the same"
	          << " as kick-in times. " << std::endl;
        return 0.0;
      }
    else
      {
        double sum2;
        int nrv= 0;
        double modFuncAmplitude, filterAmplitude;
        Filter *theFilter;

        // Loop over all modulating functions
        double sum1 = 0.0;
        for(int k=0; k<numModFuncs; k++)
          {
            // Get value of modulating function number k at time t
            modFuncAmplitude = theModulatingFunctions[k]->getAmplitude(time);
            theFilter = theModulatingFunctions[k]->getFilter();

            // Number of discretizing random variables
            nrv = randomVariables.Size();

            // Loop over all active rv's 
            sum2 = 0.0;
            for(int i=0; i<nrv; i++)
              {
                // Get value of filter for argument (t-ti)
                filterAmplitude = theFilter->getAmplitude(time-kickInTimes(i));
                                
                // Add contribution 'ui * hi'
                sum2+= randomVariables(i) * filterAmplitude;

                // Break when we get to inactive rv's
                if(time-kickInTimes(i) < 0.0)
                  { break; }
              }
            sum1 += sum2*modFuncAmplitude;
          }
        double result = mean + c*sum1;
        return result;
      }
  }


double XC::DiscretizedRandomProcessSeries::getFactorSensitivity(double time)
  {
    // The parameterID has been set to the number of 
    // the random variable in question

    // So, do the same thing as above, just set x(i-1) equal to 1.0
    // for i==parameterID

    if(time == 0.0)
      { return 0.0; }
    else if(kickInTimes.Size() != randomVariables.Size() )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ERROR : " << std::endl
                  << " number of random variables is not the same"
	          << " as kick-in times. " << std::endl;
        return 0.0;
      }
    else
      {
        double sum1;
        double sum2;
        double modFuncAmplitude;
        Filter *theFilter= nullptr;

        // Loop over all modulating functions
        sum1 = 0.0;
        for(int k=0; k<numModFuncs; k++)
          {
            // Get value of modulating function number k at time t
            modFuncAmplitude = theModulatingFunctions[k]->getAmplitude(time);
            theFilter = theModulatingFunctions[k]->getFilter();

            // Loop over all rv's (even though some may be zero at this time)
            sum2= theFilter->getAmplitude(time-kickInTimes(parameterID-1));
            sum1+= sum2*modFuncAmplitude;
          }
        double result = mean + c*sum1;
        return result;
    }
  }


int XC::DiscretizedRandomProcessSeries::sendSelf(Communicator &comm)
  { return 0; }


int XC::DiscretizedRandomProcessSeries::recvSelf(const Communicator &comm)
  { return 0; }


void XC::DiscretizedRandomProcessSeries::Print(std::ostream &s, int flag) const
  {}

int XC::DiscretizedRandomProcessSeries::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv.size() < 1)
        return -1;

//         int rvNumber = info.theInt;

//         // The second argument tells when the random variable "kicks in".
//         // Store this in a table...
//         // In case the vector doesn't exist
//         if(kickInTimes.isEmpty()) {
//                 kickInTimes= Vector(rvNumber);
//                 kickInTimes(rvNumber-1) = (double)atof(argv[0]);

//                 // Assume more than one random variable, so don't 
//                 // update factor 'c' here.
//         }
//         // In case the vector isn't big enough
//         else if (kickInTimes.Size() < rvNumber) {


//                 kickInTimes.resize(rvNumber);


//                 // Put in new_ value
//                 kickInTimes(rvNumber-1) = (double)atof(argv[0]);


//                 /////// Update factor 'c' /////////

//                 // Number of discretizing random variables
//                 int nrv = kickInTimes.Size();

//                 double new_c;
//                 double denominator;

//                 // Loop over all time instances
//                 for (int t=1; t<=nrv; t++) {

//                         denominator = 0.0;

//                         // Loop over modulating functions
//                         for (int k=0; k<numModFuncs; k++) {

//                                 // Get value of modulating function number k at time t
//                                 Filter *theFilter_k = theModulatingFunctions[k]->getFilter();
//                                 double modFuncAmplitude_k = theModulatingFunctions[k]->getAmplitude(kickInTimes(t-1));
                
                                
//                                 // Loop over modulating functions
//                                 for (int l=0; l<numModFuncs; l++) {


//                                         // Get value of modulating function number l at time t
//                                         Filter *theFilter_l = theModulatingFunctions[l]->getFilter();
//                                         double modFuncAmplitude_l = theModulatingFunctions[l]->getAmplitude(kickInTimes(t-1));


//                                         // Loop over all rv's (even though some may be zero at this time)
//                                         for (int i=0; i<nrv; i++) {

//                                                 // Get value of filters for argument (t-ti)
//                                                 double filterAmplitude_k = theFilter_k->getAmplitude((kickInTimes(t-1))-kickInTimes(i));
//                                                 double filterAmplitude_l = theFilter_l->getAmplitude((kickInTimes(t-1))-kickInTimes(i));
                                                
//                                                 // Add contribution 'ui * hi'
//                                                 denominator += filterAmplitude_k*filterAmplitude_l
//                                                                          * modFuncAmplitude_k*modFuncAmplitude_l;
//                                         }
//                                 }
//                         }
//                 }

//                 new_c = sqrt(maxStdv*maxStdv/denominator);

//                 if (c==0.0) {
//                         c = new_c;
//                 }
//                 else if (new_c < c) {
//                         c = new_c;
//                 }

// c = maxStdv;
// std::cerr << "c: " << c << std::endl;

//                 //////////////////////////////////////
//         }
//         else {
//                 kickInTimes(rvNumber-1) = (double)atof(argv[0]);
//         }

//         // The random variable number is returned as a parameter XC::ID
//        return rvNumber;
    std::cerr << "XC::DiscretizedRandomProcessSeries::setParameter not implemented." << std::endl;
    return -1;
  }

int XC::DiscretizedRandomProcessSeries::updateParameter(int parameterID, Information &info)
  {
    // In case the vector doesn't exist
    randomVariables= Vector(parameterID);
    randomVariables(parameterID-1) = info.theDouble;
    return 0;
  }

int XC::DiscretizedRandomProcessSeries::activateParameter(int passedParameterID)
  {
    parameterID = passedParameterID;
    return 0;
  }
