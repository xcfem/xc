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
// $Date: 2003/02/25 23:33:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ElasticPPMaterial.h,v $
                                                                        
#ifndef ElasticPPMaterial_h
#define ElasticPPMaterial_h

// File: ~/material/ElasticPPMaterial.h
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// ElasticPPMaterial. ElasticPPMaterial provides the abstraction
// of an elastic perfectly plastic uniaxial material, 
//
// What: "@(#) ElasticPPMaterial.h, revA"

#include <material/uniaxial/EPPBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Material elasto-plástico perfecto.
class ElasticPPMaterial : public EPPBaseMaterial
  {
  private:
    double fyp, fyn; //!< positive and negative yield stress

    //! @brief Evalúa la función de cedencia.
    inline double yield_function(const double &sigtrial) const
      {
        if(sigtrial>=0.0)
          return (sigtrial - fyp);
        else
          return (-sigtrial + fyn);
      }
  protected:
    inline double def_total(void) 
      { return EPPBaseMaterial::def_total()-commitStrain; }
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    ElasticPPMaterial(int tag, double E, double eyp);    
    ElasticPPMaterial(int tag, double E, double eyp, double eyn, double ezero);    
    ElasticPPMaterial(int tag);    
    ElasticPPMaterial(void);

    void set_fyp(const double &);
    void set_eyp(const double &);
    void set_fyn(const double &);
    void set_eyn(const double &);
    double get_fyp(void) const;
    double get_eyp(void) const;
    double get_fyn(void) const;
    double get_eyn(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0); 

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);    

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC


#endif



