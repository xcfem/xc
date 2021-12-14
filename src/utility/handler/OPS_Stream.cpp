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
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2003/02/14 22:56:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/OPS_Stream.cpp,v $

#include "OPS_Stream.h"
#include <iostream>
#include <iomanip>

XC::OPS_Stream::OPS_Stream(void)
  :fileOpen(false) {}

XC::OPS_Stream::~OPS_Stream()
  {
    if(fileOpen)
      theFile.close();
  }

int XC::OPS_Stream::setFile(const std::string &fileName, openMode mode)
  {
    if(fileOpen)
      {
        theFile.close();
        fileOpen= false;
      }

    if(mode == OVERWRITE) 
      theFile.open(fileName, std::ios::out);
    else
      theFile.open(fileName, std::ios::out| std::ios::app);

    if(theFile.bad())
      {
        std::cerr << "WARNING - StandardStream::" << __FUNCTION__
	          << " - could not open file " << fileName << std::endl;
        return -1;
      }
    else
      fileOpen= true;
    return 0;
  }

//! @brief Close stream.
int XC::OPS_Stream::close(void)
  {
    if(fileOpen)
      theFile.close();
    fileOpen= false;
    return 0;
  }

//! @brief Sets the decimal precision to be used to format
//! floating-point values on output operations.
int XC::OPS_Stream::setPrecision(int prec)
  {
    if(fileOpen)
      theFile << std::setprecision(prec);
    return 0;
  }

int XC::OPS_Stream::setFloatField(floatField field)
  {
    if(field == FIXEDD)
      {
        if(fileOpen != 0)
          theFile << std::setiosflags(std::ios::fixed);
      }
    else if(field == SCIENTIFIC)
      {
        if(fileOpen != 0)
          theFile << std::setiosflags(std::ios::scientific);
      }
    return 0;
  }

XC::OPS_Stream &XC::OPS_Stream::write(const char *s,int n)
  {
    if(fileOpen)
      theFile.write(s, n);
    return *this;
  }

XC::OPS_Stream &XC::OPS_Stream::write(const unsigned char*s,int n)
  {
    if(fileOpen)
      theFile.write((const char *) s, n);
    return *this;
  }

XC::OPS_Stream &XC::OPS_Stream::write(const signed char*s,int n)
  {
    if(fileOpen)
      theFile.write((const char *) s, n);

    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const void *s, int n)
  {
    if(fileOpen)
      theFile.write((const char *) s, n);

    return *this;
  }

XC::OPS_Stream &XC::OPS_Stream::write(const char &c)
  {
    if(fileOpen)
      theFile << c;

    return *this;
  }
XC::OPS_Stream &XC::OPS_Stream::write(const unsigned char &c)
  {
    if(fileOpen)
      theFile << c;

    return *this;
  }
XC::OPS_Stream &XC::OPS_Stream::write(const signed char &c)
  {
    if(fileOpen)
      theFile << c;

    return *this;
  }

XC::OPS_Stream &XC::OPS_Stream::write(const char *s)
    {
      // note that we do the flush so that a "/n" before
      // a crash will cause a flush() - similar to what 
      if(fileOpen)
        {
          theFile << s;
          theFile.flush();
        }
      return *this;
    }

XC::OPS_Stream &XC::OPS_Stream::write(const std::string &s)
    {
      // note that we do the flush so that a "/n" before
      // a crash will cause a flush() - similar to what 
      if(fileOpen)
        {
          theFile << s.c_str();
          theFile.flush();
        }
      return *this;
    }

XC::OPS_Stream&XC::OPS_Stream::write(const unsigned char *s)
    {
      if(fileOpen)
        theFile << s;
      return *this;
    }
 
XC::OPS_Stream &XC::OPS_Stream::write(const signed char *s)
  {
    if(fileOpen)
      theFile << s;

    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const void *p)
  {
/*
    if(fileOpen)
      theFile << p;
*/
    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const int &n)
  {
    if(fileOpen)
      theFile << 1.0*n;

    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const unsigned int &n)
  {
    if(fileOpen)
      theFile << 1.0*n;

    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const long &n)
  {
/*
    if(fileOpen)
      theFile << n;
*/
    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const unsigned long &n)
  {
/*
    if(fileOpen)
      theFile << n;
*/
    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const short &n)
  {
/*
    if(fileOpen)
      theFile << n;
*/
    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const unsigned short &n)
  {
/*
    if(fileOpen)
      theFile << n;
*/
    return *this;
  }
XC::OPS_Stream &XC::OPS_Stream::write(const bool &b)
  {
/*
    if(fileOpen)
      theFile << b;
*/
    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::write(const double &n)
  {
    if(fileOpen)
      theFile << n;

    return *this;
  }

XC::OPS_Stream &XC::OPS_Stream::write(const float &n)
  {
    if(fileOpen)
      theFile << n;

    return *this;
  }
 
XC::OPS_Stream &XC::OPS_Stream::operator<<(const char &c)
  { return write(c); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const unsigned char &c)
  { return write(c); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const signed char &c)
  { return write(c); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const char *s)
  { return write(s); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const std::string &s)
  { return write(s); }

XC::OPS_Stream&XC::OPS_Stream::operator<<(const unsigned char *s)
  { return write(s); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const signed char *s)
  { return write(s); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const void *p)
  { return write(p); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const int &n)
  { return write(n); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const unsigned int &n)
  { return write(n); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const long &n)
  { return write(n); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const unsigned long &n)
  { return write(n); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const short &n)
  { return write(n); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const unsigned short &n)
  { return write(n); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const bool &b)
  { return write(b); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const double &d)
  { return write(d); }

XC::OPS_Stream &XC::OPS_Stream::operator<<(const float &f)
  { return write(f); }

