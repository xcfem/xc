# -*- coding: utf-8 -*-
''' Structural steel as specified in EAE standard.
    Instrucción de Acero Estructural (EAE) Real Decreto 751/2011, de 27 de mayo,.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import scipy.interpolate
from materials import steel_base
from misc_utils import log_messages as lmsg
from connections import bolts
from connections import square_plate_washer as swp
from connections import bolted_plate as bp
from materials.ec3 import EC3_materials

class EAESteel(EC3_materials.EC3Steel):
    def __init__(self, fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM, gammaM1= 1.0, gammaM2= 1.1):
        ''' Constructor.
          :param fy16: 0<t<16mm
          :param fy40: 16<t<40mm
          :param fy63: 40<t<63mm
          :param fy80: 63<t<80mm
          :param fy100: 80<t<100mm
          :param fy125: 80<t<125mm
          :param gammaM1: Partial factor for buckling resistance.
          :param gammaM2: Partial factor for cross-sections in tension to fracture.
        '''

        super(EAESteel,self).__init__(fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM, gammaM1, gammaM2)

    
# European norm EN 10025-2:2004
S235JR= EAESteel(fy= 235e6, fy16= 235e6, fy40= 225e6, fy63= 215e6, fy80= 215e6, fy100= 215e6, fy125= 195e6,fu= 360e6,gammaM= 1.1)

S275JR= EAESteel(fy= 275e6, fy16= 275e6, fy40= 265e6, fy63= 255e6, fy80= 245e6, fy100= 235e6, fy125= 225e6,fu= 360e6,gammaM= 1.1)

S355JR= EAESteel(fy= 355e6, fy16= 355e6, fy40= 345e6, fy63= 335e6, fy80= 325e6, fy100= 315e6, fy125= 295e6, fu= 360e6, gammaM= 1.1)

S450J0= EAESteel(fy= 450e6, fy16= 450e6, fy40= 430e6, fy63= 410e6, fy80= 390e6, fy100= 380e6, fy125= 380e6, fu= 360e6, gammaM= 1.1)

class ISO898Steel(steel_base.BasicSteel):
    '''Mechanical properties of steel for metric fasteners.

    :ivar name: steel identifier.
    :ivar fy: yield stress (defaults to 240e6 Pa)
    :ivar fu: ultimate tensile strength (defaults to 400e6 Pa)
    :ivar gammaM: partial factor (defaults to 1.0)
    '''
    def __init__(self, name= None, fy= 250e6, fu= 400e6, gammaM= 1.25):
        ''' Constructor.

        :param name: steel identifier.
        :param fy: yield stress (defaults to 250e6 Pa)
        :param fu: ultimate tensile strength (defaults to 400e6 Pa)
        :param gammaM: partial factor (defaults to 1.25)
        '''
        super(ISO898Steel,self).__init__(200e9,0.3,fy,fu,gammaM)
        if(name):
            self.name= name
        else:
            self.name= None

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(ISO898Steel,self).getDict()
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

iso898_4_6= ISO898Steel(name= '4.6', fy= 240e6, fu= 400e6)
iso898_5_6= ISO898Steel(name= '5.6', fy= 300e6, fu= 500e6)
iso898_6_8= ISO898Steel(name= '6.8', fy= 480e6, fu= 600e6)
iso898_8_8= ISO898Steel(name= '8.8', fy= 640e6, fu= 800e6)
iso898_10_9= ISO898Steel(name= '10.9', fy= 900, fu= 1000e6)

class BoltFastener(bolts.BoltBase):
    ''' EAE bolt fasteners.

    :ivar name: bolt identifier
    :ivar steel: steel material.
    '''
    # See table table 58.7 of EAE-11.
    diams= [12e-3, 16e-3, 20e-3, 22e-3, 24e-3, 27e-3]
    tab57_7_As= [84.3e-6, 157e-6, 245e-6, 303e-6, 353e-6, 456e-6]
    fTab57_7_As= scipy.interpolate.interp1d(diams,tab57_7_As)
    
    def __init__(self, name, steel, diameter, pos3d= None):
       ''' Constructor.

       :param name: bolt identifier
       :param steel: steel material.
       :param diameter: bolt diameter.
       :param pos3d: bolt position.
       '''
       super(BoltFastener,self).__init__(diameter, pos3d)
       self.name= name
       self.steelType= steel
       
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(BoltFastener, self).getDict()
        steelTypeClassName= str(self.steelType.__class__)[8:-2]
        retval.update({'name':self.name, 'steelTypeClassName':steelTypeClassName, 'steelType':self.steelType.getDict()})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super(BoltFastener, self).setFromDict(dct)
        self.name= dct['name']
        steelTypeClassName= dct['steelTypeClassName']+'()'
        self.steelType= eval(steelTypeClassName)
        self.steelType.setFromDict(dct['steelType'])

    def getAs(self):
        ''' Return the effective area of the bolt for tension strength
            according to table 58.7 of EAE-11.'''
        return float(self.fTab57_7_As(self.diameter))
        
    def getDesignShearStrength(self, numberOfShearPlanes= 1, threadsExcluded= False):
        ''' Return the design shear strength of the bolt according
            to clause 58.6 of EAE-11.

        :param numberOfShearPlanes: number of shear planes.
        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        retval= self.steelType.fu/self.steelType.gammaM*numberOfShearPlanes
        if(threadsExcluded):
            retval*= 0.6*self.getArea()
        else:
            As= self.getAs()
            if(self.steelType.name in ['4.6', '5.6', '8.8']):
                retval*= 0.6*As
            elif(self.steelType.name in ['6.8', '10.9']):
                retval*= 0.5*As
            else:
                lmsg.error('steel type: '+ self.steelType.name + ' unknown.')
        return retval
    
    def getDesignTensileStrength(self, dieStock= False):
        ''' Return the design tensile strength of the bolt according
            to clause 58.7 of EAE-11.

        :param dieStock: threads created with a die stock or a similar tool.
        '''
        retval= 0.9*self.steelType.fu*self.getAs()/self.steelType.gammaM
        if(dieStock):
            retval*= 0.85
        return retval

    def getShearTensileEfficiency(self, Fv, Ft, numberOfShearPlanes= 1, threadsExcluded= False, dieStock= False):
        ''' Return the efficiency of the bolt under the load arguments

        :param Fv: shear force acting on the bolt.
        :param Ft: tensile force acting on the bolt. 
        :param numberOfShearPlanes: number of shear planes.
        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        :param dieStock: threads created with a die stock or a similar tool.
        '''
        return Fv/self.getDesignShearStrength(numberOfShearPlanes,threadsExcluded)+Ft/(1.4*self.getDesignTensileStrength(dieStock))
