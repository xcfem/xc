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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/25 23:33:36 $
// $Source: material/section/fiber_section/fiber/UniaxialFiber2d.h,v $
                                                                        
                                                                        
// File: ~/fiber/UniaxialFiber2d.h
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the class definition for 
// UniaxialFiber2d.h. UniaxialFiber2d provides the abstraction of a
// uniaxial fiber whose position  is defined with only one coordinate.
// The UniaxialFiber2d is subjected to a stress state with 
// only one nonzero axial stress and corresponding axial strain.
//
// What: "@(#) UniaxialFiber2d.h, revA"

#ifndef UniaxialFiber2d_h
#define UniaxialFiber2d_h

#include "UniaxialFiber.h"

namespace XC {
class UniaxialMaterial;

//! \ingroup MATSCCFibras
//
//! @brief Representa una fibra de material uniaxial en un espacio bidimensional (en el que las secciones tienen 1 dimensión).
class UniaxialFiber2d: public UniaxialFiber
  {
  private:
    double y; //!< Position of the fiber
              //(its sign is changed -see comments along the file-). 

    static Matrix ks; //!< static class wide matrix object for returns
    static Vector fs; //!< static class wide vector object for returns
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    UniaxialFiber2d(void);   
    UniaxialFiber2d(int tag, UniaxialMaterial &, const double &,const double &);
    UniaxialFiber2d(int tag,const MaterialLoader &,const std::string &nmbMat,const double &, const double &);
    
    int setTrialFiberStrain(const Vector &vs);
    Vector &getFiberStressResultants(void);
    Matrix &getFiberTangentStiffContr(void);

    Fiber *getCopy(void) const;
    int getOrder(void) const;
    const ResponseId &getType(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
    
    Response *setResponse(const std::vector<std::string> &argv, Information &info);
    int getResponse(int responseID, Information &info);
	
    void getFiberLocation(double &y, double &z) const;
    virtual double getLocY(void) const
      { return -y; } //Sign of Y coordinate is changed.
  };
} // end of XC namespace


#endif






