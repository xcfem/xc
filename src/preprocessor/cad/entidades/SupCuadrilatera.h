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
//SupCuadrilatera.h

#ifndef SUPCUADRILATERA_H
#define SUPCUADRILATERA_H

#include "Face.h"
#include "preprocessor/cad/matrices/MatrizPtrPnt.h"

namespace XC {

//! \ingroup CadEnt
//!
//! @brief Cuadrilátero entre cuatro puntos.
class SupCuadrilatera: public Face
  {
  protected:
    MatrizPos3d get_posiciones(void) const;
    const Edge *get_lado_homologo(const Edge *l) const;
  public:
    SupCuadrilatera(Preprocessor *m,const size_t &ndivI= 4, const size_t &ndivJ= 4);
    virtual SetEstruct *getCopy(void) const;
    //! @brief Devuelve el número de vértices.
    inline virtual size_t NumVertices(void) const
      { return 4; }

    void setPuntos(const ID &);
    void setPuntos(const MatrizPtrPnt &pntPtrs);
    void setPuntos(const m_int &);
    void defGridPoints(const boost::python::list &);

    Vector3d getIVector(void) const;
    Vector3d getJVector(void) const;
    Vector3d getKVector(void) const;

    void SetNDiv(const Edge *l,const size_t &nd);
    virtual void ConciliaNDivIJ(void);
    virtual void SetNDivI(const size_t &ndi);
    virtual void SetNDivJ(const size_t &ndi);
    void SetElemSizeI(const double &sz);
    void SetElemSizeJ(const double &sz);
    void SetElemSizeIJ(const double &,const double &);

    bool checkNDivs(const size_t &i,const size_t &j) const;
    bool checkNDivs(void) const;
    void crea_nodos(void);
    void Malla(dir_mallado dm);
  };

} //end of XC namespace
#endif
