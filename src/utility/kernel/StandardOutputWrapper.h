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
//Standard output redirection.

#ifndef STANDARD_OUTPUT_WRAPPER_H
#define STANDARD_OUTPUT_WRAPPER_H

#include <iostream>
#include <fstream>
#include <string>
#include "StandardStreamWrapper.h"

//! @ingroup BASE
//
//! @brief Standard output redirection.
class StandardOutputWrapper
  {
    StandardStreamWrapper errorStream; //!< character error stream.
    StandardStreamWrapper logStream; //!< character log stream.
    StandardStreamWrapper outStream; //!< character output stream.
  public:
    StandardOutputWrapper(void);
    void reset(void);
    // Error stream.
    const StandardStreamWrapper &getErrorStreamWrapper(void) const;
    StandardStreamWrapper &getErrorStreamWrapper(void);
    std::ostream &getErrStream(void);
    const std::string &getErrFileName(void) const;
    void setErrFileName(const std::string &filename);
    // Log stream.
    const StandardStreamWrapper &getLogStreamWrapper(void) const;
    StandardStreamWrapper &getLogStreamWrapper(void);
    std::ostream &getLogStream(void);
    const std::string &getLogFileName(void) const;
    void setLogFileName(const std::string &filename);
    // Output stream.
    const StandardStreamWrapper &getOutputStreamWrapper(void) const;
    StandardStreamWrapper &getOutputStreamWrapper(void);
    std::ostream &getOutputStream(void);
    const std::string &getOutputFileName(void) const;
    void setOutputFileName(const std::string &filename);
  };

#endif
