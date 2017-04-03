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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/SectionRepres.cpp,v $
                                                                        
                                                                        
// File: SectionRepres.C
//
// Written by Remo M. de Souza
// November 1998

#include <material/section/repres/section/SectionRepres.h>
#include <material/section/repres/geom_section/GeomSection.h>
#include "preprocessor/loaders/MaterialLoader.h"




XC::SectionRepres::SectionRepres(int tag,MaterialLoader *ml)
  : TaggedObject(tag), material_loader(ml), gmSecc(nullptr) {}

//! @brief Copy constructor.
XC::SectionRepres::SectionRepres(const SectionRepres &otro)
  : TaggedObject(otro), material_loader(otro.material_loader), gmSecc(otro.gmSecc) {}

//! @brief Assignment operator.
XC::SectionRepres &XC::SectionRepres::operator=(const SectionRepres &otro)
  {
    SectionRepres::operator=(otro);
    material_loader= otro.material_loader;
    gmSecc= otro.gmSecc;
    return *this;
  }

//! @brief Returns the número total de celdas.
int XC::SectionRepres::getNumCells(void) const
  {
    int ncells= 0;
    if(gmSecc)
      ncells= gmSecc->getRegiones().getNumCells();
    return ncells;
  }

void XC::SectionRepres::setGeomNamed(const std::string &nmbGeom)
  {
    const GeomSection *tmp= material_loader->find_ptr_geom_section(nmbGeom);
    if(tmp)
      gmSecc= tmp;
    else
     std::cerr << "(SectionRepres::setGeom;" 
                    << " section geometry: '"
                    << nmbGeom << "' not found.\n";
  }

//! @brief Sets section geometry.
void XC::SectionRepres::setGeom(const GeomSection *g)
  { gmSecc= g; }

//! @brief Returns a pointer to section geometry.
const XC::GeomSection *XC::SectionRepres::getGeom(void) const
  { return gmSecc; }

//! @brief Printing...
void XC::SectionRepres::Print(std::ostream &s, int flag)
  {
    s << "\nSection representation type: " << getType();
  }


std::ostream &XC::operator<<(std::ostream &s, XC::SectionRepres &sectionRepr)
  {
    sectionRepr.Print(s);
    return s;
  }
