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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/03/11 20:42:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/elasticBeamColumn/ElasticBeam2d.h,v $
                                                                        
                                                                        
// File: ~/model/ElasticBeam2d.h
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for ElasticBeam2d.
// ElasticBeam2d is a plane frame member.

#ifndef ElasticBeam2d_h
#define ElasticBeam2d_h

#include <domain/mesh/element/truss_beam_column/ProtoBeam2d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "domain/mesh/element/fvectors/FVectorBeamColumn2d.h"
#include "domain/mesh/element/coordTransformation/CrdTransf2d.h"

namespace XC {
class Channel;
class Information;
class Response;
class CrossSectionProperties3d;

//! \ingroup ElemBarra
//
//! @brief Elemento barra para problemas bidimensionales.
class ElasticBeam2d: public ProtoBeam2d
  {
  private:
    Vector eInic; //!< Section initial deformations
    double alpha;
    double d; //!< Section depth.
    
    double rho;
    
    static Matrix K;
    static Vector P;
    
    static Matrix kb;
    mutable Vector q;
    FVectorBeamColumn2d q0;  // Fixed end forces in basic system
    FVectorBeamColumn2d p0;  // Reactions in basic system
    
    CrdTransf2d *theCoordTransf; //!< Transformación de coordenadas.

    void set_transf(const CrdTransf *trf);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    ElasticBeam2d(int tag= 0);
    ElasticBeam2d(int tag,const Material *m,const CrdTransf *trf);
    ElasticBeam2d(int tag, double A, double E, double I, 
		  int Nd1, int Nd2, CrdTransf2d &theTransf, 
		  double alpha = 0.0, double d = 0.0, double rho = 0.0);
    ElasticBeam2d(const ElasticBeam2d &otro);
    ElasticBeam2d &operator=(const ElasticBeam2d &otro);
    Element *getCopy(void) const;
    ~ElasticBeam2d(void);

    int setInitialSectionDeformation(const Vector&);
    inline const Vector &getInitialSectionDeformation(void) const
      { return eInic; }
    const Vector &getSectionDeformation(void) const;

    void setDomain(Domain *theDomain);
    
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;

    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);

    double getRho(void) const
      { return rho; }
    void setRho(const double &r)
      { rho= r; }
    inline double getDepth(void) const
      { return d; }
    void setDepth(const double &h)
      { d= h; } 
    const Vector &getInitialStrain(void) const
      { return eInic; }
    void setInitialStrain(const Vector &e)
      { eInic= e; }
    
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
    
    void Print(std::ostream &s, int flag = 0);
    inline double getV(void) //Cortante en el centro.
      { return (q(1)+q(2))/theCoordTransf->getInitialLength(); }
    inline double getV1(void) //Cortante en el extremo dorsal.
      { return  (q(1)+q(2))/theCoordTransf->getInitialLength()+p0[1]; }
    inline double getV2(void) //Cortante en el extremo frontal.
      { return -(q(1)+q(2))/theCoordTransf->getInitialLength()+p0[2]; }
    inline double getN1(void) //Axil.
      { return -q(0)+p0[0]; }
    inline double getN2(void)
      { return q(0); }
    inline double getM1(void)
      { return q(1); }
    inline double getM2(void)
      { return q(2); }


    Response *setResponse(const std::vector<std::string> &argv, Parameter &param);
    int getResponse(int responseID, Information &info);
 
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);

  };
} // end of XC namespace

#endif


