// -*-c++-*-
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
                                                                        
// $Revision: 1.2 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/SubstructuringAnalysis.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: Tue Sept 17 16:34:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// SubstructuringAnalysis. SubstructuringAnalysis is a subclass 
// of Analysis, it is used when performing a domain decomposition
// analysis. It provides methods which can be invoked by a subdomain to
// perform the numerical computations required.
//
// What: "@(#) SubstructuringAnalysis.h, revA"

#ifndef SubstructuringAnalysis_h
#define SubstructuringAnalysis_h

#include <solution/analysis/analysis/DomainDecompositionAnalysis.h>

namespace XC {
class Subdomain;

//! @ingroup AnalysisType
//
//! @brief SubstructuringAnalysis is a subclass of DomainDecompositionAnalysis.
//! It is used when performing an analysis using the substructuring method.
//! It differs from the DomainDecompositionAnalysis class only in that the
//! constructor ensures that a SubstructuringSolver is given for the Solver.
class SubstructuringAnalysis: public DomainDecompositionAnalysis
  {
    friend class SolutionProcedure;
    SubstructuringAnalysis(Subdomain &theDomain,DomainSolver &theSolver,SolutionStrategy *s= nullptr);
    Analysis *getCopy(void) const;
  public:
    virtual int analyze(void);
  };

} // end of XC namespace

#endif











