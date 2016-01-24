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

#include "UserDefinedHingeIntegrationBase.h"

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/Information.h>

XC::UserDefinedHingeIntegrationBase::UserDefinedHingeIntegrationBase(int classTag, const Vector &ptL, const Vector &wtL, const Vector &ptR, const Vector &wtR)
  : UserDefinedBeamIntegrationBase(classTag,ptL,wtL), ptsR(ptR), wtsR(wtR)
  {
    const size_t sz= ptR.Size();
    for(size_t i= 0; i< sz; i++)
      {
        if(ptR(i) < 0.0 || ptR(i) > 1.0)
          std::cerr << "UserDefinedHingeIntegrationBase::UserDefinedHingeIntegrationBase -- point lies outside [0,1]" << std::endl;
        if(wtR(i) < 0.0 || wtR(i) > 1.0)
          std::cerr << "UserDefinedHingeIntegrationBase::UserDefinedHingeIntegrationBase -- weight lies outside [0,1]" << std::endl;
      }
  }

XC::UserDefinedHingeIntegrationBase::UserDefinedHingeIntegrationBase(int classTag)
  : UserDefinedBeamIntegrationBase(classTag)
  {}

void XC::UserDefinedHingeIntegrationBase::getSectionLocations(int numSections,double L, double *xi) const
  {
    const int npL = ptsL().Size();
    const int npR = ptsR.Size();

    double lpI = 0.0;
    double lpJ = 0.0;
    int i, j;
    for(i = 0; i < npL; i++)
      {
        xi[i] = ptsL()(i);
        lpI += wtsL()(i);
      }
    for(j = 0; j < npR; j++, i++)
     {
       xi[i] = ptsR(j);
       lpJ += wtsR(j);
     }

    const double alpha = 0.5-0.5*(lpI+lpJ);
    const double beta  = 0.5+0.5*(lpI-lpJ);
    xi[i++] = alpha*(-1/sqrt(3.0)) + beta;
    xi[i++] = alpha*(1/sqrt(3.0)) + beta;

    for( ; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::UserDefinedHingeIntegrationBase::getSectionWeights(int numSections,double L, double *wt) const
  {
    int npL = wtsL().Size();
    int npR = wtsR.Size();

    double lpI = 0.0;
    double lpJ = 0.0;
    int i, j;
    for(i = 0; i < npL; i++)
      {
        wt[i] = wtsL()(i);
        lpI += wtsL()(i);
      }
    for(j = 0; j < npR; j++, i++)
      {
        wt[i] = wtsR(j);
        lpJ += wtsR(j);
      }

    //double oneOverL = 1.0/L;
    wt[i++] = 0.5-0.5*(lpI+lpJ);
    wt[i++] = 0.5-0.5*(lpI+lpJ);
    for(;i<numSections;i++)
      wt[i] = 1.0;
  }

//! @brief Send object members through the channel defined in cp.
int XC::UserDefinedHingeIntegrationBase::sendData(CommParameters &cp)
  {
    int res= UserDefinedBeamIntegrationBase::sendData(cp);
    res+= cp.sendVector(ptsR,getDbTagData(),CommMetaData(3));
    res+= cp.sendVector(wtsR,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives object members through the channel defined in cp.
int XC::UserDefinedHingeIntegrationBase::recvData(const CommParameters &cp)
  {
    int res= UserDefinedBeamIntegrationBase::recvData(cp);
    res+= cp.receiveVector(ptsR,getDbTagData(),CommMetaData(3));
    res+= cp.receiveVector(wtsR,getDbTagData(),CommMetaData(4));
    return res;
  }

void XC::UserDefinedHingeIntegrationBase::Print(std::ostream &s, int flag)
  {
    s << "UserHinge" << std::endl;
    s << " Points left hinge I: " << ptsL();
    s << " Weights left hinge I: " << wtsL();
    s << " Points right hinge J: " << ptsR;
    s << " Weights right hinge J: " << wtsR;
  }
