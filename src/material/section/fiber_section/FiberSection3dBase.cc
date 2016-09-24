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
//FiberSection3dBase.cc

#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include "material/section/fiber_section/fiber/UniaxialFiber3d.h"
#include <classTags.h>
#include <material/section/fiber_section/FiberSection3dBase.h>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>


#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/d2/Semiplano2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

//! @brief Constructor.
XC::FiberSection3dBase::FiberSection3dBase(int tag, int classTag, int dim, const contenedor_fibras &fibers,XC::MaterialLoader *mat_ldr)
  : FiberSectionBase(tag,classTag, fibers.size(),dim,mat_ldr)
  {}

//! @brief Constructor.
XC::FiberSection3dBase::FiberSection3dBase(int tag, int classTag, int dim,XC::MaterialLoader *mat_ldr):
  FiberSectionBase(tag, classTag,dim,mat_ldr) {}

//! @brief constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSection3dBase::FiberSection3dBase(int classTag, int dim,XC::MaterialLoader *mat_ldr):
  FiberSectionBase(0, classTag,dim,mat_ldr) {}

//! @brief Constructor de copia.
XC::FiberSection3dBase::FiberSection3dBase(const FiberSection3dBase &otra)
  : FiberSectionBase(otra) {}

//! @brief Operador asignación.
XC::FiberSection3dBase &XC::FiberSection3dBase::operator=(const FiberSection3dBase &otro)
  {
    FiberSectionBase::operator=(otro);
    kr= otro.kr;
    return *this;
  }

double XC::FiberSection3dBase::get_strain(const double &y,const double &z) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1) + z*def(2));
  }

//! @brief Añade una fibra a la sección.
XC::Fiber *XC::FiberSection3dBase::addFiber(int tag,const MaterialLoader &ldr,const std::string &nmbMat,const double &Area, const Vector &position)
  {
    Fiber *retval= fibras.buscaFibra(tag);
    if(retval)
      std::cerr << "(FiberSection3dBase::addFiber; fiber: " << tag << " already exists." << std::endl;
    else
      {
        if(position.Size()<2)
          std::cerr << "(FiberSection2d::addFiber; el vector de posición debe ser de dimensión 2." << std::endl;
        UniaxialFiber3d f(tag,ldr,nmbMat,Area,position);
        retval= addFiber(f);
      }
    return retval;
  }

int XC::FiberSection3dBase::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // Initial declarations
    int ok= -1;

    // A material parameter
    if(argv[0] == "material")
      {
        // Get the tag of the material
        int paramMatTag= atoi(argv[1]);
        // Loop over fibers to find the right material(s)
        std::vector<std::string> argv2(argv);
        argv2.erase(argv2.begin(),argv2.begin()+2);
        ok= fibras.setParameter(paramMatTag,argv2, param);
        if(ok<0)
          {
            std::cerr << "FiberSection3dBase::setParameter() - could not set parameter. " << std::endl;
            return -1;
          }
        else
          { return ok + 100; }
      }
    else
      return -1;
  }

int XC::FiberSection3dBase::updateParameter(int parameterID, Information &info)
  {
    int ok= -1;

    switch(parameterID)
      {
      case 1:
        return -1;
      default:
        if(parameterID >= 100)
          {
            ID *paramIDPtr;
            paramIDPtr= info.theID;
            ID paramID= (*paramIDPtr);
            int paramMatrTag= paramID(1);
            ok= fibras.updateParameter(paramMatrTag,parameterID-100,info);
            if(ok<0)
              {
                std::cerr << "XC::FiberSection3dBase::updateParameter() - could not update parameter. " << std::endl;
                return ok;
              }
            else
              { return ok; }
          }
        else
          return -1;
     }
  }

//! @brief Returns strain at position being passed as parameter.
double XC::FiberSection3dBase::getStrain(const double &y,const double &z) const
  { return get_strain(-y,z); } //XXX El cambio de signo de la y se debe al lío que produce
                               //el que la coordenada este cambiada de signo
                               //ver constructor UniaxialFiber3d y otras líneas.

