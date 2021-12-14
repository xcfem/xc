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
                                                                        
// $Revision: 1.1 $
// $Date: 2003/10/27 23:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/modulatingFunction/KooModulatingFunction.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/modulatingFunction/KooModulatingFunction.h>
#include <reliability/domain/modulatingFunction/ModulatingFunction.h>
#include <classTags.h>
#include <cmath>
#include <reliability/domain/filter/Filter.h>


XC::KooModulatingFunction::KooModulatingFunction(int tag,
												 Filter *theFilt, 
												 double pt1,
												 double pt2)
:ModulatingFunction(tag,MODULATING_FUNCTION_trapezoidal)
{
	t1 = pt1;
	t2 = pt2;

	if (t1>t2 ) {
		std::cerr << "WARNING: Inconsistent input to Koo Modulating Function" << std::endl;
	}

	theFilter = theFilt;
}

double
XC::KooModulatingFunction::getAmplitude(double time)
{
	double amplitude;
	if (time < 0.0) {
		amplitude = 0.0;
	}
	else if (time < t1) {
		amplitude = time*time/25.0; 
	}
	else if (time < t2) {
		amplitude = 1.0;
	}
	else {
		amplitude = exp(-0.5*(time-10.0));
	}

	return amplitude;
}

 XC::Filter *
XC::KooModulatingFunction::getFilter()
{
	return theFilter;
}

double
XC::KooModulatingFunction::getMaxAmplitude()
{
	return 1.0;
}

//! @brief Print stuff.
void XC::KooModulatingFunction::Print(std::ostream &s, int flag) const
  {}

