//----------------------------------------------------------------------------
//  biblioteca vtk_aux; utilidades construidas sobre VTK (<http://www.vtk.org>)
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
                                                                // -*- C++ -*-
/*****************************************************************************/
/*                                                                           */
/* (c) Copyright 2001                                                        */
/*     Stanford Exploration Project, SEP.                                    */    
/*     Numerica Ltda, Bucaramanga, Colombia.                                 */
/*     All left reserved.                                                    */
/*                                                                           */
/*     See http://www.stanford.edu/ for further details.                     */
/*                                                                           */
/*****************************************************************************/
/* Elkin Rafael Arroyo Negrete, Numerica Ltda, ERAN */

#ifndef CUBE_DEFS_IS_INCLUDED
#define CUBE_DEFS_IS_INCLUDED

#include<iostream>

typedef enum
  {
    POSTSCRIPT= 1,
    TIFF= 2,
    CGM= 3,
    BMP= 4,
    VRML= 5  
  } sepFileFormat;// Format to save images

sepFileFormat int_to_file_format(int );
sepFileFormat string_to_file_format(const std::string &);

#endif // CUBE_DEFS_IS_INCLUDED
