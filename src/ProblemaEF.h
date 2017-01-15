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

namespace XC {
class Domain;
class FE_Datastore;
class FEM_ObjectBrokerAllClasses;
class MEDMesh;
class MEDMeshing;

//! @mainpage XC programa de elementos finitos de fuente abierta.
//! @author Luis C. Pérez Tato/Ana Isabel Ortega Ortega.
//!
//! XC es un código de elementos finitos, destinado a resolver
//! problemas de análisis estructural y permitir la comprobación
//! de los resultados obtenidos de acuerdo con los criterios
//! establecidos por las normas (EHE, eurocódigos, CTE,...).
//!
//! La aplicación puede resolver varios tipos de problemas, desde un simple
//! análisis lineal hasta simulaciones complejas no lineales. Posee
//! una biblioteca de elementos finitos que permite modelar distintas
//! geometrías, así como múltiples modelos de material, permitiendo
//! su aplicación en distintas áreas del análisis de estructuras.
//!
//! El programa está basado en el núcleo de cálculo de
//! <a href="http://opensees.berkeley.edu"target="_new">OpenSees.</a>.
//! Para su desarrollo se han empleado las siguientes bibliotecas:
//! <ul>
//! <li> <a href="http://www.vtk.org" target="_new">Visualization ToolKit.</a> </li>
//! <li> <a href="http://www.boost.org" target="_new">Boost c++ libraries.</a> </li>
//! <li> <a href="http://www.cgal.org" target="_new">Computational geometry algorithms library.</a> </li>
//! <li> GNU plotutils libraries (libplot).</a> </li>
//! <li> GNU plotutils libraries (libplot).</li>
//! <li> Multiple precision floating-point computation developers tools (libmpfr).</li>
//! <li> Multiprecision arithmetic library developers tools (libgmp).</li>
//! <li> MySQL C++ library bindings (libmysql++).</li>
//! <li> Berkeley Database Libraries for C++ (libdb,libdb-dev).</li>
//! <li> SQLite 3 development files (libsqlite3).</li>
//! <li> X11 (libxt-dev, libsm-dev,etc.)</li>
//! <li> OpenGL (archivos de cabecera para OpenGL).</li>
//! <li> Arpack (Fortran77 subroutines to solve large scale eigenvalue problems; libarcpack2-dev).</li>
//! <li> Lapack (Library of linear algebra routines; liblapack-dev).</li>
//! <li> SuperLU (Direct solution of large, sparse systems of linear equations libsuperlu3-dev).</li>
//! <li> Suitesparse (collection of libraries for computations for sparse matrices).</li>
//! </ul>
//! 
//! El programa puede descargarse en la dirección url: <a href="http://www.xcingenieria.com" target="_new">http://www.xcingenieria.com</a>. 

//! @brief Problema de elementos finitos.
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
