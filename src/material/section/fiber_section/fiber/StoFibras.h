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
//StoFibras.h

#ifndef StoFibras_h
#define StoFibras_h

#include "DqFibras.h"
#include <material/section/repres/section/contenedor_fibras.h>

namespace XC {

//! \ingroup MATSCCFibras
//
//! @brief Contenedor de fibras.
class StoFibras : public DqFibras
  {
    void libera(void);
    void copia_fibras(const StoFibras &otro);
    void copia_fibras(const contenedor_fibras &);

  public:
    StoFibras(const size_t &num= 0); 
    StoFibras(const StoFibras &otro);
    StoFibras &operator=(const StoFibras &otro);
    void allocFibers(int numFibras,const Fiber *muestra= nullptr);
    void setup(FiberSection2d &,const contenedor_fibras &,CrossSectionKR &);
    void setup(FiberSection3d &,const contenedor_fibras &,CrossSectionKR &);
    void setup(FiberSectionGJ &,const contenedor_fibras &,CrossSectionKR &);
    ~StoFibras(void);
  };
} // end of XC namespace

#endif
