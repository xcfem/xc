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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/EigenAnalysis.h,v $
                                                                        
                                                                        
// File: ~/analysis/analysis/eigenAnalysis/EigenAnalysis.h
//
// Written: Jun Peng
// Created: Wed Jan 27, 1999
// Revision: A
//
// Description: This file contains the class definition of EigenAnalysis.
// EigenAnalysis is a subclass of Analysis, it is used to perform the 
// eigen value analysis on the FE_Model.
//
// This class is inheritanted from the base class of Analysis
// which was created by fmk (Frank).


#ifndef EigenAnalysis_h
#define EigenAnalysis_h

#include <solution/analysis/analysis/Analysis.h>

namespace XC {
  class Vector;
  class Matrix;

//! @ingroup TipoAnalisis
//
//! @brief Análisis de autovalores y vectores propios.
class EigenAnalysis : public Analysis
  {
  protected:
    int domainStamp;

    bool procesa_comando(CmdStatus &status);

    friend class ProcSolu;
    EigenAnalysis(SoluMethod *metodo);
    Analysis *getCopy(void) const;
  public:     
    virtual int analyze(int numModes);
    void clearAll(void);
    virtual int domainChanged(void);
     
    virtual int setAlgorithm(EigenAlgorithm &theAlgo);
    virtual int setIntegrator(EigenIntegrator &theIntegrator);
    virtual int setEigenSOE(EigenSOE &theSOE);  

    //Autovectores.
    virtual const Vector &getEigenvector(int mode) const;
    Vector getNormalizedEigenvector(int mode) const;
    Matrix getEigenvectors(void) const;
    Matrix getNormalizedEigenvectors(void) const;

    //Autovalores.
    virtual const double &getEigenvalue(int mode) const;
    double getPulsacion(int mode) const;
    double getPeriodo(int mode) const;
    double getFrecuencia(int mode) const;
    Vector getEigenvalues(void) const;
    Vector getPulsaciones(void) const;
    Vector getPeriodos(void) const;
    Vector getFrecuencias(void) const;
    int getNumModes(void) const;

    //Factores de participación modal
    virtual double getModalParticipationFactor(int mode) const;
    Vector getModalParticipationFactors(void) const;

    //Factores de distribución
    Vector getDistributionFactor(int mode) const;
    Matrix getDistributionFactors(void) const;

    //Masas modales efectivas.
    double getEffectiveModalMass(int mode) const;
    Vector getEffectiveModalMasses(void) const;
    double getTotalMass(void) const;

    //Fuerza estática equivalente.
    Vector getEquivalentStaticLoad(int mode,const double &) const;

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif

