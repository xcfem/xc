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
//LinearBucklingEigenAnalysis.h


#ifndef LinearBucklingEigenAnalysis_h
#define LinearBucklingEigenAnalysis_h

#include <solution/analysis/analysis/EigenAnalysis.h>

namespace XC {
  class Vector;
  class LinearBucklingAlgo;
  class ArpackSOE;

//! @ingroup TipoAnalisis
//
//! @brief Análisis de pandeo lineal (se emplea dentro de StaticAnalysis).
class LinearBucklingEigenAnalysis: public EigenAnalysis
  {
  protected:
    bool procesa_comando(CmdStatus &status);

    friend class ProcSolu;
    friend class LinearBucklingAnalysis;
    LinearBucklingEigenAnalysis(SoluMethod *metodo);
    Analysis *getCopy(void) const;
  public:
     
    virtual int setupPreviousStep(void);
     
    virtual int setAlgorithm(LinearBucklingAlgo &theAlgo);
    virtual int setIntegrator(LinearBucklingIntegrator &theIntegrator);
    virtual int setEigenSOE(ArpackSOE &theSOE);
    virtual const double &getEigenvalue(int mode) const;
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
inline Analysis *LinearBucklingEigenAnalysis::getCopy(void) const
  { return new LinearBucklingEigenAnalysis(*this); }
} // end of XC namespace

#endif

