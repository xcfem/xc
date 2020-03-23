// -*-c++-*-
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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/25 23:33:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/OPS_Stream.h,v $

#ifndef _OPS_Stream
#define _OPS_Stream

#include <string>

namespace XC{
enum openMode  {OVERWRITE, APPEND};
enum floatField {FIXEDD, SCIENTIFIC};

class OPS_Stream
{
 public:
  OPS_Stream();

  virtual int setFile(const char *fileName, openMode mode = OVERWRITE) {return 0;};
  virtual int setPrecision(int precision) {return 0;};
  virtual int setFloatField(floatField) {return 0;};
  virtual int precision(int precision) {return 0;};
  virtual int width(int width) {return 0;};

  virtual OPS_Stream& write(const char *s, int n) {return *this;};
  virtual OPS_Stream& write(const unsigned char *s, int n) {return *this;};
  virtual OPS_Stream& write(const signed char *s, int n) {return *this;};
  virtual OPS_Stream& write(const void *s, int n) {return *this;};

  virtual OPS_Stream& operator<<(char c) {return *this;};
  virtual OPS_Stream& operator<<(unsigned char c) {return *this;};
  virtual OPS_Stream& operator<<(signed char c) {return *this;};
  virtual OPS_Stream& operator<<(const char *s) {return *this;};
  virtual OPS_Stream& operator<<(const std::string &s) {return *this;};
  virtual OPS_Stream& operator<<(const unsigned char *s) {return *this;};
  virtual OPS_Stream& operator<<(const signed char *s) {return *this;};
  virtual OPS_Stream& operator<<(const void *p) {return *this;};
  virtual OPS_Stream& operator<<(int n) {return *this;};
  virtual OPS_Stream& operator<<(unsigned int n) {return *this;};
  virtual OPS_Stream& operator<<(long n) {return *this;};
  virtual OPS_Stream& operator<<(unsigned long n) {return *this;};
  virtual OPS_Stream& operator<<(short n) {return *this;};
  virtual OPS_Stream& operator<<(unsigned short n) {return *this;};
  virtual OPS_Stream& operator<<(bool b) {return *this;};
  virtual OPS_Stream& operator<<(double n) {return *this;};
  virtual OPS_Stream& operator<<(float n) {return *this;};
};

} //end of XC namespace

#endif
