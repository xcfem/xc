# -*- coding: utf-8 -*-
''' ShearReinforcement class.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import uuid
import math
from misc_utils import log_messages as lmsg
from postprocess.reports import common_formats as cf

class ShearReinforcement(object):
    ''' Definition of the variables that make up a family of shear 
    reinforcing bars.

    :ivar familyName: name identifying the family of shear reinforcing bars.
    :ivar nShReinfBranches:  number of effective branches. 
    :ivar areaShReinfBranch: area of the shear reinforcing bar [in the unit of 
                             area of the model]. 
    :ivar shReinfSpacing: longitudinal distance between transverse 
                          reinforcements [in the unit of length of the model] 
    :ivar angAlphaShReinf: angle between the shear reinforcing bars and the 
                           axis of the member expressed in radians.
    :ivar angThetaConcrStruts: angle between the concrete's compression struts 
                             and the axis of the member expressed in radians.
    '''
    def __init__(self, familyName= None,nShReinfBranches= 0, areaShReinfBranch= 0.0, shReinfSpacing= 0.2, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0):
        '''
        :param familyName: name identifying the family of shear reinforcing bars.
        :param nShReinfBranches:  number of effective branches. 
        :param areaShReinfBranch: area of the shear reinforcing bar [in 
                                  the unit of area of the model]. 
        :param shReinfSpacing: longitudinal distance between transverse 
                               reinforcements [in the unit of length 
                               of the model] 
        :param angAlphaShReinf: angle between the shear reinforcing bars 
                                and the axis of the member expressed in
                                radians.
        :param angThetaConcrStruts: angle between the concrete's compression 
                                    struts and the axis of the member
                                    expressed in radians.
        '''
        # If no name provided, generate it.
        if(not familyName):
            familyName= str(uuid.uuid1())
        self.familyName= familyName # name identifying the family of shear reinforcing bars
        self.nShReinfBranches= nShReinfBranches # Number of effective branches
        self.areaShReinfBranch= areaShReinfBranch # Area of the shear reinforcing bar
        self.shReinfSpacing= shReinfSpacing # longitudinal distance between transverse reinforcements
        self.angAlphaShReinf= angAlphaShReinf # angle between the shear reinforcing bars and the axis of the member.
        self.angThetaConcrStruts= angThetaConcrStruts # angle between the concrete's compression struts and the axis of the member
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.familyName == other.familyName)
                if(retval):
                    retval= (self.nShReinfBranches == other.nShReinfBranches)
                if(retval):
                    retval= (self.areaShReinfBranch == other.areaShReinfBranch)
                if(retval):
                    retval= (self.shReinfSpacing == other.shReinfSpacing)
                if(retval):
                    retval= (self.angAlphaShReinf == other.angAlphaShReinf)
                if(retval):
                    retval= (self.angThetaConcrStruts == other.angThetaConcrStruts)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= dict()
        retval['familyName']= self.familyName
        retval['nShReinfBranches']= self.nShReinfBranches
        retval['areaShReinfBranch']= self.areaShReinfBranch
        retval['shReinfSpacing']= self.shReinfSpacing
        retval['angAlphaShReinf']= self.angAlphaShReinf
        retval['angThetaConcrStruts']= self.angThetaConcrStruts
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.familyName= dct['familyName']
        self.nShReinfBranches= dct['nShReinfBranches']
        self.areaShReinfBranch= dct['areaShReinfBranch']
        self.shReinfSpacing= dct['shReinfSpacing']
        self.angAlphaShReinf= dct['angAlphaShReinf']
        self.angThetaConcrStruts= dct['angThetaConcrStruts']

    def clear(self):
        ''' Clears all the members of this object.'''
        self.familyName= None
        self.nShReinfBranches= None
        self.areaShReinfBranch= None
        self.shReinfSpacing= None
        self.angAlphaShReinf= None
        self.angThetaConcrStruts= None
        
    @classmethod
    def newFromDict(cls, dct= None):
        ''' Builds a new object from the data in the given dictionary.

        :param cls: class of the object itself.
        :param dct: dictionary contaning the data.
        '''
        newObject = cls.__new__(cls) # just object.__new__
        if(dct):
            newObject.setFromDict(dct)
        return newObject
    
    def getAs(self):
        '''returns the area per unit length of the family of shear 
           reinforcements.
        '''
        return self.nShReinfBranches*self.areaShReinfBranch/self.shReinfSpacing

    def getDiameter(self):
        ''' Return the diameter of the bars from its area.'''
        return 2*math.sqrt(self.areaShReinfBranch/math.pi)

    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        steelArea= self.getAs()
        if(steelArea>0.0):
            os.write(indentation+'family name: '+str(self.familyName)+'\n')
            os.write(indentation+'number of effective branches: '+str(self.nShReinfBranches)+'\n')
            os.write(indentation+'area of the shear reinforcing bar: '+str(self.areaShReinfBranch)+'\n')
            os.write(indentation+'longitudinal distance between transverse reinforcements: '+str(self.shReinfSpacing)+'\n')
            os.write(indentation+'angle between the shear reinforcing bars and the axis of the member: '+str(math.degrees(self.angAlphaShReinf))+'\n')
            os.write(indentation+'angle between the concrete\'s compression struts and the axis of the member: '+str(math.degrees(self.angThetaConcrStruts))+'\n')
        else:
            os.write(indentation+'family name: -\n')
            
    def latexReport(self, width, os= sys.stdout):
        ''' Write a report of the object in LaTeX format.

        :param width: section width.
        :param os: output stream.
        '''
        os.write("\\hline\n")
    #    os.write(self.familyName+' & '+str(self.nShReinfBranches))  #04/01/21 commented out to avoid writing  nonsense family names
        os.write(' & '+str(self.nShReinfBranches))
        areaShReinfBranchs= self.getAs()
        diamRamas= self.getDiameter()
        os.write(' & '+str(round(diamRamas*1e3)))
        os.write(' & '+cf.fmt5_2f.format(self.areaShReinfBranch*1e4))
        os.write(' & '+cf.fmt4_1f.format(self.shReinfSpacing*1e2))
        if(abs(width)>0):
            os.write(' & '+cf.fmt5_2f.format(areaShReinfBranchs*1e4))
        else:
            os.write(' & -')
        os.write(' & '+cf.fmt3_1f.format(math.degrees(self.angAlphaShReinf)))
        os.write(' & '+cf.fmt3_1f.format(math.degrees(self.angThetaConcrStruts))+"\\\\\n")
