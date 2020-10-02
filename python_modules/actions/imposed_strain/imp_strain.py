# -*- coding: utf-8 -*-
from __future__ import division

#Base classes to define data for calculating imposed strain load state cases

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

class basicData(object):
    '''Basic parameters 

    :ivar elemSet: set of shell elements to which apply the strain
    :ivar DOF: degree of freedom (0,1 or 2) in which strain acts
    '''
    def __init__(self,elemSet,DOF):
        self.elemSet=elemSet
        self.DOF=DOF

class unifStrain(basicData):
    '''Data to define uniform strain over a set of elements

    :ivar elemSet: set of shell elements to which apply the strain
    :ivar DOF: degree of freedom (0,1 or 2) in which strain acts
    :ivar strain: strain
    '''
    def __init__(self,elemSet,DOF,strain):
        super(unifStrain,self).__init__(elemSet,DOF)
        self.strain=strain

class unifThermalStrain(basicData):
    '''Data to define uniform thermal strain over a set of elements.

    :ivar elemSet: set of shell elements to which apply the strain
    :ivar DOF: degree of freedom (0,1 or 2) in which strain acts
    :ivar alpha: Thermal expansion coefficient of material (1/ºC)
    :ivar temp: increase (+) or decrease (-) of temperature in ºC
    '''
    def __init__(self,elemSet,DOF,alpha,temp):
        super(unifThermalStrain,self).__init__(elemSet,DOF)
        self.alpha=alpha
        self.temp=temp
        self.strain=self.alpha*self.temp

class gradThermalStrain(basicData):
    '''Data to define gradient thermal strain over a set of elements.

    :ivar elemSet: set of shell elements to which apply the strain
    :ivar DOF: degree of freedom (3,4 or 5) involving the curvature due to
               gradient of temeperature.
    :ivar alpha: Thermal expansion coefficient of material (1/ºC)
    :ivar Ttop: Temperature (ºC) at the top face of elements
    :ivar Tbottom: Temperature (ºC) at the bottom face of elements
    :ivar curvature: Curvature induced by the thermal gradient.
    '''
    def __init__(self,elemSet,elThick,DOF,alpha,Ttop,Tbottom):
        ''' Constructor.
        Curvature is positive when the center of curvature
        lies along the direction of the normal to the element
        so when the temperature at the bottom face is
         greater than the temperature at the top face.

        :param elemSet: set of shell elements to which apply the strain
        :param DOF: degree of freedom (3,4 or 5) involving the curvature due to
                   gradient of temeperature.
        :param alpha: Thermal expansion coefficient of material (1/ºC)
        :param Ttop: Temperature (ºC) at the top face of elements
        :param Tbottom: Temperature (ºC) at the bottom face of elements
        '''
        super(gradThermalStrain,self).__init__(elemSet,DOF)
        self.elThick=elThick
        self.alpha=alpha
        self.Ttop=Ttop
        self.Tbottom=Tbottom
        self.curvature= self.getCurvature() # Redundant information: remove?

    def getCurvature(self):
        ''' Return the curvature induced by the thermal gradient.
        Curvature is positive when the center of curvature
        lies along the direction of the normal to the element
        so when the temperature at the bottom face is
         greater than the temperature at the top face.
        '''
        return self.alpha*(self.Tbottom-self.Ttop)/self.elThick 
        
        
    
