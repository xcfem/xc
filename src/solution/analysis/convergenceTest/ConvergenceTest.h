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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/12/15 00:19:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/convergenceTest/ConvergenceTest.h,v $
                                                                        
                                                                        
#ifndef ConvergenceTest_h
#define ConvergenceTest_h

// Written: fmk 
// Date: 09/98
// Revised:
//
// Purpose: This file contains the class definition for ConvergenceTest,
// which is an abstract class. Objects of concrete subclasses can be used 
// to test the convergence of an algorithm. 

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/nucleo/EntWOwner.h"
#include "utility/matrix/Vector.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"

namespace XC {
class EquiSolnAlgo;
class SoluMethod;

//! @ingroup CTest
//
//! @brief Test de convergencia.
class ConvergenceTest: public MovableObject, public EntWOwner
  {
    SoluMethod *getSoluMethod(void);
    const SoluMethod *getSoluMethod(void) const;
  protected:
    int currentIter; //!< number of times test() has been invokes since last start()
    int maxNumIter;  //!< max number of iterations
    int printFlag; //!< a flag indicating if to print on test
    int nType; //!< type of norm to use (1-norm, 2-norm, p-norm, max-norm)
    Vector norms; //!< vector to hold the norms

    //Intermediate results:
    mutable double lastRatio; //!< Last calculated ratio (if any).
    mutable double calculatedNormX; //!< Last calculated |x|
    mutable double calculatedNormB; //!< Last calculated |b|
    mutable double calculatedEnergyProduct; //!< Last calculated |0.5*(x ^ b)|.

    bool hasLinearSOE(void) const;
    LinearSOE *getLinearSOEPtr(void);
    const LinearSOE *getLinearSOEPtr(void) const;
    const Vector &getX(void) const;
    double getNormX(void) const;
    const Vector &getB(void) const;
    double getNormB(void) const;
    double getEnergyProduct(void) const;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    ConvergenceTest(EntCmd *owr,int classTag);	
    ConvergenceTest(EntCmd *owr,int clasTag,int maxIter,int prtFlg, int normType,int sz_norms= 1);
    inline virtual ~ConvergenceTest(void) {}

    virtual ConvergenceTest *getCopy(void) const= 0;
    virtual ConvergenceTest *getCopy(int iterations) const;

    virtual std::string getStatusMsg(const int &flag= 1) const= 0;
    virtual int start(void);
    virtual int test(void)= 0;

    int getMaxNumIter(void) const;
    void setMaxNumIter(const int &);
    int getCurrentIter(void) const;
    void setCurrentIter(const int &);
    int getPrintFlag(void) const;
    void setPrintFlag(const int &);
    int getNormType(void) const;
    void setNormType(const int &);
        
    virtual int getNumTests(void) const;
    virtual int getMaxNumTests(void) const;        
    virtual double getRatioNumToMax(void) const;            
    virtual const Vector &getNorms(void) const;
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    std::string getTestIterationMessage(void) const;
    std::string getFailedToConvergeMessage(void) const;
    std::string getDeltaXRMessage(void) const;
    std::string getDeltaXRNormsMessage(void) const;
  };
} // end of XC namespace


#endif

