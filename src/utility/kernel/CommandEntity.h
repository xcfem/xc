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
//CommandEntity.h

#ifndef ENTCMD_H
#define ENTCMD_H

#include "EntityWithProperties.h"
#include "utility/utils/text/text_string.h"
#include "utility/kernel/StandardOutputWrapper.h"

namespace boost
  {
    class any;
  }

typedef enum{FALLO,CONTINUA,COMPLETADO} resul_lectura;

//! @ingroup KERNEL
//
//! @brief Objet that can execute python scripts.
class CommandEntity: public EntityWithProperties
  {
  private:
    static StandardOutputWrapper standardOutput; //!< Streams para errores y avisos.
  protected:
    static CommandEntity *entcmd_cast(boost::any &data);

    template <class T>
    void string_to(T &,const std::string &) const;
  public:
    CommandEntity(CommandEntity *owr= nullptr);

    CommandEntity *Owner(void);
    const CommandEntity *Owner(void) const;

    const StandardOutputWrapper &getStandardOutputWrapper(void) const;
    StandardOutputWrapper &getStandardOutputWrapper(void);
    const std::string &getLogFileName(void) const;
    void setLogFileName(const std::string &);
    const std::string &getErrFileName(void) const;
    void setErrFileName(const std::string &);
    const std::string &getOutputFileName(void) const;
    void setOutputFileName(const std::string &);
    static void resetStandardOutput(void);

   
    boost::python::object evalPy(boost::python::object dict,const std::string &);
    boost::python::object execPy(boost::python::object dict,const std::string &);
    boost::python::object execFilePy(boost::python::object dict,const std::string &);
  };

#endif
