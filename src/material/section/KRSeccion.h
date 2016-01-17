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
//KRSeccion.h
//Matriz de rigidez y vector resultante de tensiones de una sección.

#ifndef KRSECCION_H
#define KRSECCION_H

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {

//! \ingroup MATSCC
//
//! @brief Objetos encargados de guardar el valor del vector resultante
//! de tensiones y de la matriz de rigidez de una sección.
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
