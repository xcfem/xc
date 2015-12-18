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
// $Date: 2006/01/17 21:24:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/BeamIntegration.h,v $

#ifndef BeamIntegration_h
#define BeamIntegration_h

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/nucleo/EntCmd.h"

class ExprAlgebra;

namespace XC {
class Matrix;
class ElementalLoad;
class Information;
class FVector;
class CrdTransf;
class Vector;

//! \ingroup ElemBarraF
//
//! @defgroup BeamInteg Integración de campos en elementos barra.
//!
//! \ingroup BeamInteg
//
//! @brief Integración de campos en elementos barra.
class BeamIntegration: public MovableObject, public EntCmd
  {
  public:
    BeamIntegration(int classTag);
    inline virtual ~BeamIntegration(void) {}

    virtual void getSectionLocations(int nIP, double L, double *xi) const= 0;
    virtual void getSectionWeights(int nIP, double L, double *wt) const= 0;
    const Matrix &getIntegrPointCoords(int nIP, double L) const;
    const Matrix &getIntegrPointNaturalCoords(int nIP, double L) const;
    const Matrix &getIntegrPointLocalCoords(int nIP, double L) const;

    const Matrix &getIntegrPointNaturalCoords(int nIP,const CrdTransf &trf) const;
    const Matrix &getIntegrPointNormalizedCoords(int nIP,const CrdTransf &trf) const;
    const Matrix &getIntegrPointLocalCoords(int nIP,const CrdTransf &trf) const;
    const Matrix &getIntegrPointGlobalCoords(int nIP,const CrdTransf &trf) const;

    const Vector &evalInIntegrPoints(const ExprAlgebra &expr,int nIP,const CrdTransf &trf) const;
    double getIntegral(const ExprAlgebra &expr,int nIP,const CrdTransf &trf) const;

    virtual void addElasticDeformations(ElementalLoad *theLoad,double loadFactor,double L,FVector &v0) {return;}
    // Return 0 if there is no elastic interior, -1 otherwise
    virtual int addElasticFlexibility(double L, Matrix &fe) {return 0;}

    virtual double getTangentDriftI(double L, double LI, double q2,double q3, bool yAxis = false) {return 0.0;}
    virtual double getTangentDriftJ(double L, double LI, double q2,double q3, bool yAxis = false) {return 0.0;}

    virtual BeamIntegration *getCopy(void) const= 0;

    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);

    virtual void getLocationsDeriv(int nIP, double L, double dLdh,double *dptsdh);
    virtual void getWeightsDeriv(int nIP, double L, double dLdh,double *dwtsdh);
  // Return 0 if there is no elastic interior, -1 otherwise
    virtual int addElasticFlexDeriv(double L, Matrix &dfedh,double dLdh = 0.0) {return 0;}

    virtual void Print(std::ostream &s, int flag = 0) = 0;
    any_const_ptr GetProp(const std::string &cod) const;
  };

int sendBeamIntegrationPtr(BeamIntegration *,int posClassTag, int posDbTag,DbTagData &,CommParameters &);
BeamIntegration *receiveBeamIntegrationPtr(BeamIntegration *,int posClassTag, int posDbTag,DbTagData &,const CommParameters &);

} // end of XC namespace

#endif
