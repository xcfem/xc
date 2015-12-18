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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.1 $
// $Date: 2003/03/06 18:34:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/BoucWenMaterial.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef BoucWenMaterial_h
#define BoucWenMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
  class Matrix;

//! @ingroup MatUnx
//
//! @brief ??.
class BoucWenMaterial : public UniaxialMaterial
  {
  private:
    // Material parameters
    double alpha;
    double ko;
    double n;
    double gamma;
    double beta;
    double Ao;
    double deltaA;
    double deltaNu;
    double deltaEta;

    // History variables(trial and commited)
    double Tstrain, Cstrain;
    double Tz, Cz;
    double Te, Ce;

    // Ohter variables
    double Tstress, Ttangent;

    double tolerance;
    int maxNumIter;

    // Sensitivit stuff
    int parameterID;
    Matrix *SHVs;
  public:
    BoucWenMaterial(int tag, double alpha, double ko, double n, double gamma, double beta, double Ao,
                             double deltaA, double deltaNu, double deltaEta, double tolerance, int maxNumIter);
    BoucWenMaterial(void);
    ~BoucWenMaterial(void);
    int setTrialStrain(double strain, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    static double signum(double);
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    UniaxialMaterial *getCopy(void) const;
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

    // Reliability and sensitivity stuff
    double getInitialTangent(void) const;
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    double getStressSensitivity(int gradNumber, bool conditional);
    double getStrainSensitivity(int gradNumber);
    double getTangentSensitivity(int gradNumber);
    double getDampTangentSensitivity(int gradNumber);
    double getRhoSensitivity(int gradNumber);
    int    commitSensitivity(double strainGradient, int gradNumber, int numGrads);
  };
} // end of XC namespace


#endif

