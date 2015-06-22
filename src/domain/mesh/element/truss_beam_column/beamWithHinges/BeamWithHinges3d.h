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
                                                                        
// $Revision: 1.11 $
// $Date: 2003/02/25 23:32:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beamWithHinges/BeamWithHinges3d.h,v $

#ifndef BeamWithHinges3d_h
#define BeamWithHinges3d_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf3d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "material/section/repres/ConstantesSecc3d.h"
#include "domain/mesh/element/fvectors/FVectorBeamColumn3d.h"

namespace XC {
class Node;
class Channel;
class FEM_ObjectBroker;

class SeccionBarraPrismatica;

class Response;
class Renderer;

//! \ingroup ElemBarra
//
//! @brief Barra con rótulas en los extremos para problemas tridimensionales.
class BeamWithHinges3d: public BeamColumnWithSectionFDTrf3d
  {
  private:
    ConstantesSecc3d ctes_scc; //Características de la seccion E,A,Iy,...
    double beta1, beta2;
    double rho;

    Matrix fs[2];
    Vector sr[2];
    Vector  e[2];
    
    Matrix kb;
    Vector q;
    
    Matrix kbCommit;
    Vector qCommit;
    Vector eCommit[2];  

    int initialFlag;
    
    int maxIter;
    double tolerance;

    Matrix *applied_sf;  // Applied section forces due to element loads, 5 x nSections
    FVectorBeamColumn3d p0; // Reactions in the basic system due to element loads
    FVectorBeamColumn3d v0; // Basic deformations due to element loads on the interior
  
    static Matrix theMatrix;
    static Vector theVector;
    static double workArea[];

    void checkNodePtrs(Domain *theDomain);
    void setHinges(void);
    
    void getForceInterpMatrix(Matrix &b, double x, const ID &c);
    void getDistrLoadInterpMatrix(Matrix &bp, double x, const ID &c);
    

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);  
 public:
    BeamWithHinges3d(int tag, int nodeI, int nodeJ,
		   double E, double A, double Iz, double Iy, double G, double J,
		   SeccionBarraPrismatica &sectionRefI, double hingeIlen, 
		   SeccionBarraPrismatica &sectionRefJ, double hingeJlen,
		   CrdTransf3d &coordTrans, double massDensPerUnitLength = 0.0,
		   int max = 1, double tol = 1.0e-16);
    BeamWithHinges3d(int tag= 0);
    BeamWithHinges3d(int tag,const Material *theSection,const CrdTransf *coordTransf);
    BeamWithHinges3d(const BeamWithHinges3d &);
    //BeamWithHinges3d &operator=(const BeamWithHinges3d &);
    Element *getCopy(void) const;
    ~BeamWithHinges3d(void);
    
    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);
    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    int update(void);
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;
    
    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    Response *setResponse(const std::vector<std::string> &argv, Information &);
    int getResponse(int responseID, Information &info);
  
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    
    void Print(std::ostream &s, int flag = 0);
  };
} // fin namespace XC

#endif
