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
// $Date: 2003/06/10 00:36:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedBeamIntegrationBase.cpp,v $

#include "UserDefinedBeamIntegrationBase.h"

#include <utility/matrix/Vector.h>

XC::UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase(int classTag,
						       const Vector &pt,
						       const Vector &wt)
  : BeamIntegration(classTag), pts(pt), wts(wt)
  {
    const size_t sz= pt.Size();
    for(size_t i= 0; i < sz; i++)
      {
        if(pt(i) < 0.0 || pt(i) > 1.0)
          std::cerr << "UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase -- point lies outside [0,1]" << std::endl;
        if(wt(i) < 0.0 || wt(i) > 1.0)
          std::cerr << "UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase -- weight lies outside [0,1]" << std::endl;
      }
  }

XC::UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase(int classTag,
						       const Vector &pt)
  : BeamIntegration(classTag), pts(pt), wts(pt.Size())
  {
    const size_t sz= pt.Size();
    for(size_t i= 0; i < sz; i++)
      {
        if(pt(i) < 0.0 || pt(i) > 1.0)
          std::cerr << "UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase -- point lies outside [0,1]" << std::endl;
      }
  }

XC::UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase(int classTag):
  BeamIntegration(classTag)
  {}

void XC::UserDefinedBeamIntegrationBase::getSectionLocations(int numSections,double L, double *xi) const
  {
    const int nIP = pts.Size();

    int i;
    for(i = 0; i < nIP; i++)
      xi[i] = pts(i);
    for( ; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::UserDefinedBeamIntegrationBase::getSectionWeights(int numSections,double L, double *wt) const
  {
    const int nIP = wts.Size();

    int i;
    for(i = 0; i < nIP; i++)
      wt[i] = wts(i);
    for(;i<numSections;i++)
      wt[i] = 1.0;
  }

//! @brief Send object members through the channel defined in cp.
int XC::UserDefinedBeamIntegrationBase::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag()); Not tagged.
    int res= cp.sendVector(pts,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(wts,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel defined in cp.
int XC::UserDefinedBeamIntegrationBase::recvData(const CommParameters &cp)
  {
    //setTag(getDbTagDataPos(0)); Not tagged.
    int res= cp.receiveVector(pts,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(wts,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::UserDefinedBeamIntegrationBase::sendSelf(CommParameters &cp)
  { return -1; }

int XC::UserDefinedBeamIntegrationBase::recvSelf(const CommParameters &cp)
  { return -1; }

void XC::UserDefinedBeamIntegrationBase::Print(std::ostream &s, int flag)
  {
    s << " Points: " << pts;
    s << " Weights: " << wts;
  }
