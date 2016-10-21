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
//BeamUniformLoad.cpp

#include "BeamUniformLoad.h"
#include "domain/mesh/element/Element1D.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "xc_utils/src/geom/pos_vec/VDesliz3d.h"
#include "domain/mesh/element/coordTransformation/CrdTransf.h"

//! @brief Constructor.
XC::BeamUniformLoad::BeamUniformLoad(int tag,int classTag, double wt, double wa,const XC::ID &theElementTags)
  :BeamMecLoad(tag, classTag,wt,wa,theElementTags) {}

//! @brief Constructor.
XC::BeamUniformLoad::BeamUniformLoad(int tag,int classTag)
  :BeamMecLoad(tag, classTag) {}

//! @brief Constructor.
XC::BeamUniformLoad::BeamUniformLoad(int classTag)
  :BeamMecLoad(0,classTag) {}

std::string XC::BeamUniformLoad::Categoria(void) const
  { return "uniforme"; }

//! brief Returns load resultant (force and moment integration over the elements).
SVD3d XC::BeamUniformLoad::getResultant(const Pos3d &centro, bool initialGeometry) const
  {
    SVD3d retval(centro);
    Matrix uniformLoads= getDistributedGlobalForces();
    const Domain *ptrDom= getDomain();
    if(ptrDom)
      {
        const size_t sz= uniformLoads.noRows();
        for(size_t i=0; i<sz; i++)
          {
            const size_t elemTag= getElementTags()(i);
            const Element1D *ptrElem= dynamic_cast<const Element1D *>(ptrDom->getElement(elemTag));
            if(ptrElem)
              {
                const CrdTransf *ptrTransf= ptrElem->getCoordTransf();
                if(ptrTransf)
		  {
                    const double l= ptrTransf->getLength(initialGeometry);
                    const Vector3d force(l*uniformLoads(i,0),l*uniformLoads(i,1),l*uniformLoads(i,2));
		    retval+= VDesliz3d(ptrElem->getPosCdg(),force);
		  }
                else
		  std::cerr << nombre_clase() << "::getResultant; el elemento: "
                            << elemTag << " no tiene transformación de coordenadas." << std::endl;

              }
            else
	      std::cerr << nombre_clase() << "::getResultant; the element: "
                        << elemTag << " is not a 1D element." << std::endl;
          }
      }
    else
      std::cerr << nombre_clase() << "::getResultant; no existe apuntador al dominio." << std::endl;
    return retval;
  }

