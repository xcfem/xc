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
//StoFibras.cc

#include "StoFibras.h"
#include "material/section/fiber_section/fiber/Fiber.h"
#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

//! @brief Reserva memoria para los materiales (uno por fibra) y para sus
//! datos dos (yLoc,Area) para secciones en 2D (getOrder()= 2) e (yLoc,zLoc,Area) para secciones en 3D (getOrder()= 3).
void XC::StoFibras::allocFibers(int numFibras,const Fiber *muestra)
  {
    libera();
    if(numFibras)
      {
        resize(numFibras);
        if(muestra)
          for(int i= 0;i<numFibras;i++)
            (*this)[i]= muestra->getCopy();
      }
  }

void XC::StoFibras::copia_fibras(const StoFibras &otro)
  {
    libera();
    const size_t numFibers= otro.getNumFibers();
    if(numFibers)
      {
        allocFibers(numFibers);
        for(register size_t i= 0;i<numFibers;i++)
          (*this)[i]= otro[i]->getCopy();
      }
  }

void XC::StoFibras::libera(void)
  {
    const size_t numFibers= getNumFibers();
    for(register size_t i= 0;i<numFibers;i++)
      if((*this)[i])
        {
          delete (*this)[i];
          (*this)[i]= nullptr;
        }
    resize(0);
  }

//! @brief Constructor por defecto.
XC::StoFibras::StoFibras(const size_t &num)
  : DqFibras(num) {}

//! @brief Constructor de copia.
XC::StoFibras::StoFibras(const StoFibras &otro)
  : DqFibras()
  { copia_fibras(otro); }

//! @brief Operador asignación.
XC::StoFibras &XC::StoFibras::operator=(const StoFibras &otro)
  {
    DqFibras::operator=(otro);
    copia_fibras(otro);
    return *this;
  }

//! @brief Copia las fibras del contenedor que se pasa como parámetro.
void XC::StoFibras::copia_fibras(const contenedor_fibras &fibers)
  {
    const size_t numFibers= fibers.size();
    allocFibers(numFibers);
    int i= 0;
    for(contenedor_fibras::const_iterator ifib=fibers.begin();ifib!=fibers.end(); ifib++,i++)
      {
        (*this)[i]= (*ifib)->getCopy();
        if(!(*this)[i])
          {
            std::cerr << "XC::StoFibras::copia_fibras -- failed to get copy of a XC::Fiber\n";
            exit(-1);
          }
      }
  }

void XC::StoFibras::setup(FiberSection2d &Section2d,const contenedor_fibras &fibers,KRSeccion &kr2)
  {
    if(!fibers.empty())
      {
        copia_fibras(fibers);
        updateKRCDG(Section2d,kr2);
      }
  }

void XC::StoFibras::setup(FiberSection3d &Section3d,const contenedor_fibras &fibers,KRSeccion &kr3)
  {
    if(!fibers.empty())
      {
        copia_fibras(fibers);
        updateKRCDG(Section3d,kr3);
      }
  }

void XC::StoFibras::setup(FiberSectionGJ &SectionGJ,const contenedor_fibras &fibers,KRSeccion &krGJ)
  {
    if(!fibers.empty())
      {
        copia_fibras(fibers);
        updateKRCDG(SectionGJ,krGJ);
      }
  }

//! @brief Destructor:
XC::StoFibras::~StoFibras(void)
  { libera(); }
