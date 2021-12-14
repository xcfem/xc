// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//hpgl.h
//Secuencias para programaci'on en HPGL de las impresoras HP.

#ifndef HPGL_H
#define HPGL_H

#include <iostream>

//Comandos de control de trabajo
inline void HPGLInitialize(std::ostream &os)
  { os << "IN;"; }
  
inline void HPGLSelectPen(std::ostream &os,short int pen)
  { os << "SP" << pen << ';'; }
  
inline void HPGLSetUserScaling(std::ostream &os,int x1,int x2,int y1,int y2)
  { os << "SC" << x1 << ',' << x2 << ',' << y1 << ',' << y2 << ';'; } 

//Movimiento de pluma.
inline void HPGLPenDown(std::ostream &os,int x,int y)
  { os << "PD" << x << ',' << y << ';'; }
inline void HPGLPenUp(std::ostream &os,int x,int y)
  { os << "PU" << x << ',' << y << ';'; }
inline void HPGLCircle(std::ostream &os,int r)
  { os << "CI" << r << ';'; }
  
//Modo
inline void HPGLPCLMode0(std::ostream &os)
//Entra en modo PCL empleando la posici'on previa
//del cursor HPGL.
  { os << char(27) << char(37) << char(48) << char(65); }
inline void HPGLPCLMode1(std::ostream &os)
//Entra en modo PCL empleando actual de la pluma HPGL2
  { os << char(27) << char(37) << char(49) << char(65); }
  
#endif
