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
//StandardStreamWrapper.cc

#include "StandardStreamWrapper.h"

//! @brief Frees memory.
void StandardStreamWrapper::free(void)
  {
    if(customStream)
      {
        delete customStream;
        customStream= nullptr;
        fileName= "";
      }
  }

//! @brief Allocates memory.
void StandardStreamWrapper::alloc(const std::string &fName)
  {
    free();
    if(fName!=fileName) // Not the same file.
      {
	customStream= new std::ofstream(fName.c_str());
	if(customStream)
	  standardStream.rdbuf(customStream->rdbuf());
	else
	  std::cerr << "StandardStreamWrapper::" << __FUNCTION__
		    << "; error redirecting standard stream.";
	fileName= fName;
      }
  }

//! @brief Fall back to the standard buffer.
void StandardStreamWrapper::reset(void)
  {
    free();
    // Fall back to the standard buffer.
    standardStream.rdbuf(standardStreamBuffer);
  }

//! @brief Constructor.
StandardStreamWrapper::StandardStreamWrapper(const std::string &fName, std::ostream &stream)
  : fileName(fName), standardStream(stream), customStream(nullptr),
    standardStreamBuffer(stream.rdbuf())
  {}

//! @brief Destructor.
StandardStreamWrapper::~StandardStreamWrapper(void)
  { reset(); }

std::ostream &StandardStreamWrapper::getStream(void)
  {
    if(customStream)
      return *customStream;
    else
      return standardStream;
  }

void StandardStreamWrapper::setFileName(const std::string &filename)
  {
    if((filename=="cerr") || (filename=="clog") || (filename=="cout"))
      { reset(); }
    else
      {	alloc(filename); }
  }

