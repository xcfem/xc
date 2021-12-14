// -*-c++-*-
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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/25 23:33:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/StandardStream.h,v $

#ifndef _StandardStream
#define _StandardStream

#include "OPS_Stream.h"

namespace XC {
//! @ingroup StreamsGrp
//
//! @brief C++ standard stream wrapper.
class StandardStream: public OPS_Stream
  {
   public:
    StandardStream(void);

    std::ofstream &stream(void)
      { return theFile; }

    OPS_Stream &write(const char *s, int n);
    OPS_Stream &write(const unsigned char *s, int n);
    OPS_Stream &write(const signed char *s, int n);
    OPS_Stream &write(const void *s, int n);
    OPS_Stream &write(const char &c);
    OPS_Stream &write(const unsigned char &c);
    OPS_Stream &write(const signed char &c);
    OPS_Stream &write(const char *s);
    OPS_Stream &write(const std::string &s);
    OPS_Stream &write(const unsigned char *s);
    OPS_Stream &write(const signed char *s);
    OPS_Stream &write(const void *p);
    OPS_Stream &write(const int &n);
    OPS_Stream &write(const unsigned int &n);
    OPS_Stream &write(const long &n);
    OPS_Stream &write(const unsigned long &n);
    OPS_Stream &write(const short &n);
    OPS_Stream &write(const unsigned short &n);
    OPS_Stream &write(const bool &b);
    OPS_Stream &write(const double &d);
    OPS_Stream &write(const float &f);
  };
} // end of XC namespace

#endif
