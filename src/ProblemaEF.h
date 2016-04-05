//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
    static inline const std::string &getVersion(void)
      { return gVERSION; }
    static inline const std::string &getVersionShort(void)
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
} // end of XC namespace

#endif
