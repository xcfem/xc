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
//log_file.h
//Standard stream redirection.

#ifndef STANDARD_STREAM_WRAPPER_H
#define STANDARD_STREAM_WRAPPER_H

#include <iostream>
#include <fstream>
#include <string>

//! @ingroup BASE
//
//! @brief Standard stream redirection.
class StandardStreamWrapper
  {
    std::string fileName;
    std::ostream &standardStream;
    std::ofstream *customStream;
    std::streambuf *standardStreamBuffer;
  protected:
    void free(void);
    void alloc(const std::string &);
  public:
    StandardStreamWrapper(const std::string &, std::ostream &);
    ~StandardStreamWrapper(void);
    void reset(void);
    std::ostream &getStream(void);
    inline const std::string &getFileName(void) const
      { return fileName; }
    void setFileName(const std::string &);
  };

#endif
