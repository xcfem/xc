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
// @ rkaul@stanford.edu
// @ ggd@stanford.edu

#ifndef YieldSurfaceSection2d_h
#define YieldSurfaceSection2d_h

#include "material/section/SeccionBarraPrismatica.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Channel;
class FEM_ObjectBroker;
class Information;
class YieldSurface_BC;

//! \ingroup MATSCC
//
//! @defroup MATSCCYS Secciones con superficie de fluencia.
//
//! \ingroup MATSCCYS
//
//! @brief Clase base para las secciones transversales con
//! superficie de fluencia definidas en un problema bidimensional
//! (tres esfuerzos por sección).
class YieldSurfaceSection2d: public SeccionBarraPrismatica
  {
  protected:
    virtual void getSectionStiffness(Matrix &Ks)=0;
    const   bool use_Kr_orig;
    YieldSurface_BC *ys;
    Vector eTrial; //!< section trial deformations
    Vector eInic; //!< section initial deformations.
    Vector eCommit;
    Vector s;
    Vector sCommit;
    mutable Matrix ks;
  
  private:
    //    int algo;
    bool use_Kr, split_step;
    static Vector dele;
    static Vector surfaceForce;
    static Matrix G;
    static Matrix Ktp;
  public:
    YieldSurfaceSection2d( int tag, int classtag,YieldSurface_BC *ptrys, bool use_kr=true);
    YieldSurfaceSection2d(void);    
    ~YieldSurfaceSection2d(void);
  
    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);
  
    virtual int setInitialSectionDeformation (const Vector&);
    virtual int setTrialSectionDeformation (const Vector&);
    virtual const Vector &getInitialSectionDeformation(void) const;
    virtual const Vector &getSectionDeformation(void) const;
  
    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
  
    const ResponseId &getType(void) const;
    int getOrder(void) const;
  
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  
    virtual void Print(std::ostream &s, int flag =0);
    virtual SectionForceDeformation *getCopy(void) const=0;
  };
} // end of XC namespace

#endif
