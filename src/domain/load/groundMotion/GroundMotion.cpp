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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.9 $
// $Date: 2004/07/07 00:51:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/groundMotion/GroundMotion.cpp,v $
                                                                        
// Written: fmk 
// Created: 05/98
// Revision: A


#include "GroundMotion.h"
#include <domain/load/pattern/TimeSeriesIntegrator.h>
#include <domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.h>

#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/matrix/ID.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"


XC::GroundMotion::GroundMotion(int theClassTag)
 :MovableObject(theClassTag), data(3) {}

// void XC::GroundMotion::setIntegrator(TimeSeriesIntegrator *integrator)
//   {
//     if(theIntegrator)
//       delete theIntegrator;
//     theIntegrator = integrator;
//   }

// XC::TimeSeries *XC::GroundMotion::integrate(TimeSeries *theSeries, double delta)
//   {
//   // check that an integrator & accel series exist
//     if(!theIntegrator)
//       {
//         std::cerr << "WARNING:XC::GroundMotion::integrate() - no XC::TimeSeriesIntegrator provided - will use Trapezoidal. \n";
//         theIntegrator = new XC::TrapezoidalTimeSeriesIntegrator();
//         if(theIntegrator == 0)
//           {
//             std::cerr << "WARNING:XC::GroundMotion::integrate() - no XC::TimeSeriesIntegrator provided - failed to create a Trapezoidal .. memory problems! \n";
//             return 0;
//           }
//       }
//     if(!theSeries)
//       {
//         std::cerr << "XC::GroundMotion::integrate - no XC::TimeSeries specified\n";
//         return 0;
//       }

//     // integrate the series, if no vel series exists set it to new one
//     TimeSeries *theNewSeries = theIntegrator->integrate(theSeries, delta);

//     if(!theNewSeries)
//       {
//         std::cerr << "XC::GroundMotion::integrate - no XC::TimeSeriesIntegrator failed to integrate\n";
//         return 0;
//       }
//     return theNewSeries;
//   }


const XC::Vector &XC::GroundMotion::getDispVelAccel(double time) const
  {
    if(time < 0.0)
      {
        data(0) = 0.0;
        data(1) = 0.0;
        data(2) = 0.0;
        return data;
      }
    else
      {
        data(2) = this->getAccel(time);
        data(1) = this->getVel(time);
        data(0) = this->getDisp(time);
      }
    return data;
  }


int  XC::GroundMotion::sendSelf(CommParameters &cp)
  {
    std::cerr << "XC::GroundMotion::sendSelf no implementada." << std::endl;


  return 0;
}


int XC::GroundMotion::recvSelf(const CommParameters &cp)
  {
    std::cerr << "XC::GroundMotion::recvSelf no implementada." << std::endl;


    return 0;
  }


//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
any_const_ptr XC::GroundMotion::GetProp(const std::string &cod) const
  {
    if(cod == "getDuration")
      {
        tmp_gp_dbl= getDuration();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod == "getPeakAccel")
      {
        tmp_gp_dbl= getPeakAccel();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod == "getPeakVel")
      {
        tmp_gp_dbl= getPeakVel();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod == "getPeakDisp")
      {
        tmp_gp_dbl= getPeakDisp();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod == "getAccel")
      {
        const double time= popDouble(cod);
        tmp_gp_dbl= getAccel(time);
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod == "getVel")
      {
        const double time= popDouble(cod);
        tmp_gp_dbl= getVel(time);
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod == "getDisp")
      {
        const double time= popDouble(cod);
        tmp_gp_dbl= getDisp(time);
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return EntCmd::GetProp(cod);
  }

//! @brief Envía un puntero al «ground motion» a través del canal que se pasa como parámetro.
//! @param posClassTag: Posición de ID del identificador de la clase del objeto.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
int XC::sendGroundMotionPtr(GroundMotion *ptr,DbTagData &dt,CommParameters &cp,const BrokedPtrCommMetaData &md)
  {
    int res= 0;
    if(ptr)
      {
        dt.setDbTagDataPos(md.getPosClassTag(),ptr->getClassTag());
        res= cp.sendMovable(*ptr,dt,md);
      }
    if(res < 0)
      std::cerr <<"WARNING sendGroundMotionPtr - "
                << " failed to send ground motion.\n";
    return res;
  }

//! @brief Recibe un puntero a «ground motion» a través del canal que se pasa como parámetro.
//! @param posClassTag: Posición de ID del identificador de la clase del objeto.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::GroundMotion *XC::receiveGroundMotionPtr(GroundMotion* ptr,DbTagData &dt,const CommParameters &cp,const BrokedPtrCommMetaData &md)
  {
    GroundMotion *retval= nullptr;
    const int matClass= dt.getDbTagDataPos(md.getPosClassTag());
    if(ptr && (ptr->getClassTag() == matClass))
      retval= ptr;
    else 
      {

        // check if we have a ground motion object already &
        // if we do if of right type
        if(ptr) // if old one .. delete it
          delete ptr;
        ptr= nullptr;
        retval= cp.getBrokedGroundMotion(ptr,dt,md);
      }
    if(retval)
      {
        int res= cp.receiveMovable(*retval,dt,CommMetaData(md.getPosDbTag()));
        if(res<0)
          std::cerr <<"WARNING - receiveGroundMotionPtr "
                    << "failed to receive ground motion.\n";
      }
    else
      std::cerr <<"WARNING  - receiveGroundMotionPtr "
                << " failed to get a blank ground motion of type "
                << matClass << std::endl; 
    return retval;
  }
