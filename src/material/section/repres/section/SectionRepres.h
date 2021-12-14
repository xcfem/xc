// -*-c++-*-
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
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/SectionRepres.h,v $
                                                                        
                                                                        
// File: SectionRepres.h
//
// Written by Remo M. de Souza
// November 1998


// Purpose: This file contains the class definition for SectionRepres.  
// SectionRepres is an abstract base class and thus no objects of it's
// type can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.


#ifndef SectionRepres_h 
#define SectionRepres_h 

#include <utility/tagged/TaggedObject.h>
#include <list>
#include "boost/lexical_cast.hpp"


namespace XC {
class SectionGeometry;
class MaterialHandler;
class Material;

//! @ingroup MATSCCRepresGeom
//
//! @brief Material distribution on a cross section.
class SectionRepres: public TaggedObject
  {

  protected:
    MaterialHandler *material_handler; //!< Material handler (searching,...).
    const SectionGeometry *gmSecc; //!< Section geometry.

  public:
    //Constructores
    SectionRepres(int tag,MaterialHandler *ml);
    SectionRepres(const SectionRepres &);

    SectionRepres &operator=(const SectionRepres &);

    // Section inquiring functions
    virtual int getType(void) const = 0;
    int getNumCells(void) const;
    const SectionGeometry *getGeom(void) const;
    void setGeom(const SectionGeometry *);
    void setGeomNamed(const std::string &);


    void Print(std::ostream &s, int flag =0) const;
    friend std::ostream &operator<<(std::ostream &, SectionRepres &);    
  };

std::ostream &operator<<(std::ostream &, SectionRepres &);    

} // end of XC namespace


#endif

