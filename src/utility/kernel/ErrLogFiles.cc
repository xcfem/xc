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
//ErrLogFiles.cc

#include "ErrLogFiles.h"


ErrLogFiles::ErrLogFiles(void)
  : nmbErrFile("cerr"), salida_err_file(nullptr),
    nmbLogFile("clog"), salida_log_file(nullptr),
    stream_buffer_cerr(std::cerr.rdbuf()),
    stream_buffer_clog(std::clog.rdbuf())
  {}

std::ostream &ErrLogFiles::getErrFile(void)
  {
    if(salida_err_file)
      return *salida_err_file;
    else
      return std::cerr;
  }

void ErrLogFiles::setErrFileName(const std::string &filename)
  {
    if(salida_err_file)
      {
        delete salida_err_file;
        salida_err_file= nullptr;
        nmbErrFile= "";
      }
    if(filename=="cerr")
      std::cerr.rdbuf(stream_buffer_cerr);
    else
      {
	nmbErrFile= filename;
	if(nmbErrFile!=nmbLogFile)
	  {
	    if(nmbErrFile!="cerr")
	      salida_err_file= new std::ofstream(filename.c_str());
	  }
	else
	  salida_err_file= salida_log_file;
	if(salida_err_file)
	  std::cerr.rdbuf(salida_err_file->rdbuf());
	else
	  std::cerr << "ErrLogFiles::" << __FUNCTION__
		    << "; error redirecting default error stream.";
      }
  }

std::ostream &ErrLogFiles::getLogFile(void)
  {
    if(salida_log_file)
      return *salida_log_file;
    else
      return std::clog;
  }

void ErrLogFiles::setLogFileName(const std::string &filename)
  {
    if(salida_log_file)
      {
        delete salida_log_file;
        salida_log_file= nullptr;
        nmbLogFile= "";
      }
    if(filename=="clog")
      std::clog.rdbuf(stream_buffer_clog);
    else
      {
	nmbLogFile= filename;
	if(nmbLogFile!=nmbErrFile)
	  {
	    if(nmbLogFile!="clog")
	      salida_log_file= new std::ofstream(filename.c_str());
	  }
	else
	  salida_log_file= salida_err_file;
	if(salida_log_file)
	  std::clog.rdbuf(salida_log_file->rdbuf());
	else
	  std::cerr << "ErrLogFiles::" << __FUNCTION__
		    << "; error redirecting default 'log' stream.";
      }
  }
