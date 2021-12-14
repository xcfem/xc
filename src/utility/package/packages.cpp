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
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2005/07/25 22:56:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/package/packages.cpp,v $
                                                                        
// Written: fmk 
// Created: 06/05

#include "packages.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <dlfcn.h>

//! @brief Return a pointer to the function named functName from
//! the library named libName.
int XC::getLibraryFunction(const std::string &libName, const std::string &funcName, void **libHandle, void **funcHandle)
  {

    int result = 0;
  
    *libHandle= nullptr;
    *funcHandle= nullptr;
  
    const std::string localLibName= libName+".so";
  
    char *error;
    *libHandle = dlopen(localLibName.c_str(), RTLD_NOW);
    if(*libHandle != nullptr)
      {    
        *funcHandle = dlsym(*libHandle, funcName.c_str());
        if((error = dlerror()) != nullptr)
          {
            std::cerr << *error << std::endl;
            result = -2;
          }
        else
          result = -1;
      }
    else
      result = -3;
    return result;
  }
