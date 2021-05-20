// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//pcl.h
//Secuencias para programaci'on en PCL de las impresoras HP.

#ifndef PCL_H
#define PCL_H

#include <iostream>
#include "utility/utils/text/tab_cod.h"

//Comandos de control de trabajo
inline void PCLReset(std::ostream &os)
  { os << char(27) << char(69); }

//Control de p'agina.
inline void PCLBasePageCtl(std::ostream &os)
  { os << char(27) << char(38) << char(108); }
  
//Tamagno del papel
inline void PCLExec(std::ostream &os)
  { PCLBasePageCtl(os); os << char(49) << char(65); }
inline void PCLLetter(std::ostream &os)
  { PCLBasePageCtl(os); os << char(50) << char(65); }
inline void PCLLegal(std::ostream &os)
  { PCLBasePageCtl(os); os << char(51) << char(65); }
inline void PCLA4(std::ostream &os)
  { PCLBasePageCtl(os); os << char(50) << char(54) << char(65); }

//Orientaci'on de la p'agina.
inline void PCLPortrait(std::ostream &os)
  { PCLBasePageCtl(os); os << char(48) << char(79); }
inline void PCLLandscape(std::ostream &os)
  { PCLBasePageCtl(os); os << char(49) << char(79); }
inline void PCLPortraitInv(std::ostream &os)
  { PCLBasePageCtl(os); os << char(50) << char(79); }
inline void PCLLandscapeInv(std::ostream &os)
  { PCLBasePageCtl(os); os << char(51) << char(79); }

//Configuraci'on del marco de imagen.
inline void PCLFrameBase(std::ostream &os)
  { os << char(27) << char(42) << char(99); }
inline void PCLFrameSzXDP(std::ostream &os,short int x)
//Tamagno horizontal del marco de imagen.
//x: Tamagno expresado en decipuntos 1 decipunto = 1/720 pulgadas.
  { PCLFrameBase(os); os << char(x) << char(88); }
inline void PCLFrameSzXIN(std::ostream &os,short int x)
//Tamagno en horizontal en pulgadas
  { PCLFrameBase(os); os << char(x) << char(075); }
inline void PCLFrameSzYDP(std::ostream &os,short int y)
//Tamagno horizontal del marco de imagen.
//y: Tamagno expresado en decipuntos 1 decipunto = 1/720 pulgadas.
  { PCLFrameBase(os); os << char(y) << char(89); }
inline void PCLFrameSzYIN(std::ostream &os,short int y)
//Tamagno en vertical en pulgadas
  { PCLFrameBase(os); os << char(y) << char(076); }   
inline void PCLFramePos(std::ostream &os,short int y)
//Establece la posici'on punto de anclaje del marco
//en la posici'on actual del cursor.
  { PCLFrameBase(os); os << char(48) << char(84); }

//! @brief Cursor position.
inline void PCLCurPosBase(std::ostream &os)
  { os << char(27) << char(42) << char(112); }

//! @brief Horizontal position of the cursor expressed in points.
//! @param x: coordinate expresed in points.
inline void PCLCurPosX(std::ostream &os, short int x)
  { PCLCurPosBase(os); os << char(x) << char(88); }

//! @brief Vertical position of the cursor expressed in points.
//! @param y: coordinate expresed in points.
inline void PCLCurPosY(std::ostream &os, short int y)
  { PCLCurPosBase(os); os << char(y) << char(89); }
  
//Modo HPGL
inline void PCLModoHPGL0(std::ostream &stream)
//Entra en modo HPGL empleando la posicion del cursor por defecto en HPGL.
  { stream << char(27) << char(37) << char(48) << char(66); }
inline void PCLModoHPGL1(std::ostream &stream)
//Entra en modo HPGL empleando la posicion actual del cursor PCL.
  { stream << char(27) << char(37) << char(49) << char(66); }  

//Margenes
inline void PCLElimMarg(std::ostream &stream)
  { stream << char(27) << char(57); }
inline void PCLMargIzdo(const short int n,std::ostream &stream)
  { stream << char(27) << char(38) << char(97) << char(n) << char(76); }
inline void PCLMargDcho(const short int n,std::ostream &stream)
  { stream << char(27) << char(38) << char(97) << char(n) << char(77); }

//Conjunto de caracteres PC-8 Code Page 437.
inline void PCLCharSetPC8(std::ostream &stream)
  { stream << char(27) << char(40) << char(49) << char(48) << char(85); }

//Conjunto de caracteres PC-850 Multilingual.
inline void PCLCharSetPC850ML(std::ostream &stream)
  { stream << char(27) << char(40) << char(49) << char(50) << char(85); }

//Control de p'agina.
inline void PCLExpPag(std::ostream &stream)
  { stream << char(27) << char(38) << char(108) << char(48) << char(72); }
inline void PCLLinPPag(const short int n,std::ostream &stream)
  { stream << char(27) << char(38) << char(108) << char(n) << char(80); }

//Estilo de letra.
inline void PCLEstiloNormal(std::ostream &stream)
  { stream << char(27) << char(40) << char(115) << char(48) << char(83); }

//Paso
inline void PCLPaso10(std::ostream &stream)
  { stream << char(27) << char(38) << char(107) << char(48) << char(83); }
inline void PCLPasoElite(std::ostream &stream)
  { stream << char(27) << char(38) << char(107) << char(52) << char(83); }
inline void PCLPasoComp(std::ostream &stream)
  { stream << char(27) << char(38) << char(107) << char(50) << char(83); }

//Acentos


#endif
