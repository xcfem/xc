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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticPlateSection.h,v $

// Ed "C++" Love
//
//  Elastic Plate Section
//

#ifndef ElasticPlateSection_h
#define ElasticPlateSection_h

#include <material/section/plate_section/ElasticPlateProto.h>


namespace XC {
//! @ingroup MATPLAC
//
//! @brief Bidimensional elastic section for plate modellings.
class ElasticPlateSection: public ElasticPlateProto<5>
  {
  public: 
    ElasticPlateSection(int tag) ;
    ElasticPlateSection(void) ;
    ElasticPlateSection(int tag, double E, double nu,double h = 1.0 ) ;
    SectionForceDeformation *getCopy(void) const;

    const ResponseId &getType(void) const;

    const Vector &getStressResultant(void) const;
    const Matrix& getSectionTangent(void) const;
    const Matrix& getInitialTangent(void) const;

    void Print(std::ostream &s,int flag) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };

} // end of XC namespace
#endif
