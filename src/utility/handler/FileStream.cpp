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
                                                                        
// $Revision: 1.7 $
// $Date: 2005/02/14 19:43:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/FileStream.cpp,v $


#include <utility/handler/FileStream.h>
#include <iostream>
#include <iomanip>
using std::cerr;
using std::ios;
using std::setiosflags;

XC::FileStream::FileStream(void)
  :fileOpen(0), fileName("")
  {}

XC::FileStream::~FileStream(void)
  {
    if(fileOpen == 1)
      theFile.close();
  }

// int XC::FileStream::setFile(const char *name, openMode mode)
// {
//   if(name == 0) {
//     std::cerr << "XC::FileStream::setFile() - no name passed\n";
//     return -1;
//   }

//   // first create a copy of the file name
//   if(fileName != 0) {
//     if(strcmp(fileName, name) != 0)
//       delete [] fileName;
//     fileName = 0;
//   }
//   if(fileName == 0) {
//     fileName = new char[strlen(name)+1];
//     if(fileName == 0) {
//       std::cerr << "XC::FileStream::setFile() - out of memory copying name: " << name << std::endl;
//       return -1;
//     }
    
//     // copy the strings
//     strcpy(fileName, name);
//   }

//   // if file already open, close it
//   if(fileOpen == 1) {
//     theFile.close();
//     fileOpen = 0;
//   }

//   if(mode == OVERWRITE) 
//     theFile.open(fileName, ios::out);
//   else
//     theFile.open(fileName, ios::out| ios::app);

//   if(theFile.bad()) {
//     std::cerr << "WARNING - XC::FileStream::setFile()";
//     std::cerr << " - could not open file " << fileName << std::endl;

//     return -1;
//   } else
//     fileOpen = 1;

//   return 0;
// }

int XC::FileStream::setFile(const std::string &name, openMode mode)
  {
    if(name == "")
      {
        std::cerr << "XC::FileStream::setFile() - no name passed\n";
        return -1;
      }
    fileName= name;

    // if file already open, close it
    if(fileOpen == 1)
      {
        theFile.close();
        fileOpen = 0;
      }

    if(mode == OVERWRITE) 
      theFile.open(fileName.c_str(), ios::out);
    else
      theFile.open(fileName.c_str(), ios::out| ios::app);

    if(theFile.bad())
      {
        std::cerr << "WARNING - XC::FileStream::setFile()";
        std::cerr << " - could not open file " << fileName << std::endl;
        return -1;
      }
    else
      fileOpen = 1;
    return 0;
  }

int XC::FileStream::open(void)
  {
    // check setFile has been called
    if(fileName == "")
      {
        std::cerr << "XC::FileStream::open(void) - no file name has been set\n";
        return -1;
      }

    // if file already open, return
    if(fileOpen == 1)
      { return 0; }

    // open file
    theFile.open(fileName.c_str(), ios::out| ios::app);
    if(theFile.bad())
      {
        std::cerr << "WARNING - XC::FileStream::open()";
        std::cerr << " - could not open file " << fileName << std::endl;
        return -1;
      }
    else
      fileOpen = 1;
    return 0;
  }

int XC::FileStream::close(void)
  {
    if(fileOpen != 0)
      theFile.close();
    fileOpen = 0;
    return 0;
  }


int XC::FileStream::setPrecision(int prec)
  {
    if(fileOpen != 0)
      theFile << std::setprecision(prec);
    return 0;
  }

int XC::FileStream::setFloatField(floatField field)
  {
    if(field == FIXEDD)
      {
        if(fileOpen != 0)
          theFile << setiosflags(ios::fixed);
      }
    else if(field == SCIENTIFIC)
      {
        if(fileOpen != 0)
          theFile << setiosflags(ios::scientific);
      }
    return 0;
  }



XC::OPS_Stream &XC::FileStream::write(const char *s,int n)
  {
    if(fileOpen != 0)
      theFile.write(s, n);
    return *this;
  }

XC::OPS_Stream &XC::FileStream::write(const unsigned char*s,int n)
  {
    if(fileOpen != 0)
      theFile.write((const char *) s, n);
    return *this;
  }

XC::OPS_Stream &XC::FileStream::write(const signed char*s,int n)
  {
    if(fileOpen != 0)
      theFile.write((const char *) s, n);

    return *this;
  }
 XC::OPS_Stream &XC::FileStream::write(const void *s, int n)
  {
    if(fileOpen != 0)
      theFile.write((const char *) s, n);

    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const char &c)
  {
    if(fileOpen != 0)
      theFile << c;

    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const unsigned char &c)
  {
    if(fileOpen != 0)
      theFile << c;

    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const signed char &c)
  {
    if(fileOpen != 0)
      theFile << c;

    return *this;
  }

XC::OPS_Stream &XC::FileStream::operator<<(const char *s)
    {
      // note that we do the flush so that a "/n" before
      // a crash will cause a flush() - similar to what 
      if(fileOpen != 0)
        {
          theFile << s;
          theFile.flush();
        }
      return *this;
    }

XC::OPS_Stream &XC::FileStream::operator<<(const std::string &s)
    {
      // note that we do the flush so that a "/n" before
      // a crash will cause a flush() - similar to what 
      if(fileOpen != 0)
        {
          theFile << s.c_str();
          theFile.flush();
        }
      return *this;
    }

XC::OPS_Stream& XC::FileStream::operator<<(const unsigned char *s)
    {
      if(fileOpen != 0)
        theFile << s;
      return *this;
    }
 XC::OPS_Stream &XC::FileStream::operator<<(const signed char *s)
  {
    if(fileOpen != 0)
      theFile << s;

    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const void *p)
  {
/*
    if(fileOpen != 0)
      theFile << p;
*/
    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const int &n)
  {
    if(fileOpen != 0)
      theFile << 1.0*n;

    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const unsigned int &n)
  {
    if(fileOpen != 0)
      theFile << 1.0*n;

    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const long &n)
  {
/*
    if(fileOpen != 0)
      theFile << n;
*/
    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const unsigned long &n)
  {
/*
    if(fileOpen != 0)
      theFile << n;
*/
    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const short &n)
  {
/*
    if(fileOpen != 0)
      theFile << n;
*/
    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const unsigned short &n)
  {
/*
    if(fileOpen != 0)
      theFile << n;
*/
    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const bool &b)
  {
/*
    if(fileOpen != 0)
      theFile << b;
*/
    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const double &n)
  {
    if(fileOpen != 0)
      theFile << n;

    return *this;
  }
 XC::OPS_Stream &XC::FileStream::operator<<(const float &n)
  {
    if(fileOpen != 0)
      theFile << n;

    return *this;
  }
