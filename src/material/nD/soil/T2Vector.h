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
//<<<<<<< T2Vector.h
//<<<<<<< T2Vector.h
// $Revision: 1.7 $
// $Date: 2002/05/16 00:07:47 $
//=======
// $Revision: 1.7 $
// $Date: 2002/05/16 00:07:47 $
//>>>>>>> 1.4
//=======
// $Revision: 1.7 $
// $Date: 2002/05/16 00:07:47 $
//>>>>>>> 1.6
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/T2Vector.h,v $
                                                                        
// Written: ZHY
// Created: August 2000

//
// T2Vector.h
// --------
//

#ifndef _T2Vector_H_
#define _T2Vector_H_

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"
#include <cfloat>

#define UP_LIMIT    1.0e+30
#define LOW_LIMIT   20.*DBL_EPSILON

namespace XC {
// global function: scalar product of two second order tensor vectors
double operator &&(const Vector &, const Vector &);

// define second order tensor vector class
//! @ingroup NDMat
//
//! @brief Tensor de segundo orden.
class T2Vector: public MovableObject
  {
  private:
    Vector theT2Vector;
    Vector theDeviator;
    double theVolume;
    static Vector engrgStrain;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // constructors
    T2Vector();
    T2Vector(const Vector & T2Vector_init, int isEngrgStrain=0);
    T2Vector(const Vector & deviat_init, double volume_init);
    
    void setData(const Vector &init, int isEngrgStrain =0);
    void setData(const Vector &deviat, double volume);

    const Vector & t2Vector(int isEngrgStrain=0) const; 
    const Vector & deviator(int isEngrgStrain=0) const;
    double volume() const {return theVolume; }
    const Vector &unitT2Vector() const;
    const Vector &unitDeviator() const;
    double t2VectorLength() const;
    double deviatorLength() const;
    double octahedralShear(int isEngrgStrain=0) const;

    // = -sqrt(3/2*(S:S))/(p+residualPress)
    double deviatorRatio(double residualPress=0.) const; 

    //next function return the angle between two T2Vectors in radians (-PI to PI)
    double angleBetweenT2Vector(const T2Vector &) const; 

    //next function return the angle between deviatoric components of
    //two vectors in radians (-PI to PI)
    double angleBetweenDeviator(const T2Vector &) const; 

    int operator == (const T2Vector & a) const;
    int isZero(void) const;
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
} // fin namespace XC


#endif
