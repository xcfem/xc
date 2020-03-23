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
                                                                        
// $Revision: 1.3 $
// $Date: 2004/11/13 00:53:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/FileStream.h,v $

#ifndef _FileStream
#define _FileStream

#include <utility/handler/OPS_Stream.h>

#include <fstream>
#include <string>

namespace XC {
class FileStream: public OPS_Stream
  {
  private:
    std::ofstream theFile;
    int fileOpen;
    std::string fileName;
  public:
    FileStream(void);
    virtual ~FileStream(void);


    //int setFile(const char *fileName, openMode mode = OVERWRITE);
    int setFile(const std::string &fileName, openMode mode = OVERWRITE);
    int open(void);
    int close(void);

    int setPrecision(int precision);
    int setFloatField(floatField);
    int precision(int precision) {return 0;};
    int width(int width) {return 0;};
    inline const std::string &getFileName(void) const
      {return fileName;}
    std::ostream &stream(void)
      { return theFile; }
   
    OPS_Stream& write(const char *s, int n);
    OPS_Stream& write(const unsigned char *s, int n);
    OPS_Stream& write(const signed char *s, int n);
    OPS_Stream& write(const void *s, int n);
    OPS_Stream& operator<<(const char &c);
    OPS_Stream& operator<<(const unsigned char &c);
    OPS_Stream& operator<<(const signed char &c);
    OPS_Stream& operator<<(const char *s);
    OPS_Stream& operator<<(const std::string &s);
    OPS_Stream& operator<<(const unsigned char *s);
    OPS_Stream& operator<<(const signed char *s);
    OPS_Stream& operator<<(const void *p);
    OPS_Stream& operator<<(const int &n);
    OPS_Stream& operator<<(const unsigned int &n);
    OPS_Stream& operator<<(const long &n);
    OPS_Stream& operator<<(const unsigned long &n);
    OPS_Stream& operator<<(const short &n);
    OPS_Stream& operator<<(const unsigned short &n);
    OPS_Stream& operator<<(const bool &b);
    OPS_Stream& operator<<(const double &n);
    OPS_Stream& operator<<(const float &n);
    // OPS_Stream& operator<<(const __omanip func);
    // OPS_Stream& operator<<(const __manip func);
    // OPS_Stream& operator<<(const streambuf*);
    // OPS_Stream& ends(OPS_Stream& outs);
    // OPS_Stream& flush(OPS_Stream& outs);
    // OPS_Stream& (OPS_Stream& outs);
  };
} // end of XC namespace

#endif
