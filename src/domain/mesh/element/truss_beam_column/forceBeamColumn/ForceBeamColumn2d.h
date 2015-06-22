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
// $Date: 2003/10/06 18:37:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/ForceBeamColumn2d.h,v $

#ifndef ForceBeamColumn2d_h
#define ForceBeamColumn2d_h

#include <domain/mesh/element/truss_beam_column/NLForceBeamColumn2dBase.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Response;
class ElementalLoad;
class BeamIntegration;
class SeccionBarraPrismatica;

//! \ingroup ElemBarra
//
//! @defgroup ElemBarraF Elementos barra no lineales F.
//!
//! \ingroup ElemBarraF
//
//! @brief Elemento barra no lineal con material de tipo SeccionBarraPrismatica para problemas bidimensionales.
class ForceBeamColumn2d: public NLForceBeamColumn2dBase
  {
  private:
    // internal data
    BeamIntegration *beamIntegr;
    FVectorBeamColumn2d v0; // Initial deformations due to element loads

    // following are added for subdivision of displacement increment
    int maxSubdivisions;       // maximum number of subdivisons of dv for local iterations

    void libera(void);
    void alloc(const BeamIntegration &);
  
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ForceBeamColumn2d(int tag= 0);
    ForceBeamColumn2d(const ForceBeamColumn2d &);
    ForceBeamColumn2d(int tag,int numSec,const Material *theSection,const CrdTransf *trf,const BeamIntegration *integ);
    ForceBeamColumn2d(int tag, int nodeI, int nodeJ, 
	              int numSections,const std::vector<SeccionBarraPrismatica *> &,
		      BeamIntegration &beamIntegr,
		      CrdTransf2d &coordTransf, double rho = 0.0, 
		      int maxNumIters = 10, double tolerance = 1.0e-12);
    ForceBeamColumn2d &operator=(const ForceBeamColumn2d &);
    Element *getCopy(void) const;
    virtual ~ForceBeamColumn2d(void);
  
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
  
    friend std::ostream &operator<<(std::ostream &s, ForceBeamColumn2d &E);        
    void Print(std::ostream &s, int flag =0);    
  
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);

    any_const_ptr GetProp(const std::string &cod) const;
  
  protected:
    void setSectionPointers(const std::vector<SeccionBarraPrismatica *> &);
    int getInitialFlexibility(Matrix &fe) const;
  
  private:
    void getForceInterpolatMatrix(double xi, Matrix &b, const ID &code);
    void getDistrLoadInterpolatMatrix(double xi, Matrix &bp, const ID &code);
    void compSectionDisplacements(std::vector<Vector> &,std::vector<Vector> &) const;
  };
} // fin namespace XC

#endif
