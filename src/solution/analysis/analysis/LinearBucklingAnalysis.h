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
//LinearBucklingAnalysis.h
                                                                        
                                                                        
#ifndef LinearBucklingAnalysis_h
#define LinearBucklingAnalysis_h


// Description: This file contains the interface for the LinearBucklingAnalysis
// class. LinearBucklingAnalysis is a subclass of StaticAnalysis, it is used to perform 
// a linear buckling analysis on the FE\_Model.

#include <solution/analysis/analysis/StaticAnalysis.h>
#include "LinearBucklingEigenAnalysis.h"


namespace XC {
class ConvergenceTest;
class EigenAnalysis;
class LinearBucklingAlgo;
class LinearBucklingIntegrator;
class Vector;
class ArpackSOE;
class ArpackSolver;

//! @ingroup TipoAnalisis
//
//! @brief Análisis de pandeo lineal.
class LinearBucklingAnalysis: public StaticAnalysis
  {
  private:
    SoluMethod *eigen_solu; //!< Estrategia de solución para el problema de autovalores.
    LinearBucklingEigenAnalysis linearBucklingEigenAnalysis;
    int numModes;
    int linear_buckling_analysis_step; //!Paso en el que se inicia el linear buckling analysis.
  protected:
    bool procesa_comando(CmdStatus &status);

    friend class ProcSolu;
    LinearBucklingAnalysis(SoluMethod *metodo,SoluMethod *eigen_solu);
    Analysis *getCopy(void) const;
  public:

    void clearAll(void);	    
    
    int analyze(int numSteps);
    int domainChanged(void);

    int getNumModes(void) const
      { return numModes; }
    void setNumModes(const int &nm)
      { numModes= nm; }

    int setLinearBucklingAlgorithm(LinearBucklingAlgo &);
    int setLinearBucklingIntegrator(LinearBucklingIntegrator &);
    int setArpackSOE(ArpackSOE &theSOE);
    virtual const Vector &getEigenvector(int mode);
    virtual const double &getEigenvalue(int mode) const;
  };
inline Analysis *LinearBucklingAnalysis::getCopy(void) const
  { return new LinearBucklingAnalysis(*this); }
} // end of XC namespace

#endif
