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
//VectorSeccionesBarraPrismatica.h

#ifndef VectorSeccionesBarraPrismatica_h
#define VectorSeccionesBarraPrismatica_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include <vector>

namespace XC {

class SeccionBarraPrismatica;
class Vector;
class Matrix;
class Material;
class BeamStrainLoad;

//! \ingroup MATSCC
//
//! @brief Vector de punteros a SeccionBarraPrismaticaes.
//! se emplea en los elementos para guardar las secciones
//! en los puntos de integración.
class VectorSeccionesBarraPrismatica: public EntCmd, public std::vector<SeccionBarraPrismatica *>, public MovableObject
  {
  protected:
    void borra_secciones(void);
    void clearAll(void);
    VectorSeccionesBarraPrismatica &operator=(const VectorSeccionesBarraPrismatica &);
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);

  public:
    typedef std::vector<SeccionBarraPrismatica *> mat_vector;
    typedef mat_vector::iterator iterator;
    typedef mat_vector::reference reference;
    typedef mat_vector::const_reference const_reference;

    VectorSeccionesBarraPrismatica(const size_t &sz);
    VectorSeccionesBarraPrismatica(const size_t &sz,const Material *m);
    VectorSeccionesBarraPrismatica(const size_t &sz,const SeccionBarraPrismatica *matModel);
    VectorSeccionesBarraPrismatica(const VectorSeccionesBarraPrismatica &);
    ~VectorSeccionesBarraPrismatica(void);

    void for_each(const std::string &);

    bool isTorsion(void) const;

    void setSection(const SeccionBarraPrismatica *nueva_secc);
    void setSectionCopy(size_t i,SeccionBarraPrismatica *nueva_secc);
    void setupSection(const Material *sec);
    bool setSections(const std::vector<SeccionBarraPrismatica *> &sectionPtrs);
    void setInitialSectionDeformations(const std::vector<Vector> &vs);
    void addInitialSectionDeformations(const BeamStrainLoad &,const double &,const Matrix &, const double &L);
    void setTrialSectionDeformations(const std::vector<Vector> &vs);

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

  };

} // end of XC namespace


#endif
