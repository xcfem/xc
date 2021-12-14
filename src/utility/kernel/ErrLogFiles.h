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
//Arhivo para imprimir errores y avisos.

#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <iostream>
#include <fstream>
#include <string>

//! @ingroup BASE
//
//! @brief Archivos para imprimir errores y avisos.
class ErrLogFiles
  {
    std::string nmbErrFile;
    std::ofstream *salida_err_file;
    std::string nmbLogFile;
    std::ofstream *salida_log_file;
    std::streambuf *stream_buffer_cerr;
    std::streambuf *stream_buffer_clog;
  public:
    ErrLogFiles(void);
    std::ostream &getErrFile(void);
    std::ostream &getLogFile(void);
    inline const std::string &getErrFileName(void) const
      { return nmbErrFile; }
    void setErrFileName(const std::string &filename);
    inline const std::string &getLogFileName(void) const
      { return nmbLogFile; }
    void setLogFileName(const std::string &filename);
  };

#endif
