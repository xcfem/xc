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

 //Modificado LCPT 10-2006.


#ifndef VTK_TAKEPHOTO_H
#define VTK_TAKEPHOTO_H

#include<vtkRenderWindow.h>
#include"vtkTakePhotoDefs.h"
 
class vtkWindowToImageFilter;

//! @brief This class is intended to write images
//! in differents formats, VRML, BMP, POSTSCRIPT/
//! also contain the information of the name of the output
//! file. 
class vtkTakePhoto: public vtkObject
  {
  private:
    std::string name;  
    vtkRenderWindow *renWin;
    vtkWindowToImageFilter *w2if;
    sepFileFormat format;

    vtkTakePhoto(const vtkTakePhoto &); //Not implemented.
    vtkTakePhoto &operator=(const vtkTakePhoto &); //Not implemented.
  public :
    vtkTakePhoto(void);
    static vtkTakePhoto* New();
    ~vtkTakePhoto(void);

    // void SetInput(vtkStructuredPoints *);
    void SetRenderWindow(vtkRenderWindow *);
    void SetFileName(const std::string &name);
    void SetFileFormat(sepFileFormat f)
      {this->format=f;}
    void SetFileFormat(int f);
    void SetFileFormat(const std::string &);
    bool Write(void);
  };

#endif
