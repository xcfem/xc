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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/FiberSectionRepr.h,v $
                                                                        
                                                                        
// File: FiberSectionRepr.h
// Written by Remo M. de Souza (November 1998)
// modified by rms (July 1999) - doesn't prespecify number of regions and reinf. layers
//             rms (August 1999) - add fibers to section for the analysis
// modified by lcpt (September 2006) - big modification. 
		    
#ifndef FiberSectionRepr_h 
#define FiberSectionRepr_h 

#include <material/section/repres/section/SectionRepres.h>
#include <material/section/repres/section/fiber_list.h>
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include <list>

namespace XC {
class Fiber;
class FiberSection2d;
class FiberSection3d;
class FiberSectionGJ;
class FiberData;


//! @ingroup MATSCCRepresGeom
//
//! @brief Fiber section representation.
class FiberSectionRepr: public SectionRepres
  {
  private:
    int sectID;
    fiber_list fibers; //!< Pointer to isolated fibers.

  protected:
    void clear_fibers(void);
    void copy_fibers(const FiberSectionRepr &otro);
  public:
    // constructor and destructor
    FiberSectionRepr(int sectionID,MaterialLoader *ml);
    FiberSectionRepr(const FiberSectionRepr &otro);
    FiberSectionRepr &operator=(const FiberSectionRepr &otro);
    virtual FiberSectionRepr *getCopy(void) const;
    ~FiberSectionRepr(void);
        
    // edition functions
    int addFiber(Fiber &theFiber);

    // inquiring functions
    int getType(void) const;
      
    int getNumFibers(void) const;   
    FiberData getFiberData(void) const;
    fiber_list get2DFibers(void) const;
    FiberSection2d getFiberSection2d(int secTag) const;
    fiber_list get3DFibers(void) const;
    FiberSection3d getFiberSection3d(int secTag) const;
    FiberSectionGJ getFiberSectionGJ(int secTag,const double &GJ) const;
    
    void Print(std::ostream &s, int flag =0);
    friend std::ostream &operator<<(std::ostream &s, FiberSectionRepr &fiberSectionRepr);    
  };
} // end of XC namespace
#endif

