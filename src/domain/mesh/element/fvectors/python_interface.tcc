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

class_<XC::FVector, boost::noncopyable >("FVector", no_init);

typedef XC::FVectorData<3> FVectorData3;
class_<FVectorData3, bases<XC::FVector>, boost::noncopyable >("FVectorData3", no_init);

class_<XC::FVectorBeamColumn2d, bases<FVectorData3>, boost::noncopyable >("FVectorBeamColumn2d", no_init);

typedef XC::FVectorData<5> FVectorData5;
class_<FVectorData5, bases<XC::FVector>, boost::noncopyable >("FVectorData5", no_init);

class_<XC::FVectorBeamColumn3d, bases<FVectorData5>, boost::noncopyable >("FVectorBeamColumn3d", no_init);

typedef XC::FVectorData<24> FVectorData24;
class_<FVectorData24, bases<XC::FVector>, boost::noncopyable >("FVectorData24", no_init);

class_<XC::FVectorShell, bases<FVectorData24>, boost::noncopyable >("FVectorShell", no_init);
