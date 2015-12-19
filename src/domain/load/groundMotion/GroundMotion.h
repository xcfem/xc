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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/03/04 00:48:11 $
// $Source: domain/load/groundMotion/GroundMotion.h,v $
                                                                        
                                                                        
// File: ~/earthquake/GroundMotion.h
// 
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class definition for GroundMotion.
// GroundMotion is an abstract base class, i.e. no objects of it's
// type can be created. A GroundMotion describes the motion of the ground
// in a dynamic analysis.
//
// What: "@(#) GroundMotion.h, revA"

#ifndef GroundMotion_h
#define GroundMotion_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {
  class TimeSeries;
  class Information;

//! \ingroup Loads
//!
//!
//! @defgroup GMotions Movimientos del terreno (terremotos).
//
//! @ingroup GMotion
//
//! @brief Clase base para la definición de sismos.
class GroundMotion: public MovableObject, public EntCmd
  {
  protected:
    mutable Vector data;
  public:
    GroundMotion(int classTag);

    virtual ~GroundMotion(void)
      {}

    virtual GroundMotion *getCopy(void) const= 0;

    virtual double getDuration(void) const= 0;

    virtual double getPeakAccel() const= 0;
    virtual double getPeakVel() const= 0;
    virtual double getPeakDisp() const= 0;

    virtual double getAccel(double time) const= 0;
    virtual double getVel(double time) const= 0;
    virtual double getDisp(double time) const= 0;
    virtual const  Vector &getDispVelAccel(double time) const;
    
/*     void setIntegrator(TimeSeriesIntegrator *integrator); */
/*     TimeSeries *integrate(TimeSeries *theSeries, double delta = 0.01);  */

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual double getAccelSensitivity(double time)= 0;
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param)= 0;
    virtual int updateParameter(int parameterID, Information &info)= 0;
    virtual int activateParameter(int parameterID)= 0;
    // AddingSensitivity:END ///////////////////////////////////////////
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

int sendGroundMotionPtr(GroundMotion *,DbTagData &,CommParameters &cp,const BrokedPtrCommMetaData &);
GroundMotion *receiveGroundMotionPtr(GroundMotion *,DbTagData &,const CommParameters &cp,const BrokedPtrCommMetaData &);


} // end of XC namespace

#endif

