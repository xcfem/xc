//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//KRSeccion.h

#ifndef KRSECCION_H
#define KRSECCION_H

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {

//! \ingroup MATSCC
//
//! @brief Stiffness matrix and resultant vector for a section.
class KRSeccion: public EntCmd
  {
    friend class DqFibras;
    double rData[4]; //Datos del vector resultante de tensiones.
    Vector *R; //Vector resultante de tensiones.
    double kData[16]; //Datos de la matriz de rigidez.
    Matrix *K; //Matriz de rigidez.

    static double value,vas1,vas2,vas1as2;//Para guardar resultados parciales.
  protected:
    void libera(void);
    void alloc(const size_t &dim);
    void copia(const KRSeccion &otra);
    inline void updateNMz(const double &f,const double &y)
      {
        rData[0]+= f; //N.
        rData[1]+= f*y; //Mz.
      }
    inline void updateNMzMy(const double &f,const double &y,const double &z)
      {
        updateNMz(f,y);
        rData[2]+= f*z; //My.
      }
    static inline void updateK2d(double k[],const double &areaFibra,const double &y,const double &tangent)
      {
        value= tangent * areaFibra;
        vas1= y*value;

        k[0]+= value; //Rigidez axil
        k[1]+= vas1;
        k[2]+= vas1 * y;
      }
    inline void updateK2d(const double &areaFibra,const double &y,const double &tangent)
      { updateK2d(kData,areaFibra,y,tangent); }
    static inline void updateK3d(double k[],const double &areaFibra,const double &y,const double &z,const double &tangent)
      {
        value= tangent * areaFibra;
        vas1= y*value;
        vas2= z*value;
        vas1as2= vas1*z;

        k[0]+= value; //Rigidez axil
        k[1]+= vas1;
        k[2]+= vas2;

        k[4]+= vas1 * y;
        k[5]+= vas1as2;

        k[8]+= vas2 * z;        
      }
    inline void updateK3d(const double &areaFibra,const double &y,const double &z,const double &tangent)
      { updateK3d(kData,areaFibra,y,z,tangent); }
    static inline void updateKGJ(double k[],const double &areaFibra,const double &y,const double &z,const double &tangent)
      {
        value= tangent * areaFibra;
        vas1= y*value;
        vas2= z*value;
        vas1as2= vas1*z;

        k[0]+= value; //(0,0)->0
        k[1]+= vas1; //(0,1)->4 y (1,0)->1
        k[2]+= vas2; //(0,2)->8 y (2,0)->2
    
        k[5]+= vas1 * y; //(1,1)->5
        k[6]+= vas1as2; //(1,2)->9 y (2,1)->6
    
        k[10]+= vas2 * z; //(2,2)->10
      }
    inline void updateKGJ(const double &areaFibra,const double &y,const double &z,const double &tangent)
      { updateKGJ(kData,areaFibra,y,z,tangent); }

  public:
    KRSeccion(const size_t &dim);
    KRSeccion(const KRSeccion &otra);
    KRSeccion &operator=(const KRSeccion &otro);
    virtual ~KRSeccion(void);

    void zero(void);
    inline size_t dim(void) const
      { return ((R) ? R->Size():0); }

    inline const Vector &Resultante(void) const
      { return *R; }
    inline Vector &Resultante(void)
      { return *R; }
    inline const Matrix &Rigidez(void) const
      { return *K; }
    inline Matrix &Rigidez(void)
      { return *K; }
  };

} // end of XC namespace

#endif
