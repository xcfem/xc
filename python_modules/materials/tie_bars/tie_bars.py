# -*- coding: utf-8 -*-
''' Tie-bars as specified in supplier documentation.

See: https://www.s3i.co.uk/carbon-steel-tie-bar.php
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2021, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

from materials import steel_base

class TieBarSteel(steel_base.BasicSteel):
    '''ASTM structural steel.

    :ivar name: steel identifier.
    '''
    def __init__(self, name= None, fy= 355e6, fu= 610e6, gammaM= 1.0):
        ''' Constructor.

        :param name: steel identifier.
        :param fy: yield stress.
        :param fu: ultimate tensile strength.
        :param gammaM: partial factor (defaults to 1.0)
        '''
        super(TieBarSteel,self).__init__(200e9,0.3,fy,fu,gammaM)
        if(name):
            self.name= name
        else:
            self.name= None
            
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(TieBarSteel,self).getDict()
        name= None
        if(self.name):
            name= self.name
        retval.update({'name': name})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.name= None
        if('name' in dct):
            name= dct['name']
            if(name): self.name= name

SBS355= TieBarSteel('SBS355', fy= 355e6, fu=610e6, gammaM= 1.0)
SBS520= TieBarSteel('SBS520', fy= 520e6, fu=660e6, gammaM= 1.0)

class TieBar(object):
    ''' Tie bar.

    :ivar diameter: diameter of the bar.
    :ivar steelType: bar steel type.
    '''
    def __init__(self, diameter, stressArea, steelType):
       ''' Constructor.

       :param diameter: bar diameter.
       :param stressArea: bar area for stress calculations.
       :param steelType: bar steel type.
       :param pos3d: bar position.
       '''
       self.diameter= diameter
       self.stressArea= stressArea
       self.steelType= steelType

    def getNominalArea(self):
        ''' Return the nominal area of the bar.
        '''
        return math.pi*(self.diameter/2.0)**2
    
    def getArea(self):
        ''' Return the stress area of the bar.
        '''
        return self.stressArea
        
    def getName(self):
        return 'M'+str(self.diameter*1e3)[0:2]

    def getNominalYieldStrength(self):
        ''' Return the nominal yield strength of the bar.
        '''
        return self.getArea()*self.steelType.fy
            
    def getDesignYieldStrength(self):
        ''' Return the design yield strength of the bar according
            to section J3.6 of of AISC 360-16.
        '''
        return self.getNominalYieldStrength()/self.steelType.gammaF
    
    def getNominalTensileStrength(self):
        ''' Return the nominal strength of the bar.
        '''
        return self.getArea()*self.steelType.fu
            
    def getDesignTensileStrength(self):
        ''' Return the design tensile strength of the bar according
            to section J3.6 of of AISC 360-16.
        '''
        return self.getNominalTensileStrength()/self.steelType.gammaF
    
    def __str__(self):
        return self.getName()
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= {'diameter':self.diameter,'stressArea':self.stressArea}
        steelTypeClassName= str(self.steelType.__class__)[8:-2]
        retval.update({'steelTypeClassName':steelTypeClassName, 'steelType':self.steelType.getDict()})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: dictionary to read the values from.
        '''
        self.diameter= dct['diameter']
        self.stressArea= dct['stressArea']
        steelTypeClassName= dct['steelTypeClassName']+'()'
        self.steelType= eval(steelTypeClassName)
        self.steelType.setFromDict(dct['steelType'])

M12= TieBar(12e-3, stressArea= 84e-6, steelType= SBS355)
M16= TieBar(16e-3, stressArea= 157e-6, steelType= SBS520)
M20= TieBar(20e-3, stressArea= 245e-6, steelType= SBS520)
M24= TieBar(24e-3, stressArea= 353e-6, steelType= SBS520)
M30= TieBar(30e-3, stressArea= 561e-6, steelType= SBS520)
M36= TieBar(36e-3, stressArea= 817e-6, steelType= SBS520)
M42= TieBar(42e-3, stressArea= 1121e-6, steelType= SBS520)
M48= TieBar(48e-3, stressArea= 1473e-6, steelType= SBS520)
M56= TieBar(56e-3, stressArea= 2030e-6, steelType= SBS520)
M64= TieBar(64e-3, stressArea= 2676e-6, steelType= SBS520)
