//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//python_interface.tcc

class_<XC::BeamWithHinges2d, bases<XC::BeamColumnWithSectionFDTrf2d>, boost::noncopyable >("BeamWithHinges2d", no_init)
  .add_property("rho", &XC::BeamWithHinges2d::getRho,&XC::BeamWithHinges2d::setRho)
  .add_property("sectionProperties", &XC::BeamWithHinges2d::getSectionProperties, &XC::BeamWithHinges2d::setSectionProperties)
   ;

class_<XC::BeamWithHinges3d, bases<XC::BeamColumnWithSectionFDTrf3d>, boost::noncopyable >("BeamWithHinges3d", no_init)
  .add_property("rho", &XC::BeamWithHinges3d::getRho,&XC::BeamWithHinges3d::setRho)
  .add_property("sectionProperties", &XC::BeamWithHinges3d::getSectionProperties, &XC::BeamWithHinges3d::setSectionProperties)
   ;
