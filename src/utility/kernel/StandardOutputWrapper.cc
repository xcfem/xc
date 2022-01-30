//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//StandardOutputWrapper.cc

#include "StandardOutputWrapper.h"


StandardOutputWrapper::StandardOutputWrapper(void)
  : errorStream("cerr", std::cerr),
    logStream("clog", std::clog),
    outStream("cout", std::cout)
  {}

//! @brief Fall back to the standard buffers.
void StandardOutputWrapper::reset(void)
  {
    errorStream.reset();
    logStream.reset();
    outStream.reset();
  }

//! @brief return the stream wrapper for errors.
const StandardStreamWrapper &StandardOutputWrapper::getErrorStreamWrapper(void) const
  { return errorStream; }

//! @brief return the stream wrapper for errors.
StandardStreamWrapper &StandardOutputWrapper::getErrorStreamWrapper(void)
  { return errorStream; }

//! @brief return the current error stream.
std::ostream &StandardOutputWrapper::getErrStream(void)
  { return errorStream.getStream();  }

//! @brief Set the file to use for standard error logging.
void StandardOutputWrapper::setErrFileName(const std::string &filename)
  { errorStream.setFileName(filename); }

//! @brief return the current file name for the error stream.
const std::string &StandardOutputWrapper::getErrFileName(void) const
  { return errorStream.getFileName(); }

//! @brief return the stream wrapper for logging.
const StandardStreamWrapper &StandardOutputWrapper::getLogStreamWrapper(void) const
  { return logStream; }

//! @brief return the stream wrapper for logging.
StandardStreamWrapper &StandardOutputWrapper::getLogStreamWrapper(void)
  { return logStream; }

//! @brief return the current log stream.
std::ostream &StandardOutputWrapper::getLogStream(void)
  { return logStream.getStream(); }

//! @brief Set the file to use for standard logging.
void StandardOutputWrapper::setLogFileName(const std::string &filename)
  { logStream.setFileName(filename); }

//! @brief return the current file name for the log stream.
const std::string &StandardOutputWrapper::getLogFileName(void) const
  { return logStream.getFileName(); }

//! @brief return the stream wrapper for regular output.
const StandardStreamWrapper &StandardOutputWrapper::getOutputStreamWrapper(void) const
  { return outStream; }

//! @brief return the stream wrapper for regular output.
StandardStreamWrapper &StandardOutputWrapper::getOutputStreamWrapper(void)
  { return outStream; }

//! @brief return the current output stream.
std::ostream &StandardOutputWrapper::getOutputStream(void)
  { return outStream.getStream(); }

//! @brief Set the file to use for standard output.
void StandardOutputWrapper::setOutputFileName(const std::string &filename)
  { outStream.setFileName(filename); }

//! @brief return the current file name for the output stream.
const std::string &StandardOutputWrapper::getOutputFileName(void) const
  { return outStream.getFileName(); }
