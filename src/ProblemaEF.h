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
//ProblemaEF.h

#ifndef PROBLEMAEF_H
#define PROBLEMAEF_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "version.h"
#include "preprocessor/Preprocessor.h"
#include "solution/ProcSolu.h"
#include "post_process/MapFields.h"
#include "utility/handler/DataOutputHandler.h"

//! @brief Open source finite element program for structural analysis
namespace XC {
class Domain;
class FE_Datastore;
class FEM_ObjectBrokerAllClasses;
class MEDMesh;
class MEDMeshing;

//! @mainpage XC Open source finite element analysis program.
//! @author Luis C. Pérez Tato/Ana Ortega.
//!
//! XC is a finite element analysis code designed to solve
//! structural problems and to allow the checking of the results
//! following structural codes (Eurocodes, ACI, EHE, SIA,...).
//!
//! The program can solve various types of problems, from simple
//! linear static analysis to complex nonlinear simulations. It
//! has a library of finite elements which allows to model
//! various geometries, and multiple material models, allowing
//! its application in various areas of structural analysis.
//!
//! The program is based on:
//! <ul>
//! <li> <a href="http://www.python.org/" target="_new">Python.</a>. </li>
//! <li> <a href="http://opensees.berkeley.edu" target="_new">OpenSees.</a>. </li>
//! <li> <a href="http://www.vtk.org" target="_new">Visualization ToolKit.</a> </li>
//! <li> <a href="http://www.boost.org" target="_new">Boost c++ libraries.</a> </li>
//! <li> <a href="http://www.cgal.org" target="_new">Computational geometry algorithms library.</a> </li>
//! <li> <a href="http://matplotlib.org/" target="_new">Matplotlib.</a> </li>
//! <li> <a href="http://http://www.mpfr.org/" target="_new">libmpfr.</a> Multiple precision floating-point computation developers tools (libmpfr).</li>
//! <li> <a href="https://gmplib.org/" target="_new">libgmp.</a> Multiple precision arithmetic library developers tools (libgmp).</li>
//! <li> <a href="https://dev.mysql.com/downloads/connector/cpp/" target="_new">libmysql++.</a> MySQL C++ library bindings.</li>
//! <li> Berkeley Database Libraries for C++ (libdb,libdb-dev).</li>
//! <li> SQLite 3 development files (libsqlite3).</li>
//! <li> X11 (libxt-dev, libsm-dev,etc.)</li>
//! <li> OpenGL.</li>
//! <li> Arpack (Fortran77 subroutines to solve large scale eigenvalue problems; libarcpack2-dev).</li>
//! <li> Lapack (Library of linear algebra routines; liblapack-dev).</li>
//! <li> SuperLU (Direct solution of large, sparse systems of linear equations libsuperlu3-dev).</li>
//! <li> Suitesparse (collection of libraries for computations for sparse matrices).</li>
//! </ul>
//! 
//! The program can be downloaded here: <a href="https://github.com/lcpt/xc" target="_new"> GitHub repo</a>. 

//! @brief Finite element problem.
class ProblemaEF: public EntCmd
  {
    mutable DataOutputHandler::map_output_handlers output_handlers; //Manejadores para salida de resultados.
    Preprocessor preprocessor; //!< Objeto encargado de crear el modelo.
    ProcSolu proc_solu; //!< Procedimiento de solución.
    MapFields fields; //!< Definición de campos para salida de resultados.
    FE_Datastore *dataBase; //!< Base de datos para salvar estados.
    static FEM_ObjectBrokerAllClasses theBroker;

  public:
    static Domain *theActiveDomain;
    ProblemaEF(void);
    ~ProblemaEF(void);
    static inline const std::string &getXCVersion(void)
      { return gVERSION; }
    static inline const std::string &getXCVersionShort(void)
      { return gVERSION_SHORT; }
    void clearAll(void);
    FE_Datastore *defineDatabase(const std::string &tipo, const std::string &nombre);
    inline FE_Datastore *getDataBase(void)
      { return dataBase; }
    inline const Preprocessor &getPreprocessor(void) const
      { return preprocessor; }
    inline Preprocessor &getPreprocessor(void)
      { return preprocessor; }
    inline Domain *getDomain(void)
      { return preprocessor.GetDominio(); }
    inline const Domain *getDomain(void) const
      { return preprocessor.GetDominio(); }
    inline const ProcSolu &getSoluProc(void) const
      { return proc_solu; }
    inline ProcSolu &getSoluProc(void)
      { return proc_solu; }
    inline const MapFields &getFields(void) const
      { return fields; }
    inline DataOutputHandler::map_output_handlers *getOutputHandlers(void) const
      { return &output_handlers; }
  };

inline std::string getXCVersion(void)
   { return ProblemaEF::getXCVersion(); }
inline std::string getXCVersionShort(void)
   { return ProblemaEF::getXCVersionShort(); }

} // end of XC namespace

#endif
