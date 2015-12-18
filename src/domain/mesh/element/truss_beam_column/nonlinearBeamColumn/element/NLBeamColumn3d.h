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
                                                                        
// $Revision: 1.15 $
// $Date: 2003/02/27 17:15:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/nonlinearBeamColumn/element/NLBeamColumn3d.h,v $
                                                                        
                                                                        
// File: ~/model/element/NLBeamColumn3d.h
//
// Written: Remo Magalhaes de Souza on 03/99 
// Revised: rms 06/99 (mass matrix)
//          rms 07/99 (using setDomain)
//          rms 08/99 (included P-Delta effect)
//	    fmk 10/99 setResponse() & getResponse()
//          rms 04/00 (using CrdTransf class)
//          mhs 06/00 (using new section class)
//          mhs 06/00 (using new section class w/ variable dimensions)
//          rms 06/00 (torsional stiffness considered at the section level)
//          rms 06/00 (making copy of the sections)
//          rms 06/00 (storing section history variables at the element level)
//            
// Purpose: This file contains the class definition for NLBeamColumn3d.
// NLBeamColumn3d is a materially nonlinear flexibility based frame element.

#ifndef NLBeamColumn3d_h
#define NLBeamColumn3d_h

#include <domain/mesh/element/truss_beam_column/NLForceBeamColumn3dBase.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Response;
class GaussLobattoQuadRule1d01;

//! \ingroup ElemBarraNL
//
//! @brief Elemento barra no lineal para problemas tridimensionales.
class NLBeamColumn3d: public  NLForceBeamColumn3dBase
  {
  private:
    static GaussLobattoQuadRule1d01 quadRule;

    void getGlobalDispls(Vector &dg) const;
    void getGlobalAccels(Vector &ag) const;             
    void getForceInterpolatMatrix(double xi, Matrix &b, const ID &code);
    void getDistrLoadInterpolatMatrix(double xi, Matrix &bp, const ID &code);
    void compSectionDisplacements(std::vector<Vector> &,std::vector<Vector> &) const;

  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    NLBeamColumn3d(int tag= 0);
    NLBeamColumn3d(int tag,int numSec,const Material *theSection,const CrdTransf *coordTransf);
    NLBeamColumn3d(int tag, int nodeI, int nodeJ,  
                    int numSections,const std::vector<SeccionBarraPrismatica *> &, 
                    CrdTransf3d &coordTransf, double massDensPerUnitLength = 0.0, 
		    int maxNumIters = 10, double tolerance = 1e-12);
    Element *getCopy(void) const;

    void setDomain(Domain *theDomain);

    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
    int update(void);    
    
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    void zeroLoad(void);	
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForceIncInertia(void) const;            
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    friend std::ostream &operator<<(std::ostream &s, NLBeamColumn3d &E);        
    void Print(std::ostream &s, int flag =0);    

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
  };
} // end of XC namespace

#endif

