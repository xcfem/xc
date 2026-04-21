# -*- coding: utf-8 -*-
''' TorsionReinforcement class.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import sys
from materials.sections.fiber_section import shear_reinforcement
from misc_utils import log_messages as lmsg

class TorsionReinforcement(shear_reinforcement.ShearReinforcement):
    ''' Definition of the variables that make up a family of torsion 
    reinforcing bars.

    :ivar A1: Area of the torsion longitudinal reinforcements.
    '''
    
    def __init__(self, familyName= None, A1= 0.0, nShReinfBranches= 0, areaShReinfBranch= 0.0, shReinfSpacing= 0.2,angThetaConcrStruts= math.pi/4.0):
        '''
        :param familyName: name identifying the family of shear reinforcing bars.
        :param A1: area of the torsion longitudinal reinforcements.
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
        super().__init__(familyName= familyName, nShReinfBranches= nShReinfBranches, areaShReinfBranch= areaShReinfBranch, shReinfSpacing= shReinfSpacing, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= angThetaConcrStruts)
        self.A1= A1 # area of the torsion longitudinal reinforcements.
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        retval= super(TorsionReinforcement, self).__eq__(other)
        if(retval):
            retval= (self.A1 == other.A1)
        return retval
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super().getDict()
        retval['A1']= self.A1
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setFromDict(dct)
        self.A1= dct['A1']
        
    def clear(self):
        ''' Clears all the members of this object.'''
        self.A1= None
        super().clear()
        
    def getAt(self):
        '''returns the area of the reinforcements used as hoops or 
           transverse reinforcement.
        '''
        return self.getAs()/2.0

    def getAtDiameter(self):
        ''' Return the diameter of the einforcements used as hoops or 
           transverse reinforcement.
        '''
        return super().getDiameter()

    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        super().report(os= os, indentation= indentation)
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(className+'.'+methodName+'; not implemented yet.')
        
    def latexReport(self, width, os= sys.stdout):
        ''' Write a report of the object in LaTeX format.

        :param width: section width.
        :param os: output stream.
        '''
        super().latexReport(width= width, os= os)
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(className+'.'+methodName+'; not implemented yet.')
