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

class_<XC::BeamIntegration, bases<EntCmd>, boost::noncopyable >("BeamIntegration", no_init);
class_<XC::LegendreBeamIntegration, bases<XC::BeamIntegration>, boost::noncopyable >("LegendreBeamIntegration", no_init);

class_<XC::LobattoBeamIntegration , bases<XC::BeamIntegration>, boost::noncopyable >("LobattoBeamIntegration", no_init);

class_<XC::NewtonCotesBeamIntegration , bases<XC::BeamIntegration>, boost::noncopyable >("NewtonCotesBeamIntegration", no_init);

class_<XC::RadauBeamIntegration , bases<XC::BeamIntegration>, boost::noncopyable >("RadauBeamIntegration", no_init);

class_<XC::UserDefinedBeamIntegration , bases<XC::BeamIntegration>, boost::noncopyable >("UserDefinedBeamIntegration", no_init);

class_<XC::UserDefinedHingeIntegration , bases<XC::BeamIntegration>, boost::noncopyable >("UserDefinedHingeIntegration", no_init);

class_<XC::UserDefinedHingeIntegration2d , bases<XC::BeamIntegration>, boost::noncopyable >("UserDefinedHingeIntegration2d", no_init);

class_<XC::UserDefinedHingeIntegration3d , bases<XC::BeamIntegration>, boost::noncopyable >("UserDefinedHingeIntegration3d", no_init);

class_<XC::PlasticLengthsBeamIntegration, bases<XC::BeamIntegration>, boost::noncopyable >("PlasticLengthsBeamIntegration", no_init);

class_<XC::DistHingeIntegration , bases<XC::PlasticLengthsBeamIntegration>, boost::noncopyable >("DistHingeIntegration", no_init);

class_<XC::HingeBeamIntegration2d, bases<XC::PlasticLengthsBeamIntegration>, boost::noncopyable >("HingeBeamIntegration2d", no_init);

class_<XC::HingeBeamIntegration3d, bases<XC::PlasticLengthsBeamIntegration>, boost::noncopyable >("HingeBeamIntegration3d", no_init);

class_<XC::HingeEndpointBeamIntegration , bases<XC::PlasticLengthsBeamIntegration>, boost::noncopyable >("HingeEndpointBeamIntegration", no_init);

class_<XC::HingeMidpointBeamIntegration, bases<XC::PlasticLengthsBeamIntegration>, boost::noncopyable >("HingeMidpointBeamIntegration", no_init);

class_<XC::HingeRadauBeamIntegration, bases<XC::PlasticLengthsBeamIntegration>, boost::noncopyable >("HingeRadauBeamIntegration", no_init);

class_<XC::HingeRadauTwoBeamIntegration, bases<XC::PlasticLengthsBeamIntegration>, boost::noncopyable >("HingeRadauTwoBeamIntegration", no_init);

class_<XC::HingeMidpointBeamIntegration2d, bases<XC::HingeBeamIntegration2d>, boost::noncopyable >("HingeMidpointBeamIntegration2d", no_init);

class_<XC::HingeMidpointBeamIntegration3d , bases<XC::HingeBeamIntegration3d>, boost::noncopyable >("HingeMidpointBeamIntegration3d", no_init);

class_<XC::HingeRadauBeamIntegration2d , bases<XC::HingeBeamIntegration2d>, boost::noncopyable >("HingeRadauBeamIntegration2d", no_init);

class_<XC::HingeRadauBeamIntegration3d , bases<XC::HingeBeamIntegration3d>, boost::noncopyable >("HingeRadauBeamIntegration3d", no_init);

class_<XC::HingeRadauTwoBeamIntegration2d, bases<XC::HingeBeamIntegration2d>, boost::noncopyable >("HingeRadauTwoBeamIntegration2d", no_init);

class_<XC::HingeRadauTwoBeamIntegration3d, bases<XC::HingeBeamIntegration3d>, boost::noncopyable >("HingeRadauTwoBeamIntegration3d", no_init);


