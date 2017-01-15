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
//BilinearCyclic.cpp

#include "BilinearCyclic.h"
#include <cmath>

XC::BilinearCyclic::BilinearCyclic(int tag, double weight)
:CyclicModel(tag, -1), weightFactor(weight)
{

}


XC::CyclicModel *XC::BilinearCyclic::getCopy()
  { return new BilinearCyclic(*this); }

double XC::BilinearCyclic::getTaskFactor()
{
double tfactor;
	// redundant - only for print
	if(d_curr >= 0 && !initYieldPos)
		return 1.0;
	if(d_curr  < 0 && !initYieldNeg)
		return 1.0;
	// end redundant

	if(yielding /* && fabs(d_curr) >= fabs(d_end) */)
//		return resFactor; // will eventually unload
		tfactor = cycFactor_hist;
    else
    {
    	if(f_bgn*f_end < 0) // full-cycle
    	{
    		if(contains(0.0, f_bgn, f_curr))
    			tfactor=resFactor;
    		else
			  {
				  tfactor=rationalize(d_curr, f_curr, d_end, f_end); // actually with (x0, 0.0)
				  tfactor = weightFactor*tfactor + (1 - weightFactor)*resFactor;
			  }
				  
    	}
    	else // half-cycle
		 {
			 tfactor = rationalize(d_bgn, f_bgn, d_end, f_end);
			 tfactor = weightFactor*tfactor + (1 - weightFactor)*resFactor;
         }
    }

    // std::cerr << *this; std::cerr << "\a";
	// std::cerr << "tFactor = " << tfactor << std::endl;
	// if(yielding) {clog << "-yielding-" << endl;cin.get();}

	return tfactor;
}

void XC::BilinearCyclic::Print (std::ostream &s, int flag)
{
	this->XC::CyclicModel::Print (s, flag);
	s << "+BilinearCyclic\n";
	//s << "   taskFactor = " << getTaskFactor() << std::endl;
	s << "----------------------------------------"
      << "----------------------------------------"
	  << std::endl;
}



