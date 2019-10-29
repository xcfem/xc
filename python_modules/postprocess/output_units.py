# -*- coding: utf-8 -*-

from __future__ import print_function
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

from miscUtils import LogMessages as lmsg

class UnitDefinitionPair(object):
    ''' Text symbol describing the unit (kN,lb,N,m,mm,in, etc) and
        scale factor to apply to the results in order to obtain 
        the desired output units (i.e.: N->kN scaleUnitsForce: 1e-3).

        :ivar symbol: symbol of the unit 
        :ivar scaleFactor: scale factor to apply to the results
                           in order to obtain the desired output units
                           (i.e.: N->kN scaleUnitsForce: 1e-3).
    '''
    def __init__(self, symbol, scaleFactor, quantity):
        ''' Constructor.

            :param symbol: symbol of the unit 
            :param scaleFactor: scale factor to apply to the results
                               in order to obtain the desired output units
                               (i.e.: N->kN scaleUnitsForce: 1e-3).
            :param quantity: measured magnitude (force, length
        '''
        self.symbol= symbol
        self.scaleFactor= scaleFactor
        self.quantity= quantity
        
    def getDecoratedSymbol(self):
        ''' Return the symbol between brackets.'''
        return '['+self.symbol+']'

kilonewton= UnitDefinitionPair('kN',1e-3, 'force')
kilonewton_meter= UnitDefinitionPair('kN/m',1e-3, 'force/length')
meter= UnitDefinitionPair('m',1.0, 'length')
millimeter= UnitDefinitionPair('mm',1e3, 'length')
milliradian= UnitDefinitionPair('x1E3 rad',1e3, 'plane angle')

class DynamicsUnits(object):
    ''' Units to use for the output of forces

        :ivar forceUnit: unit to express forces.
        :ivar lengthUnit: unit to express lengths.
    '''
    def __init__(self, forceUnit= kilonewton, lengthUnit= meter):
        ''' Constructor.

            :param forceUnit: unit to express forces.
            :param lengthUnit: unit to express lengths.
        '''
        self.forceUnit= forceUnit
        self.lengthUnit= lengthUnit
        
    def getDecoratedUnitsText(self):
        return 'units:['+self.lengthUnit.symbol+','+self.forceUnit.symbol+']'
        
defaultDynamicUnits= DynamicsUnits(kilonewton,meter)

class DisplacementUnits(object):
    ''' Units to use for the output of displacement magnitudes

        :ivar lengthUnit: unit to express lengths.
        :ivar planeAngleUnit: unit to express angles.
    '''
    def __init__(self, lengthUnit= millimeter, planeAngleUnit= milliradian):
        ''' Constructor.

            :param lengthUnit: unit to express lengths.
            :param planeAngleUnit: unit to express forces.
        '''
        self.lengthUnit= lengthUnit
        self.planeAngleUnit= planeAngleUnit
        
    def getDecoratedRotSymbol(self):
        return self.planeAngleUnit.getDecoratedSymbol()
    
    def getDecoratedDisplSymbol(self):
        return self.lengthUnit.getDecoratedSymbol()

defaultDisplacementUnits= DisplacementUnits(millimeter,milliradian)

class OutputUnits(object):
    ''' Units to use in output stuff (graphics, etc.)

        :ivar displacementUnits: units for the displacements
        :ivar dynamicUnits: units for the forces and moments
    '''
    def __init__(self,displacementUnits= defaultDisplacementUnits, dynamicUnits= defaultDynamicUnits):

        self.displacementUnits= displacementUnits
        self.dynamicUnits= dynamicUnits
        #self.textUnitsRot='['+textUnitsRot+']'
        #self.textUnitsDispl='['+textUnitsDispl+']'
        #self.textUnitsLoadsIntForces='units:['+textUnitsLengthMoment+','+textUnitsForce+']'
        
    def getDisplacementUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.displacementUnits.lengthUnit.scaleFactor
    
    def getDisplacementUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.displacementUnits.lengthUnit.getDecoratedSymbol()
    
    def getRotationUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.displacementUnits.planeAngleUnit.scaleFactor
    
    def getRotationUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.displacementUnits.planeAngleUnit.getDecoratedSymbol()
    
    def getForceUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.dynamicUnits.forceUnit.scaleFactor
    
    def getForceUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.dynamicUnits.getDecoratedUnitsText()
