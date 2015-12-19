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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/03/05 00:53:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete01.h,v $
                                                                        
                                                                        
#ifndef Concrete01_h
#define Concrete01_h

// File: Concrete01.h
//
// Written: MHS 
// Created: 06/99
// Revision: A
//
//
// What: "@(#) Concrete01.h, revA"


#include <material/uniaxial/concrete/ConcreteBase.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Modelización 1D del hormigón.
//! Uniaxial Kent-Scott-Park concrete model with linear
//! unloading/reloading according to the work of Karsan-Jirsa and no
//! strength in tension. The model contains a compressive strength of fpc,
//! a strain at the compressive strength of epsc0, a crushing strength of
//! fpcu, and a strain at the crushing strength of epscu. Compressive
//! concrete parameters should be input as negative numeric values for
//! this model to behave properly. Specification of minimum and maximum
//! failure strains through the -min and -max switches is optional. The
//! argument matTag is used to uniquely identify the material object among
//! material objects in the BasicBuilder object.
class Concrete01: public ConcreteBase
  {
  private:
      /*** Material Properties ***/
    double fpcu; //!< Crushing strength

    void make_negative(void);
    void setup_parameters(void);
    void determineTrialState(double dStrain);

    void reload(void);
    void unload(void);
    void envelope(void);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    void libera(void);
    void alloc(const Matrix &);
    int parameterID;
    Matrix *SHVs;
// AddingSensitivity:END ///////////////////////////////////////////
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
    void calcula_trial_state(const double &strain);
  public:
    Concrete01(int tag, double fpc, double eco, double fpcu, double ecu);
    Concrete01(int tag);
    Concrete01(void);
    Concrete01(const Concrete01 &);
    Concrete01 &operator=(const Concrete01 &);
    ~Concrete01(void);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);
    inline double getInitialTangent(void) const
      {return 2.0*fpc/epsc0;}

    void setFpcu(const double &d);
    double getFpcu(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
    virtual any_const_ptr GetProp(const std::string &cod) const;

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    double getStressSensitivity(int gradNumber, bool conditional);
    int    commitSensitivity(const double &strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace


#endif


