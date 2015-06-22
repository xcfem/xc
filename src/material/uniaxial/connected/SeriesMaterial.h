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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/25 23:33:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/SeriesMaterial.h,v $

#ifndef SeriesMaterial_h
#define SeriesMaterial_h

// Written: MHS
// Created: Sept 2000
//
// Description: This file contains the class definition for 
// SeriesMaterial. SeriesMaterial is an aggregation
// of UniaxialMaterial objects all considered acting in Series.

#include "ConnectedMaterial.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Materiales uniaxiales conectados en serie.
class SeriesMaterial : public ConnectedMaterial
  {
  private:
    double Tstrain;
    double Cstrain;

    double Tstress;
    double Cstress;

    double Ttangent;
    double Ctangent;

    int maxIterations;
    double tolerance;

    std::vector<double> stress;
    std::vector<double> flex;
    std::vector<double> strain;

    bool initialFlag;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    SeriesMaterial(int tag,const DqUniaxialMaterial &theMaterials, int maxIter = 1, double tol = 1.0e-10);
    SeriesMaterial(int tag= 0);


    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;          
    double getStress(void) const;
    double getTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);

    Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    int getResponse(int responseID, Information &matInformation);
  };
} // fin namespace XC


#endif

