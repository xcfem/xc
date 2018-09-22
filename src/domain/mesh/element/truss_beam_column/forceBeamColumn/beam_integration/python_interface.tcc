//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC is free software: you can redistribute it and/or modify 
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

class_<XC::BeamIntegration, bases<CommandEntity>, boost::noncopyable >("BeamIntegration", no_init);
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


