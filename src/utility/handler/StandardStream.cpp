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
                                                                        
// $Revision: 1.5 $
// $Date: 2004/11/24 22:37:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/StandardStream.cpp,v $

#include <utility/handler/StandardStream.h>
#include <iostream>
#include <iomanip>
using std::cerr;
using std::ios;
using std::setiosflags;

XC::StandardStream::StandardStream(void)
  {}



XC::OPS_Stream &XC::StandardStream::write(const char *s,int n)
  {
    std::cerr.write(s, n);
    OPS_Stream::write(s,n);
    return *this;
  }

 XC::OPS_Stream& 
XC::StandardStream::write(const unsigned char*s, int n)
  {
    std::cerr.write((const char *) s, n);
    OPS_Stream::write(s,n);
    return *this;
  }
XC::OPS_Stream &XC::StandardStream::write(const signed char*s, int n)
  {
    std::cerr.write((const char *)s, n);
    OPS_Stream::write(s,n);
    return *this;
  }

XC::OPS_Stream &XC::StandardStream::write(const void *s, int n)
  {
    std::cerr.write((const char *)s, n);
    OPS_Stream::write(s,n);
    return *this;
  }

XC::OPS_Stream &XC::StandardStream::write(const char &c)
  {
    std::cerr << c;
    OPS_Stream::write(c);
    return *this;
  }
XC::OPS_Stream &XC::StandardStream::write(const unsigned char &c)
  {
    std::cerr << c;
    OPS_Stream::write(c);
    return *this;
  }
XC::OPS_Stream &XC::StandardStream::write(const signed char &c)
  {
    std::cerr << c;
    OPS_Stream::write(c);
    return *this;
  }

XC::OPS_Stream &XC::StandardStream::write(const char *s)
    {
      std::cerr << s;
      std::cerr.flush();
      OPS_Stream::write(s);
      return *this;
    }

XC::OPS_Stream &XC::StandardStream::write(const std::string &s)
    {
      // note that we do the flush so that a "/n" before
      // a crash will cause a flush() - similar to what 
      std::cerr << s;
      std::cerr.flush();
      OPS_Stream::write(s);
      return *this;
    }

XC::OPS_Stream&XC::StandardStream::write(const unsigned char *s)
    {
      if(fileOpen)
        theFile << s;
      return *this;
    }
 
XC::OPS_Stream &XC::StandardStream::write(const signed char *s)
  {
    std::cerr << s;
    OPS_Stream::write(s);
    return *this;
  }
 
XC::OPS_Stream &XC::StandardStream::write(const void *p)
  {
/*
    std::cerr << p;
    OPS_Stream::write(p);
*/    return *this;
  }
 
XC::OPS_Stream &XC::StandardStream::write(const int &n)
  {
    std::cerr << 1.0*n;
    OPS_Stream::write(1.0*n);
    return *this;
  }
 
XC::OPS_Stream &XC::StandardStream::write(const unsigned int &n)
  {
    std::cerr << 1.0*n;
    OPS_Stream::write(1.0*n);
    return *this;
  }
 
XC::OPS_Stream &XC::StandardStream::write(const long &n)
  {
/*
    std::cerr << n;
    OPS_Stream::write(n);
*/    return *this;
  }
 
XC::OPS_Stream &XC::StandardStream::write(const unsigned long &n)
  {
/*
    std::cerr << n;
    OPS_Stream::write(n);
*/    return *this;
  }
 
XC::OPS_Stream &XC::StandardStream::write(const short &n)
  {
/*
    std::cerr << n;
    OPS_Stream::write(n);
*/    return *this;
  }
 
XC::OPS_Stream &XC::StandardStream::write(const unsigned short &n)
  {
/*
    std::cerr << n;
    OPS_Stream::write(n);
*/    return *this;
  }
XC::OPS_Stream &XC::StandardStream::write(const bool &b)
  {
/*
    std::cerr << b;
    OPS_Stream::write(b);
*/    return *this;
  }
 
XC::OPS_Stream &XC::StandardStream::write(const double &n)
  {
    std::cerr << n;
    OPS_Stream::write(n);
    return *this;
  }

XC::OPS_Stream &XC::StandardStream::write(const float &n)
  {
    std::cerr << n;
    OPS_Stream::write(n);
    return *this;
  }


