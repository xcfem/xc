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
//StoFibras.cc

#include "StoFibras.h"
#include "material/section/fiber_section/fiber/Fiber.h"
#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

//! @brief Allocates memory for each fiber material and for its data;
//! two (yLoc,Area) for 2D sections (getOrder()= 2) and three (yLoc,zLoc,Area) for 3D sections (getOrder()= 3).
void XC::StoFibras::allocFibers(int numFibras,const Fiber *muestra)
  {
    libera();
    if(numFibras)
      {
        resize(numFibras);
        if(muestra)
          for(int i= 0;i<numFibras;i++)
            (*this)[i]= muestra->getCopy();
      }
  }

void XC::StoFibras::copia_fibras(const StoFibras &otro)
  {
    libera();
    const size_t numFibers= otro.getNumFibers();
    if(numFibers)
      {
        allocFibers(numFibers);
        for(register size_t i= 0;i<numFibers;i++)
          (*this)[i]= otro[i]->getCopy();
      }
  }

void XC::StoFibras::libera(void)
  {
    const size_t numFibers= getNumFibers();
    for(register size_t i= 0;i<numFibers;i++)
      if((*this)[i])
        {
          delete (*this)[i];
          (*this)[i]= nullptr;
        }
    resize(0);
  }

//! @brief Default constructor.
XC::StoFibras::StoFibras(const size_t &num)
  : DqFibras(num) {}

//! @brief Copy constructor.
XC::StoFibras::StoFibras(const StoFibras &otro)
  : DqFibras()
  { copia_fibras(otro); }

//! @brief Assignment operator.
XC::StoFibras &XC::StoFibras::operator=(const StoFibras &otro)
  {
    DqFibras::operator=(otro);
    copia_fibras(otro);
    return *this;
  }

//! @brief Copia las fibras del contenedor being passed as parameter.
void XC::StoFibras::copia_fibras(const contenedor_fibras &fibers)
  {
    const size_t numFibers= fibers.size();
    allocFibers(numFibers);
    int i= 0;
    for(contenedor_fibras::const_iterator ifib=fibers.begin();ifib!=fibers.end(); ifib++,i++)
      {
        (*this)[i]= (*ifib)->getCopy();
        if(!(*this)[i])
          {
            std::cerr << "XC::StoFibras::copia_fibras -- failed to get copy of a XC::Fiber\n";
            exit(-1);
          }
      }
  }

void XC::StoFibras::setup(FiberSection2d &Section2d,const contenedor_fibras &fibers,KRSeccion &kr2)
  {
    if(!fibers.empty())
      {
        copia_fibras(fibers);
        updateKRCDG(Section2d,kr2);
      }
  }

void XC::StoFibras::setup(FiberSection3d &Section3d,const contenedor_fibras &fibers,KRSeccion &kr3)
  {
    if(!fibers.empty())
      {
        copia_fibras(fibers);
        updateKRCDG(Section3d,kr3);
      }
  }

void XC::StoFibras::setup(FiberSectionGJ &SectionGJ,const contenedor_fibras &fibers,KRSeccion &krGJ)
  {
    if(!fibers.empty())
      {
        copia_fibras(fibers);
        updateKRCDG(SectionGJ,krGJ);
      }
  }

//! @brief Destructor:
XC::StoFibras::~StoFibras(void)
  { libera(); }
