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

// $Revision: 1.6 $
// $Date: 2005/03/25 00:32:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/fiber_section/FiberSectionRepr.cpp,v $


// File: FiberSectionRepr.C
// Written by Remo M. de Souza
// November 1998

#include <material/section/repres/section/FiberSectionRepr.h>
#include <material/section/repres/section/FiberData.h>

#include "material/section/fiber_section/fiber/Fiber.h"

#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "material/section/fiber_section/FiberSectionGJ.h"


#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

void XC::FiberSectionRepr::vacia_fibras(void)
  {
    fibras.erase(fibras.begin(),fibras.end());// NOTE: don't delete fiber objects themselves
                                              // leave this to FiberSection destructor
  }

void XC::FiberSectionRepr::copia_fibras(const FiberSectionRepr &otro)
  {
    vacia_fibras();
    for(contenedor_fibras::const_iterator i= otro.fibras.begin();i!=otro.fibras.end();i++)
      addFiber(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::FiberSectionRepr::FiberSectionRepr(int sectionID,MaterialLoader *ml)
  :SectionRepres(sectionID, ml), sectID(sectionID) {}

//! @brief Constructor de copia.
XC::FiberSectionRepr::FiberSectionRepr(const FiberSectionRepr &otro)
  :SectionRepres(otro), sectID(otro.sectID)
  { copia_fibras(otro); }

//! @brief Operador asignación.
XC::FiberSectionRepr &XC::FiberSectionRepr::operator=(const FiberSectionRepr &otro)
  {
    SectionRepres::operator=(otro);
    sectID= otro.sectID;
    copia_fibras(otro);
    return *this;
  }

XC::FiberSectionRepr *XC::FiberSectionRepr::getCopy(void) const
  { return new FiberSectionRepr(*this); }


XC::FiberSectionRepr::~FiberSectionRepr(void)
  { vacia_fibras(); }


#define SEC_TAG_FiberSectionRepr 1

int XC::FiberSectionRepr::getType(void) const
  { return SEC_TAG_FiberSectionRepr; }



void XC::FiberSectionRepr::Print(std::ostream &s, int flag)
  {
    SectionRepres::Print(s,flag);
  }


std::ostream &XC::operator<<(std::ostream &s, XC::FiberSectionRepr &fiberSectionRepr)
  {
    fiberSectionRepr.Print(s);
    return s;
  }


int XC::FiberSectionRepr::addFiber(Fiber &newFiber)
  {
    fibras.push_back(&newFiber);
    return 0;
  }

//! @brief Returns the número de fibras de la sección.
int XC::FiberSectionRepr::getNumFibers(void) const
  { return fibras.size(); }

//! @brief Returns the datos de las fibras correspondientes a celdas y armaduras.
XC::FiberData XC::FiberSectionRepr::getFiberData(void) const
  {
    assert(gmSecc);
    return FiberData(*gmSecc);
  }

XC::contenedor_fibras XC::FiberSectionRepr::getFibras2d(void) const
  {
    contenedor_fibras retval;
    if(!material_loader)
      {
        std::cerr << "XC::FiberSectionRepr::getFibras2d; material handler not defined.\n";
        return retval;
      }
    for(contenedor_fibras::const_iterator i= fibras.begin();i!=fibras.end();i++)
      retval.push_back(*i);

    FiberData fiberData= getFiberData();
    fiberData.getFibras2d(retval);
    return retval;
  }

XC::contenedor_fibras XC::FiberSectionRepr::getFibras3d(void) const
  {
    contenedor_fibras retval;
    if(!material_loader)
      {
        std::cerr << "XC::FiberSectionRepr::getFibras2d; material handler not defined.\n";
        return retval;
      }
    for(contenedor_fibras::const_iterator i= fibras.begin();i!=fibras.end();i++)
      retval.push_back(*i);

    FiberData fiberData= getFiberData();
    fiberData.getFibras3d(retval);
    return retval;
  }

XC::FiberSection2d XC::FiberSectionRepr::getFiberSection2d(int secTag) const
  { return FiberSection2d(secTag,fibras,material_loader); }

XC::FiberSection3d XC::FiberSectionRepr::getFiberSection3d(int secTag) const
  { return FiberSection3d(secTag,fibras,material_loader); }

XC::FiberSectionGJ XC::FiberSectionRepr::getFiberSectionGJ(int secTag,const double &GJ) const
  { return FiberSectionGJ(secTag,fibras,GJ,material_loader); }

