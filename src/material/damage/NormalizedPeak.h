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
                                                                        
#ifndef NormalizedPeak_h
#define NormalizedPeak_h         

// $Revision: 1.1 $
// $Date: 2004/09/01 03:54:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/damage/NormalizedPeak.h,v $

// Written: Arash Altoontash, Gregory Deierlein
// Created: 08/02
// Revision: AA
//
// Description: This file contains the class definition for NormalizedPeak
// Damage model. This is a maximum based, non hysteretic damage model. 
// It gets the maximum positive and maximum negative values as initial
// parameters and calculates the damage index based on the maximum and minimum
// values occured.


#include <utility/handler/ErrorHandler.h>
#include <material/damage/DamageModel.h>

namespace XC {
class DamageResponse;
class Element;


class NormalizedPeak : public DamageModel
  {
  private:
    std::string damagename;
    DamageType damagetype;
  
    // Model parameters
    double MaxValue, MinValue;
  
    // Trial step
    double TrialScalar;
    double TrialDmg;
    Vector TrialVector;
  
    // Commited state
    double CommitScalar;
    double CommitDmg;
    Vector CommitVector;
  
    // Last commit
    double LCommitScalar;
    double LCommitDmg;
    Vector LCommitVector;
  public:
    NormalizedPeak(int tag, double maxVal, double minVal , const char *argv);
    NormalizedPeak(void);  
  
    int setTrial(const Vector &trialVector);
    int setTrial(void) { return -1; }
  
    double getDamage(void);
    double getPosDamage(void);
    double getNegDamage(void);
  
    int commitState(void);
    int revertToLastCommit (void);
    int revertToStart (void);
  
    DamageModel *getCopy(void) const;
  
    int setParameter(const std::vector<std::string> &argv, Information &eleInformation) { return -1; }
    int updateParameter(int responseID, Information &eleInformation) { return -1; }
  
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &info);
  
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif
