# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import sys
import os
import math
import uuid
import geom
import xc
from materials.sections import section_properties
from materials.sections import stress_calc as sc
from materials.sections.fiber_section import plot_fiber_section as pfs
from misc_utils import log_messages as lmsg
import matplotlib.pyplot as plt
from pathlib import Path
from misc.latex import latex_utils
from postprocess.reports import common_formats as cf
from postprocess.reports import latex_reports as ltxr
import io # strstream Python equivalent

# Classes defining reinforcement.

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

class TorsionReinforcement(ShearReinforcement):
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

#                   Reinforce concrete section
#    + -------------------------------------------------------+ 
#    |  o    o    o    o    o    o    o    o    o    o    o   | <- ReinfRow
#    |                                                        |
#    |                                                        |
#    |  o    o    o    o    o    o    o    o    o    o    o   | <- ReinfRow
#    + -------------------------------------------------------+ 
#


class ReinfRow(object):
    ''' Definition of the variables that make up a family (row) of main 
    (longitudinal) reinforcing bars.

    :ivar rebarsDiam: diameter of the bars (if omitted, the diameter is calculated from the rebar area) 
    :ivar areaRebar: cross-sectional area of the bar (if omitted, the area is calculated from the rebar diameter)
    :ivar rebarsSpacing: spacing between bars (not considered if nRebars is defined)
    :ivar nRebars: number of rebars to be placed in the row (>1)
    :ivar width: width of the cross-section (defautls to 1m)
    :ivar cover: concrete cover.
    '''
    def __init__(self, rebarsDiam=None, areaRebar= None, rebarsSpacing= None, nRebars= None, width= 1.0, nominalCover= 0.03, nominalLatCover= 0.03):
        ''' Constructor.

        :param rebarsDiam: diameter of the bars (if omitted, the diameter is calculated from the rebar area) 
        :param areaRebar: cross-sectional area of the bar (if omitted, the area is calculated from the rebar diameter)
        :param rebarsSpacing: spacing between bars (not considered if nRebars is defined)
        :param nRebars: number of rebars to be placed in the row (>1)
        :param width: width of the cross-section (defautls to 1 m)
        :param nominalCover: nominal cover (defaults to 0.03m)
        :param nominalLatCover: nominal lateral cover (only considered if nRebars is defined, defaults to 0.03)
        '''
        # Set the rebar diameter and area.
        if(rebarsDiam and areaRebar):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; you must define either the diameter or the area of rebars, but not both.')
            
        if(rebarsDiam):
            self.setRebarDiameter(rebarsDiam)
        elif(areaRebar):
            self.setRebarArea(areaRebar)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; you must define either the diameter or the rebar area.')
            exit(1)

        self.width= width
        if(nRebars and rebarsSpacing):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; you must define either the number of bars or its spacing, but not both.')
            
        if nRebars:
            self.setNumberOfBars(nRebars= nRebars, width= width, nominalLatCover= nominalLatCover)
        if rebarsSpacing:
            self.setSpacing(rebarsSpacing= rebarsSpacing, width= width)
            
        self.cover= nominalCover+self.rebarsDiam/2.0
        self.centerRebars(width)
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.rebarsDiam == other.rebarsDiam)
                if(retval):
                    retval= (self.areaRebar == other.areaRebar)
                if(retval):
                    retval= (self.rebarsSpacing == other.rebarsSpacing)
                if(retval):
                    retval= (self.nRebars == other.nRebars)
                if(retval):
                    retval= (self.width == other.width)
                if(retval):
                    retval= (self.cover == other.cover)
                if(retval and hasattr(self,'latCover')):
                    retval= (self.latCover == other.latCover)
            else:
                retval= True
        else:
            retval= False
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= {'rebarsDiam':self.rebarsDiam, 'areaRebar':self.areaRebar, 'rebarsSpacing':self.rebarsSpacing, 'nRebars': self.nRebars, 'width':self.width, 'cover':self.cover}
        if(hasattr(self,'latCover')):
           retval['latCover']= self.latCover
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.rebarsDiam= dct['rebarsDiam']
        self.areaRebar= dct['areaRebar']
        self.rebarsSpacing= dct['rebarsSpacing']
        self.nRebars= dct['nRebars']
        self.width= dct['width']
        self.cover= dct['cover']
        if('latCover' in dct):
            self.latCover= dct['latCover']
            
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

    def setRebarDiameter(self, rebarDiameter):
        '''Set the diameter of the rebars.

        :param rebarDiameter: diameter of the rebars.
        '''
        self.rebarsDiam= rebarDiameter
        self.areaRebar=math.pi*rebarDiameter**2/4.

    def setRebarArea(self, rebarArea):
        '''Set the area of the rebars.

        :param rebarArea: area of the rebars.
        '''
        self.areaRebar= rebarArea
        self.rebarsDiam=2*math.sqrt(rebarArea/math.pi)

    def setNumberOfBars(self, nRebars:int, width:float, nominalLatCover:float):
        ''' Set the number of rebars.

        :param nRebars: number of rebars.
        :param width: width occupied by the rebars.
        :param nominalLatCover: nominal lateral cover.
        '''
        self.nRebars= nRebars
        self.rebarsSpacing= (width-2*nominalLatCover-self.rebarsDiam)/(nRebars-1)
    def setSpacing(self, rebarsSpacing:float, width:float):
        ''' Set the space between rebar axes.

        :param rebarsSpacing: spacing between bars.
        :param width: width occupied by the rebars.
        '''
        self.rebarsSpacing= rebarsSpacing
        nRebarsTeor= width/rebarsSpacing
        self.nRebars= int(math.floor(nRebarsTeor))
        
    def getCopy(self):
        ''' Return a copy of this object.'''
        retval= ReinfRow(rebarsDiam= self.rebarsDiam, nRebars= self.nRebars)
        retval.rebarsSpacing= self.rebarsSpacing
        retval.width= self.width
        retval.cover= self.cover
        return retval
    
    def getAs(self):
        ''' Returns the total cross-sectional area of reinforcing steel 
           in the family.
        '''
        return self.nRebars*self.areaRebar

    def getNominalCover(self):
        ''' Return the nominal cover of the reinforcement bars.'''
        return self.cover-self.rebarsDiam/2.0

    def getNominalLatCover(self):
        ''' Return the nominal cover of the reinforcement bars.'''
        return self.latCover-self.rebarsDiam/2.0

    def getI(self):
        ''' Return the moment of inertia around the axis containing the bar
            centers.'''
        return self.nRebars*math.pi*(self.rebarsDiam/2.0)**4/4.0
      
    def centerRebars(self,width):
        '''center the row of rebars in the width of the section'''
        self.latCover= (width-(self.nRebars-1)*self.rebarsSpacing)/2.0

    def defStraightLayer(self, reinforcement, layerCode, diagramName, p1, p2):
        '''Definition of a straight reinforcement layer in the XC section 
           geometry object between the 2d positions p1 and p2.

        :param reinforcement: XC section geometry reinforcement.
        :param layerCode: identifier for the layer.
        :param diagramName: name of the strain-stress diagram of the steel.
        :param p1: first point of the layer.
        :param p2: last point of the layer.
        '''
        if(self.nRebars>0):
            self.reinfLayer= reinforcement.newStraightReinfLayer(diagramName)
            self.reinfLayer.code= layerCode
            self.reinfLayer.numReinfBars= self.nRebars
            self.reinfLayer.barDiameter= self.rebarsDiam
            self.reinfLayer.barArea= self.areaRebar
            self.reinfLayer.p1= p1
            self.reinfLayer.p2= p2
            return self.reinfLayer
        
    def defCircularLayer(self,reinforcement, code, diagramName, extRad, initAngle= 0.0, finalAngle= 2*math.pi):
        '''Definition of a circular reinforcement layer in the XC section 
           geometry object between the angle arguments.

        :param reinforcement: XC section geometry reinforcement.
        :param code: identifier for the layer.
        :param diagramName: name of the strain-stress diagram of the steel.
        :param extRad: concrete external radius. 
        :param initAngle: initial angle.
        :param finalAngle: final angle.
        '''
        if(self.nRebars>0):
            self.reinfLayer= reinforcement.newCircReinfLayer(diagramName)
            self.reinfLayer.code= code
            self.reinfLayer.numReinfBars= self.nRebars
            self.reinfLayer.barDiameter= self.rebarsDiam
            self.reinfLayer.barArea= self.areaRebar
            self.reinfLayer.initAngle= initAngle
            self.reinfLayer.finalAngle= finalAngle
            self.reinfLayer.radius= extRad-self.cover
            return self.reinfLayer
        
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        os.write(indentation+'bar diameter: '+str(self.rebarsDiam*1e3)+' mm\n')
        os.write(indentation+'bar area: '+str(self.areaRebar*1e4)+' cm2\n')
        os.write(indentation+'spacing: '+str(self.rebarsSpacing*1e3)+' mm\n')
        os.write(indentation+'number of bars: '+str(self.nRebars)+'\n')
        os.write(indentation+'width: '+str(self.width*1e3)+' mm\n')
        os.write(indentation+'cover: '+str(self.cover*1e3)+' mm\n')

def RebarRow2ReinfRow(rebarRow, width= 1.0, nominalLatCover= 0.03):
    ''' Returns a ReinfRow object from a RebarRow object
        as defined in the rebar_family module.

    :param rebarRow: RebarRow object.
    :param width: width of the cross-section (defautls to 1 m)
    :param nominalLatCover: nominal lateral cover (only considered if nRebars is defined, defaults to 0.03)
    '''
    retval= None
    if(rebarRow):
        retval= ReinfRow(rebarsDiam= rebarRow.diam,rebarsSpacing= rebarRow.spacing,width= width, nominalCover= rebarRow.concreteCover, nominalLatCover= nominalLatCover)
    return retval
    

class LongReinfLayers(object):
    ''' Layers of longitudinal reinforcement.'''
    def __init__(self, lst= None):
        ''' Constructor.'''
        if(lst):
            self.rebarRows= lst  # list of ReinfRow data
        else:
            self.rebarRows= list()
        self.reinfLayers= list()  # list of StraightReinfLayer created.
                    
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (len(self.rebarRows) == len(other.rebarRows))
                if(retval):
                    for rowA, rowB in zip(self.rebarRows, other.rebarRows):
                        retval= (rowA==rowB)
                        if(not retval):
                            break
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def __getitem__(self, index):
        '''Return the i-th reinforcement row.'''
        return self.rebarRows[index]

    def __len__(self):
        '''Return the number of reinforcement rows.'''
        return len(self.rebarRows)

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        tmp= list()
        for rr in self.rebarRows:
            tmp.append(rr)
        retval= {'rebarRows':tmp}
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.rebarRows= dct['rebarRows']
            
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
            
    def append(self, rebarRow:ReinfRow):
        ''' Append a reinforcement row to the list.

        :rebarRow: row of reinforcement bars.
        '''
        self.rebarRows.append(rebarRow)
        
    def getAsRows(self):
        '''Returns a list with the cross-sectional area of the rebars in 
           each row.'''
        retval=[]
        for rbRow in self.rebarRows:
            retval.append(rbRow.getAs())
        return retval
       
    def getAs(self):
        '''returns the cross-sectional area of the rebars.'''
        return sum(self.getAsRows())

    def getMinCover(self):
        '''Return the minimum value of the cover.'''
        retval= 1e6
        if(len(self.rebarRows)>0):
            retval= self.rebarRows[0].cover
            for rbRow in self.rebarRows[1:]:
                retval= min(retval,rbRow.cover)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; reinforcement layer is empty, returning default valueunknown.\n")
            retval= 0.035 # Default value.
        return retval
        
    def getRowsCGcover(self):
        '''returns the distance from the center of gravity of the rebars
        to the face of the section 
        '''
        retval=0
        if(len(self.rebarRows)>0):
            for rbRow in self.rebarRows:
                retval+= rbRow.getAs()*rbRow.cover
            retval/= self.getAs()
        return retval

    def getSpacings(self):
        '''returns a list with the distance between bars for each row of bars.'''
        retval=[]
        for rbRow in self.rebarRows:
            retval.append(rbRow.rebarsSpacing)
        return retval
    
    def getDiameters(self):
        '''returns a list with the bar diameter for each row of bars in local 
        positive face.'''
        retval=[]
        for rbRow in self.rebarRows:
            retval.append(rbRow.rebarsDiam)
        return retval
    
    def getNBar(self):
        '''returns a list with the number of bars for each row.'''
        retval=[]
        for rbRow in self.rebarRows:
            retval.append(rbRow.nRebars)
        return retval

    def getCover(self):
        '''returns a list with the cover of bars for each row of bars.'''
        retval=[]
        for rbRow in self.rebarRows:
            retval.append(rbRow.cover)
        return retval
    
    def getLatCover(self):
        '''returns a list with the lateral cover of bars for each row of bars.
        '''
        retval=[]
        for rbRow in self.rebarRows:
            retval.append(rbRow.latCover)
        return retval
    
    def getNominalLatCover(self):
        '''returns a list with the nominal lateral cover of bars for each 
           row of bars. '''
        retval=[]
        for rbRow in self.rebarRows:
            retval.append(rbRow.getNominalLatCover())
        return retval

    def centerRebars(self, b):
        '''centers in the width of the section the rebars.''' 
        for rbRow in self.rebarRows:
            rbRow.centerRebars(b)
            
    def defStraightLayers(self, reinforcement, layerCode, diagramName, pointPairs):
        '''
        Definition of the reinforcement layers

        :param reinforcement: XC section reinforcement.
        :param layerCode: identifier for the layer.
        :param diagramName: name of the strain-stress diagram of the steel.
        :param pointPairs: end points for each row.
        '''
        for rbRow, pts in zip(self.rebarRows, pointPairs):
            p1= pts[0]; p2= pts[1]
            self.reinfLayers.append(rbRow.defStraightLayer(reinforcement,layerCode,diagramName,p1,p2))
            
    def defCircularLayers(self, reinforcement, code, diagramName, extRad, anglePairs= None):
        '''
        Definition of the reinforcement layers

        :param reinforcement: XC section reinforcement.
        :param code: identifier for the layer.
        :param diagramName: name of the strain-stress diagram of the steel.
        :param points: end points for each row.
        '''
        if(len(self.rebarRows)>0):
            if(not anglePairs):
                for rbRow in self.rebarRows:
                    layer= rbRow.defCircularLayer(reinforcement,code,diagramName,extRad)
                    self.reinfLayers.append(layer)
            else:
                for rbRow, angles in zip(self.rebarRows, anglePairs):
                    initAngle= anglePairs[0]; finalAngle= anglePairs[1]
                    layer= rbRow.defCircularLayer(reinforcement,code,diagramName, extRad, initAngle, finalAngle)
                    self.reinfLayers.append(layer)
        else:            
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warningMsg= '; no longitudinal reinforcement.'
            lmsg.warning(className+'.'+methodName+warningMsg)

    def clearLayers(self):
        ''' Clear the previously defined reinforcement layers.'''
        self.reinfLayers.clear()

    def clear(self):
        ''' Clear previously defined longitudinal reinforcement.'''
        self.clearLayers()
        self.rebarRows.clear()
            
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.

        :param os: output stream.
        '''
        os.write(indentation+'rebar rows: \n')
        for rrow in self.rebarRows:
            rrow.report(os, indentation+'  ')        

def rebLayer_mm(fi,s,c):
    '''Defines a layer of main reinforcement bars, given the spacement.

    :param fi: bar diameter [mm]
    :param s: spacing [mm]
    :param c: cover [mm] (nominal cover)
    '''
    return ReinfRow(rebarsDiam=fi*1e-3, rebarsSpacing=s*1e-3,width=1.0,nominalCover=c*1e-3)

def rebLayerByNumFi_mm(n, fi, c, latC, L):
    '''Defines a layer of  main reinforcement bars with a fixed number of rebars. Spacing is calculated
    so that the rebars (and two lateral covers) are inserted in the length L passed as parameter.

    :param n: number of rebars
    :param fi: bar diameter [mm]
    :param c: nominal cover [mm]
    :param latC: nominal lateral cover [mm]
    :param L: length where the n rebars and two lateral covers are inserted [mm]
    '''
    rl=ReinfRow(rebarsDiam=fi*1e-3,nRebars=n,width=L*1e-3,nominalCover=c*1e-3,nominalLatCover=latC*1e-3)
    return rl

def rebLayer_m(fi,s,c):
    '''Defines a layer of main reinforcement bars, given the spacement.

    :param fi: bar diameter [m]
    :param s: spacing [m]
    :param c: cover [m] (nominal cover)
    '''
    return ReinfRow(rebarsDiam=fi, rebarsSpacing=s,width=1.0,nominalCover=c)

def rebLayerByNumFi_m(n, fi, c, latC, L):
    '''Defines a layer of  main reinforcement bars with a fixed number of rebars. Spacing is calculated
    so that the rebars (and two lateral covers) are inserted in the length L passed as parameter.

    :param n: number of rebars
    :param fi: bar diameter [m]
    :param c: nominal cover [m]
    :param latC: nominal lateral cover [m]
    :param L: length where the n rebars and two lateral covers are inserted [m]
    '''
    return ReinfRow(rebarsDiam=fi, nRebars=n, width=L, nominalCover=c, nominalLatCover=latC)

# Reinforced concrete.

class RCFiberSectionParameters(object):
    '''
    Parameters needed to create a reinforced concrete fiber section.

    :ivar concrType:       type of concrete (e.g. EHE_materials.HA25)     
    :ivar concrDiagName:   name identifying the characteristic stress-strain 
                           diagram of the concrete material
    :ivar reinfSteelType:  type of reinforcement steel
    :ivar reinfDiagName:   name identifying the characteristic stress-strain 
                           diagram of the reinforcing steel material
    :ivar nDivIJ:          number of cells in IJ (width or radial) direction
    :ivar nDivJK:          number of cells in JK (height or tangential)
                           direction
    '''
    def __init__(self, concrType, reinfSteelType, nDivIJ= 10, nDivJK= 10):
        self.concrType= concrType
        self.concrDiagName= None
        self.reinfSteelType= reinfSteelType
        self.reinfDiagName= None # Name of the uniaxial material
        self.nDivIJ= nDivIJ
        self.nDivJK= nDivJK
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= dict()
        retval['concrete_type']= self.concrType
        retval['concrete_diagram_name']= self.concrDiagName
        retval['reinforcing_steel_type']= self.reinfSteelType
        retval['reinforcing_steel_diagram_name']= self.reinfDiagName
        retval['n_div_ij']= self.nDivIJ
        retval['n_div_jk']= self.nDivJK
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        self.concrType= dct['concrete_type']
        self.concrDiagName= dct['concrete_diagram_name']
        self.reinfSteelType= dct['reinforcing_steel_type']
        self.reinfDiagName= dct['reinforcing_steel_diagram_name']
        self.nDivIJ= dct['n_div_ij']
        self.nDivJK= dct['n_div_jk']
        
            
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
    
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.concrType == other.concrType)
                if(retval):
                    retval= (self.concrDiagName == other.concrDiagName)
                if(retval):
                    retval= (self.reinfSteelType == other.reinfSteelType)
                if(retval):
                    retval= (self.reinfDiagName == other.reinfDiagName)
                if(retval):
                    retval= (self.nDivIJ == other.nDivIJ)
                if(retval):
                    retval= (self.nDivJK == other.nDivJK)
            else:
                retval= True
        else:
            retval= False
        return retval

    def nDivCirc(self):
        '''Alias for nDivIJ when defining circular sections.'''
        return self.nDivIJ
    
    def nDivRad(self):
        '''Alias for nDivJK when defining circular sections.'''
        return self.nDivJK

    def getConcreteDiagram(self, preprocessor):
        ''' Return the concrete strain-stress diagram.

        :param preprocessor: preprocessor of the finite element problem.
        '''

        return preprocessor.getMaterialHandler.getMaterial(self.concrDiagName)
      
    def getSteelDiagram(self, preprocessor):
        ''' Return the steel strain-stress diagram.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return preprocessor.getMaterialHandler.getMaterial(self.reinfDiagName)
      
    def getSteelEquivalenceCoefficient(self, preprocessor):
        ''' Return the equivalence coefficiente for the steel (Es/Ec).

        :param preprocessor: preprocessor of the finite element problem.
        '''
        tangHorm= self.getConcreteDiagram(preprocessor).getTangent()
        tangSteel= self.getSteelDiagram(preprocessor).getTangent()
        return tangSteel/tangHorm

    def defDiagrams(self, preprocessor, matDiagType):
        '''Stress-strain diagrams definition.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
        '''
        self.diagType= matDiagType
        if(self.diagType=="d"): # design diagram.
            if(self.concrType.matTagD<0):
                concreteMatTag= self.concrType.defDiagD(preprocessor)
                if(__debug__):
                    if(concreteMatTag is None):
                        AssertionError('Can\'t get concrete stress-strain diagram.')                    
            if(self.reinfSteelType.matTagD<0):
                reinfSteelMaterialTag= self.reinfSteelType.defDiagD(preprocessor)
                if(__debug__):
                    if(reinfSteelMaterialTag is None):
                        AssertionError('Can\'t get steel stress-strain diagram.')                    
            self.concrDiagName= self.concrType.nmbDiagD
            self.reinfDiagName= self.reinfSteelType.nmbDiagD
        elif(self.diagType=="k"): # characteristic diagram
            if(self.concrType.matTagK<0):
                concreteMatTag= self.concrType.defDiagK(preprocessor)
                if(__debug__):
                    if(concreteMatTag is None):
                        AssertionError('Can\'t get concrete stress-strain diagram.')                    
            if(self.reinfSteelType.matTagK<0):
                reinfSteelMaterialTag= self.reinfSteelType.defDiagK(preprocessor)
                if(__debug__):
                    if(reinfSteelMaterialTag is None):
                        AssertionError('Can\'t get steel stress-strain diagram.')                    
            self.concrDiagName= self.concrType.nmbDiagK
            self.reinfDiagName= self.reinfSteelType.nmbDiagK
        elif(self.diagType=='td'): # creep diagram.
            if(self.concrType.matTagTD<0):
                concreteMatTag= self.concrType.defDiagTD(preprocessor)
                if(__debug__):
                    if(concreteMatTag is None):
                        AssertionError('Can\'t get concrete stress-strain diagram.')                    
            if(self.reinfSteelType.matTagE<0): # linear elastic steel.
                reinfSteelMaterialTag= self.reinfSteelType.defDiagE(preprocessor)
                if(__debug__):
                    if(reinfSteelMaterialTag is None):
                        AssertionError('Can\'t get steel stress-strain diagram.')                    
            self.concrDiagName= self.concrType.nmbDiagTD
            self.reinfDiagName= self.reinfSteelType.nmbDiagE # linear elastic steel.
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; diagram type: '"+self.diagType+"' unknown.\n")
            exit(1)
            
    def clearDiagrams(self):
        '''Clear the previously defined stress-strain diagrams for the section
           materials.
        '''
        self.concrType.clearDiagrams()
        self.reinfSteelType.clearDiagrams()
        self.diagType= None
        self.concrDiagName= None
        self.reinfDiagName= None
            
    def defInteractionDiagramParameters(self, preprocessor):
        ''' Defines the parameters for interaction diagrams.

         :param preprocessor: preprocessor of the finite element problem.
        '''
        self.idParams= xc.InteractionDiagramParameters()
        if(self.diagType=="d"):
            self.idParams.concreteTag= self.concrType.matTagD
            self.idParams.reinforcementTag= self.reinfSteelType.matTagD
        elif(self.diagType=="k"):
            self.idParams.concreteTag= self.concrType.matTagK
            self.idParams.reinforcementTag= self.reinfSteelType.matTagK
        return self.idParams
    
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        os.write(indentation+'concrete type: '+str(self.concrType.materialName)+'\n')
        os.write(indentation+'concrete stress-strain diagram: '+str(self.concrDiagName)+'\n')
        os.write(indentation+'steel type: '+str(self.reinfSteelType.materialName)+'\n')
        os.write(indentation+'steel stress-strain diagram: '+str(self.reinfDiagName)+'\n')
        os.write(indentation+'number of IJ divisions nDivIJ= '+str(self.nDivIJ)+'\n')
        os.write(indentation+'number of JK divisions nDivJK= '+str(self.nDivJK)+'\n')
        

class RCSectionBase(object):
    '''
    Base class for reinforced concrete sections.

    :ivar sectionDescr: section description.
    :ivar fiberSectionParameters: Parameters needed to create a reinforced 
                                  concrete fiber section.
    :ivar fiberSectionRepr: fiber model of the section.
    '''
    def __init__(self, sectionDescr= None, concrType=None, reinfSteelType=None, nDivIJ= 10, nDivJK= 10):
        ''' Constructor.

        :param sectionDescr: section description.
        :param concrType: type of concrete (e.g. EHE_materials.HA25).     
        :param reinfSteelType: type of reinforcement steel.
        :param nDivIJ: number of cells in IJ (width or radial) direction.
        :param nDivJK: number of cells in JK (height or tangential) direction.
        '''
        if(sectionDescr is None):
            self.sectionDescr= 'Text describing the role/position of the section in the structure.'
        else:
            self.sectionDescr= sectionDescr
        self.fiberSectionParameters= RCFiberSectionParameters(concrType= concrType, reinfSteelType= reinfSteelType, nDivIJ= nDivIJ, nDivJK= nDivJK)
        self.fiberSectionRepr= None
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= dict()
        retval['section_description']= self.sectionDescr
        retval['fiber_section_parameters']= self.fiberSectionParameters
        retval['fiber_section_representation']= self.fiberSectionRepr
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        self.sectionDescr= dct['section_description']
        self.fiberSectionParameters= dct['fiber_section_parameters']
        self.fiberSectionRepr= dct['fiber_section_representation']
        
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
    
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.sectionDescr == other.sectionDescr)
                if(retval):
                    retval= (self.fiberSectionParameters == other.fiberSectionParameters)
                if(retval):
                    retval= (self.fiberSectionRepr == other.fiberSectionRepr)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def getCopy(self):
        ''' Returns a copy of the object.'''
        retval= RCSectionBase(sectionDescr= self.sectionDescr, concrType= self.getConcreteType(), reinfSteelType= self.getReinfSteelType(), nDivIJ= self.getNDivIJ(), nDivJK= self.getNDivJK())
        return retval

    def gmSectionName(self):
        ''' returns the name of the geometric section'''
        return "geom"+self.name

    def getMaterialHandler(self):
        ''' Return the material handler used to define the XC materials
            corresponding to this section object.'''
        return self.fiberSectionRepr.getMaterialHandler
        
    def getSectionGeometry(self):
        ''' Return the geometry defined for this section in XC.'''
        geomSectionName= self.gmSectionName()
        materialHandler= self.getMaterialHandler()
        retval= materialHandler.getSectionGeometry(geomSectionName)
        return retval
            
    
    def getConcreteType(self):
        ''' returns the concrete type of this sections.'''
        return self.fiberSectionParameters.concrType
    
    def getReinfSteelType(self):
        ''' returns the type of the reinforcing steel in this sections.'''
        return self.fiberSectionParameters.reinfSteelType

    def getElasticMaterialData(self, overrideRho= None):
        ''' Return an elastic material constitutive model.

        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return self.fiberSectionParameters.concrType.getElasticMaterialData(overrideRho= overrideRho)
    
    def getHomogenizationCoefficient(self):
        '''Return the homogenization coefficient of the section.'''
        return self.fiberSectionParameters.reinfSteelType.Es/self.fiberSectionParameters.concrType.Ecm()
    
    def izHomogenizedSection(self):
        '''Return the radius of gyration of the section around
           the axis parallel to the section width that passes 
           through section centroid.
        '''
        return math.sqrt(self.getIzHomogenizedSection()/self.getAreaHomogenizedSection())
    
    def getNonDimensionalAxialForce(self, Nd):
        ''' Return the valud of the non-dimensional axial force corresponding 
            to the given axial force according to clause 43.1.2 of EHE-08.

        :param Nd: design value of the axial force.
        '''
        fcd= self.fiberSectionParameters.concrType.fcd()
        Ac= self.getAc()
        return Nd/Ac/fcd
    
    def getNDivIJ(self):
        ''' Return the number of cells in IJ (width or radial) direction.'''
        return self.fiberSectionParameters.nDivIJ

    def getNDivJK(self):
        ''' Return the number of cells in JK (height or tangential) direction.'''
        return self.fiberSectionParameters.nDivJK
    
    def getConcreteDiagram(self, preprocessor):
        ''' Return the concrete stress-strain diagram.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return self.fiberSectionParameters.getConcreteDiagram(preprocessor)
      
    def getSteelDiagram(self, preprocessor):
        ''' Return the reinforcing steel stress-strain diagram.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return self.fiberSectionParameters.getSteelDiagram(preprocessor)
      
    def getSteelEquivalenceCoefficient(self, preprocessor):
        ''' Return the steel equivalence coefficient: Es/Ec.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return self.fiberSectionParameters.getSteelEquivalenceCoefficien(preprocessor)

    def defDiagrams(self, preprocessor, matDiagType):
        '''Stress-strain diagrams definition.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
        '''
        return self.fiberSectionParameters.defDiagrams(preprocessor, matDiagType)
    def clearDiagrams(self):
        '''Clear previously defined diagrams.'''
        return self.fiberSectionParameters.clearDiagrams()
    
    def defShearResponse2d(self, preprocessor):
        ''' Define the shear response of the 2D section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        self.respVy= self.getRespVy(preprocessor)
        
    def defShearResponse(self, preprocessor):
        ''' Define the shear/torsional response of the section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        self.respT= self.getRespT(preprocessor) # Torsional response of the section.
        self.respVy= self.getRespVy(preprocessor)
        self.respVz= self.getRespVz(preprocessor)
        
    def clearShearResponse(self):
        ''' Clear the shear/torsional response of the section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        self.respT= None # Torsional response of the section.
        self.respVy= None
        self.respVz= None

    def defFiberSection2d(self, preprocessor):
        '''Define 2D fiber section from geometry data.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        self.fiberSection= preprocessor.getMaterialHandler.newMaterial("fiberSectionShear2d", self.name)
        self.fiberSectionRepr= self.fiberSection.getFiberSectionRepr()
        self.fiberSectionRepr.setGeomNamed(self.gmSectionName())
        self.fiberSection.setupFibers()
        self.fiberSection.setRespVyByName(self.respVyName())
        self.fiberSection.setProp('sectionData',self)
        
    def defRCSection2d(self, preprocessor, matDiagType):
        ''' Definition of a 2D reinforced concrete section.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
         '''
        self.defShearResponse2d(preprocessor)
        self.defSectionGeometry(preprocessor,matDiagType)
        self.defFiberSection2d(preprocessor)
        
    def defFiberSection(self, preprocessor):
        '''Define fiber section from geometry data.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        self.fiberSection= preprocessor.getMaterialHandler.newMaterial("fiberSectionShear3d", self.name)
        self.fiberSectionRepr= self.fiberSection.getFiberSectionRepr()
        self.fiberSectionRepr.setGeomNamed(self.gmSectionName())
        self.fiberSection.setupFibers()
        self.fiberSection.setRespVyByName(self.respVyName())
        self.fiberSection.setRespVzByName(self.respVzName())
        self.fiberSection.setRespTByName(self.respTName())
        self.fiberSection.setProp('sectionData',self)
        
    def clearFiberSection(self):
        '''Clear the previously defined fiber section.'''
        if(self.fiberSectionRepr):
            self.fiberSectionRepr.clear()
            self.fiberSectionRepr= None
            self.fiberSection.clear()
            self.fiberSection= None
        
    def defRCSection(self, preprocessor, matDiagType):
        ''' Definition of an XC reinforced concrete section.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
        '''
        self.defShearResponse(preprocessor= preprocessor)
        self.defSectionGeometry(preprocessor= preprocessor, matDiagType= matDiagType)
        self.defFiberSection(preprocessor= preprocessor)
        
    def clearRCSection(self):
        ''' Clear a previously defined XC reinforced concrete section (possibly
            with a different preprocessor, which can lead to errors).
        '''
        self.clearShearResponse()
        self.clearSectionGeometry()
        self.clearFiberSection()

    def isCircular(self):
        ''' Return true if it's a circular section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return False
        
    def getMainReinforcementArea(self):
        ''' Return the total area of the main reinforcement.'''
        return self.geomSection.getReinfLayers.getAreaGrossSection()
    
    def defInteractionDiagramParameters(self, preprocessor):
        ''' parameters for interaction diagrams.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return self.fiberSectionParameters.defInteractionDiagramParameters(preprocessor)

    def defInteractionDiagram(self, preprocessor, matDiagType= None):
        '''Defines 3D interaction diagram.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: if not None call defRCSection to create the fiber
                            model of this section.
        '''
        if(matDiagType):
            self.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)
        if(not self.fiberSectionRepr):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "Fiber section representation for section: "+ self.name + ";  not defined yet; use defRCSection (or defRCSection2d) method."
            lmsg.error(className+'.'+methodName+errMsg)
            exit(1)
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagram(self.name,self.fiberSectionParameters.idParams)

    def defInteractionDiagramNMy(self, preprocessor, matDiagType= None):
        '''Defines N-My interaction diagram.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: if not None call defRCSection to create the fiber
                            model of this section.
        '''
        if(matDiagType):
            self.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)
        if(not self.fiberSectionRepr):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "Fiber section representation for section: "+ self.name + ";  not defined yet; use defRCSection (or defRCSection2d) method."
            lmsg.error(className+'.'+methodName+errMsg)
            exit(1)
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagramNMy(self.name,self.fiberSectionParameters.idParams)

    def defInteractionDiagramNMz(self, preprocessor, matDiagType= None):
        '''Defines N-Mz interaction diagram.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: if not None call defRCSection to create the fiber
                            model of this section.
        '''
        if(matDiagType):
            self.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)
        if(not self.fiberSectionRepr):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "Fiber section representation for section: "+ self.name + ";  not defined yet; use defRCSection (or defRCSection2d) method."
            lmsg.error(className+'.'+methodName+errMsg)
            exit(1)
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagramNMz(self.name,self.fiberSectionParameters.idParams)

    def subplot(self, ax, preprocessor, matDiagType= 'k'):
        ''' Put the section drawing in the subplot argument.

        :param ax: matplotlib subplot.
        :param preprocessor: pre-processor of the finite element problem.
        :param matDiagType: type of stress-strain diagram
                     ("k" for characteristic diagram, "d" for design diagram)
        '''
        ax.axis('equal')
        ax.set_title('Section: '+self.name)
        ax.grid(visible= True, linestyle='dotted')
        # Plot contour.
        contour= self.getContour()
        x= list(); y= list()
        for p in contour:
            x.append(p.x)
            y.append(p.y)
        ax.fill(x,y,'tab:gray')
        #ax.plot(x,y,'tab:blue')
        # Plot reinforcement.
        if(not hasattr(self, 'geomSection')):
            self.defSectionGeometry(preprocessor, matDiagType)
        reinforcement= self.geomSection.getReinfLayers
        pfs.mplot_section_reinforcement(ax, reinforcement)

    def plot(self, preprocessor, matDiagType= 'k'):
        ''' Get a drawing of the section using matplotlib.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                            ("k" for characteristic diagram, 
                             "d" for design diagram)
        '''
        fig = plt.figure()
        ax = fig.add_subplot(111)
        self.subplot(ax, preprocessor= preprocessor, matDiagType= matDiagType)
        plt.show()
   
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.

        :param os: output stream.
        '''
        os.write(indentation+'Section description: '+str(self.sectionDescr)+'\n')
        indentation+= '  '
        os.write(indentation+'Fiber section parameters:\n')
        self.fiberSectionParameters.report(os, indentation+'  ')
        if(self.fiberSectionRepr):
            self.fiberSectionRepr.report(os, indentation+'  ')
            
    def latexReportMainReinforcementLayer(self, reinfLayer, concreteArea, os= sys.stdout):
        ''' Write a report of the reinforcement layer argument 
            in LaTeX format.

        :param os: output stream.
        '''
        name= reinfLayer.name
        nRebars= reinfLayer.numReinfBars # number of rebars
        rebarsDiam= reinfLayer.barDiameter # Rebars diameter
        areaRebar= reinfLayer.barArea # total area of reinforcement in the layer
        minEffCover= reinfLayer.getCover() # Minimum value of effective cover
        barsCOG= reinfLayer.getCenterOfMass() # center of gravity of the bars
        os.write(name+' & '+str(nRebars))
        os.write(' & '+str(round(rebarsDiam*1e3)))
        os.write(' & '+cf.fmt5_2f.format(areaRebar*1e4))
        os.write(' & '+cf.fmt4_2f.format(areaRebar/concreteArea*1e3))
        os.write(' & '+cf.fmt4_1f.format(minEffCover*1e2))
        os.write(' & '+cf.fmt5_3f.format(barsCOG[0]) +' & '+cf.fmt5_3f.format(barsCOG[1]) +"\\\\\n")

    def latexReportMainReinforcement(self, os= sys.stdout):
        ''' Write a report of the main reinforcement in LaTeX format.

        :param os: output stream.
        '''
        os.write("\\begin{tabular}{ll}\n")
        mainReinfFamilies= self.geomSection.getReinfLayers
        areaMainReinforcement= mainReinfFamilies.getAreaGrossSection()
        concreteArea= self.geomSection.getAreaGrossSection() # Area
        os.write("Total area $A_s="+cf.fmt5_2f.format(areaMainReinforcement*1e4) +"\\ cm^2$ & Geometric quantity $\\rho= "+cf.fmt4_2f.format(areaMainReinforcement/concreteArea*1e3) +"\\permil$\\\\\n")
        os.write("\\end{tabular} \\\\\n")
        os.write("\\hline\n")
        os.write("Layers of main reinforcement:\\\\\n")
        os.write("\\hline\n")
        os.write("\\begin{tabular}{cccccccc}\n")
        os.write("Id & N$^o$ bars & $\\phi$ & area & c. geom. & eff. cover & $y_{COG}$ & $z_{COG}$\\\\\n")
        os.write(" &  & $(mm)$ & $(cm^2)$ & $(\\permil)$ & $(cm)$ & $(m)$ & $(m)$\\\\\n")
        for f in mainReinfFamilies:
            os.write("\\hline\n")
            self.latexReportMainReinforcementLayer(f, concreteArea, os)
        os.write("\\end{tabular} \\\\\n")

    def getCrossSectionFigureFileName(self, outputPath= None):
        ''' Return the file name to use for the cross-section graphics.

        :param outputPath: directory to write the section plot into.
        '''
        retval= ''.join(x for x in self.name if x.isalnum())
        if(outputPath):
            retval= outputPath+'/'+retval
        return retval

    def latexReport(self, os= sys.stdout, graphicWidth='70mm', outputPath= None, includeGraphicsPath= None, preprocessor= None, matDiagType= 'k'):
        ''' Write a report of the object in LaTeX format.

        :param os: output stream.
        :param graphicWidth: width for the cross-section graphic.
        :param outputPath: directory to write the section plot into.
        :param includeGraphicsPath: directory to use in the latex includegraphics command.
        :param preprocessor: pre-processor of the FE problem.
        :param matDiagType: diagram type; if "k" use the diagram 
                            corresponding to characteristic values of the 
                            material, if "d" use the design values one.
        '''
        temporaryFiles= list()
        clearRCSection= False
        # Retrieve section geometry definition.
        if(not self.fiberSectionRepr):
            if(preprocessor):
                self.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)
                clearRCSection= True
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errMsg= "; no section representation for section: '"+self.name+"'. and undefined preprocessor. Can't call defRCSection."
                lmsg.error(className+'.'+methodName+errMsg)
        if(self.fiberSectionRepr):
            geomSection= self.getSectionGeometry()
            # Plot cross-section
            crossSectionFigureFName= self.getCrossSectionFigureFileName(outputPath= outputPath)
            if(self.geomSection):
                epsFileName= crossSectionFigureFName+'.eps'
                pfs.plot_section_geometry(geomSection,epsFileName)
                temporaryFiles.append(Path(epsFileName))
                # Convert the image to PNG
                pfs.eps2png(inputFileName= epsFileName, outputFileName= None)
                temporaryFiles.append(Path(epsFileName).with_suffix('.png'))
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; geometry of section: '+self.name+' not defined\n')

            # Some convenience lines.
            os.write('%% ****** Packages needed for LaTeX document: ****** \n')
            os.write('%%\\usepackage{graphicx} %%\\postscript includes\n')
            os.write('%%\\usepackage{multirow} %%\\multirow command\n')
            os.write('%%\\usepackage{wasysym} %%\\permil command\n')
            os.write('%%\\usepackage{gensymb} %%\\degree command\n')
            # Write table header.
            os.write('\\begin{table}\n')
            os.write('\\begin{center}\n')
            os.write('\\begin{tabular}{|c|}\n')
            os.write('\\hline\n')
            os.write('\\begin{large} '+latex_utils.toLaTeX(self.name)+' \\end{large}\\\\\n')
            os.write('\\hline\n')
            os.write(self.sectionDescr+'\\\\\n')
            os.write('\\hline\n')
            # Section geometry
            os.write('\\begin{tabular}{c|l}\n')
            ## Include figure in table
            os.write('\\begin{minipage}{85mm}\n')
            os.write('\\vspace{2mm}\n')
            os.write('\\begin{center}\n')

            #  name without extension to allow pdfLatex choose the file
            nameWOExt= Path(crossSectionFigureFName).stem
            if(includeGraphicsPath):
                nameWOExt= includeGraphicsPath+'/'+nameWOExt
            os.write('\\includegraphics[width='+graphicWidth+']{'+nameWOExt+'}\n')
            os.write('\\end{center}\n')
            os.write('\\vspace{1pt}\n')
            os.write('\\end{minipage} & \n')
            self.latexReportGeometry(os)
            os.write('\\end{tabular} \\\\\n')
            # Write materials.
            os.write('\\hline\n')
            os.write('\\textbf{Materials - mechanical properties}:\\\\\n')
            os.write('\\hline\n')
            os.write('\\begin{tabular}{ll}\n')
            concrete= self.fiberSectionParameters.concrType
            os.write('Concrete: '+concrete.materialName+' & Modulus of elasticity: $E_c= '+'{0:.2f}'.format(concrete.Ecm()/1e9)+'\\ GPa$\\\\\n')
            os.write('\\hline\n')
            steel= self.fiberSectionParameters.reinfSteelType
            os.write('Steel: '+steel.materialName+' & Modulus of elasticity: $E_s= '+'{0:.2f}'.format(steel.Es/1e9)+'\\ GPa$\\\\\n')
            os.write('\\end{tabular} \\\\\n')
            os.write('\\hline\n')
            # Write section mechanical properties.
            os.write('\\textbf{Sections - geometric and mechanical characteristics}:\\\\\n')
            os.write('\\hline\n')

            os.write('Gross section:\\\\\n')
            GB= self.geomSection.getCenterOfMassGrossSection() # Center of mass.
            AB= self.geomSection.getAreaGrossSection() # Area
            IyB= self.geomSection.getIyGrossSection() # Inertia
            IzB= self.geomSection.getIzGrossSection()
            PyzB= self.geomSection.getPyzGrossSection()
            JTorsion= self.getJTorsion()
            os.write('\\hline\n')
            os.write('\\begin{tabular}{ll}\n')
            os.write('$A_{gross}='+cf.fmt6_3f.format(AB) +'\\ m^2$ & \\multirow{3}{*}{Inertia tensor ($cm^4$): $ \\left( \\begin{array}{ccc}'+ cf.fmt5_2f.format(JTorsion*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ cf.fmt5_2f.format(IyB*1e4) +' & '+cf.fmt5_2f.format(PyzB) +' \\\\ 0.00 & '+cf.fmt5_2f.format(PyzB) +' & '+cf.fmt5_2f.format(IzB*1e4) +' \\end{array} \\right)$} \\\\\n')
            os.write('& \\\\\n')
            os.write('C.O.G.: $('+cf.fmt5_3f.format(GB[0])+','+cf.fmt5_3f.format(GB[1])+')\\ m$  & \\\\\n')
            os.write('\\end{tabular} \\\\\n')
            os.write('\\hline\n')
            os.write('Homogenized section:\\\\\n')
            preprocessor= self.getMaterialHandler().getPreprocessor
            tangConcr= self.getConcreteDiagram(preprocessor).getTangent()
            GH= self.geomSection.getCenterOfMassHomogenizedSection(tangConcr) # Center of gravity of the homogenized section
            AH= self.geomSection.getAreaHomogenizedSection(tangConcr) # Area of the homogenized section
            IyH= self.geomSection.getIyHomogenizedSection(tangConcr) # Inertia tensor of homogenized section.
            IzH=  self.geomSection.getIzHomogenizedSection(tangConcr)
            PyzH= self.geomSection.getPyzHomogenizedSection(tangConcr)

            os.write('\\hline\n')
            os.write('\\begin{tabular}{ll}\n')
            os.write('$A_{homog.}='+cf.fmt6_3f.format(AH) +'\\ m^2$ & \\multirow{3}{*}{Inertia tensor ($cm^4$): $ \\left( \\begin{array}{ccc}'+ cf.fmt5_2f.format(JTorsion*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ cf.fmt5_2f.format(IyH*1e4) +' & '+cf.fmt5_2f.format(PyzH) +' \\\\ 0.00 & '+cf.fmt5_2f.format(PyzH) +' & '+cf.fmt5_2f.format(IzH*1e4)+' \\end{array} \\right)$} \\\\\n')
            os.write('& \\\\\n')
            os.write('C.O.G.: $('+cf.fmt5_3f.format(GH[0])+','+cf.fmt5_3f.format(GH[1])+')\\ m$  & \\\\\n')
            os.write('\\end{tabular} \\\\\n')
            os.write('\\hline\n')
            # Passive reinforcement.
            os.write('\\textbf{Passive reinforcement}:\\\\\n')
            os.write('\\hline\n')
            ## Main reinforcement.
            self.latexReportMainReinforcement(os)
            ## Shear reinforcement.
            os.write('\\hline\n')
            os.write('Layers of shear reinforcement:\\\\\n')
            os.write('\\hline\n')
            os.write('\\begin{tabular}{cccccccc}\n')
            os.write('Id & N$^o$ branch & $\\phi$ & area & spac. & area/m & $\\alpha$ & $\\beta$\\\\\n')
            os.write(' &  & $(mm)$ & $(cm^2)$ & $(cm)$ & $(cm^2/m^2)$ & $( \\degree)$ & $( \\degree)$\\\\\n')
            self.latexReportShearReinforcement(os)
            os.write('\\end{tabular} \\\\\n')
            os.write('\\hline\n')
            os.write('\\end{tabular}\n')
            os.write('\\end{center}\n')
            os.write('\\caption{'+self.sectionDescr+' ('+ latex_utils.toLaTeX(self.name) +').'+'} \\label{tb_'+self.name.replace(' ','_')+'}\n')
            os.write('\\end{table}\n')
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "; no section representation for section: '"+self.name+"'. Can't create report. Have you called defRCSection (or defRCSection2d) method?"
            lmsg.error(className+'.'+methodName+errMsg)
        if(clearRCSection):
            self.clearRCSection()
        return temporaryFiles

    def pdfReport(self, outputFileName:str= None, graphicWidth='70mm', showPDF= False, keepPDF= True, preprocessor= None, matDiagType= 'k'):
        ''' Write a report of the object in LaTeX format.

        :param outputFileName: name of the output file.
        :param graphicWidth: width for the cross-section graphic.
        :param showPDF: if true display the PDF output on the screen.
        :param keepPDF: if true don't remove the PDF output.
        :param preprocessor: pre-processor of the FE problem.
        :param matDiagType: diagram type; if "k" use the diagram 
                            corresponding to characteristic values of the 
                            material, if "d" use the design values one.
        '''
        retval= None
        if(showPDF or keepPDF):
            if(not outputFileName):
                outputFileName= self.gmSectionName()+'.tex'
            outputPath= '/tmp/'
            ltxIOString= io.StringIO()
            temporaryFiles= self.latexReport(os= ltxIOString, graphicWidth= graphicWidth, outputPath= outputPath, includeGraphicsPath= outputPath, preprocessor= preprocessor, matDiagType= matDiagType)
            # Compile LaTeX document.
            if(temporaryFiles):
                ltxIOString.seek(0)
                ltxString= ltxr.rc_section_report_latex_header+str(ltxIOString.read())+ltxr.rc_section_report_latex_tail
                ltxIOString.close()
                pdfFile= ltxr.latex_string_to_pdf(texString= str(ltxString), outputFileName= outputFileName, showPDF= showPDF)
                # Remove temporary files
                ## cross-section graphics.
                for f in temporaryFiles:
                    f.unlink()
                ## LaTeX source file
                Path(outputPath+outputFileName).unlink()
                if(showPDF):
                    input("Press Enter to continue...")
                if(keepPDF): # return the PDF file name for later use.
                    retval= pdfFile
                else: # remove PDF file.
                    if os.path.exists(pdfFile):
                        os.remove(pdfFile)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; latexReport returned nothing.')                     
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; both showPDF and keepPDF are false; nothing to do.')
        return retval
        
    def writeDXF(self, modelSpace, concreteLayerName= 'concrete', reinforcementLayerName= 'reinforcement'):
        ''' Writes the shape contour in the model
            space argument.

        :param modelSpace: ezdxf model space to write into.
        :param concretLayerName: DXF layer name for concrete material.
        :param reinforcementLayerName: DXF layer name for steel material.
        '''
        # Draw contour
        # Retrieve section geometry definition.
        if(self.fiberSectionRepr):
            geomSection= self.getSectionGeometry()
            write_dxf(geomSection= geomSection, modelSpace= modelSpace, concreteLayerName= concreteLayerName, reinforcementLayerName= reinforcementLayerName)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; no fiber section representation. Have you called defRCSection (or defRCSection2d) method?')
            
        
def write_dxf(geomSection, modelSpace, concreteLayerName= 'concrete', reinforcementLayerName= 'reinforcement'):
    ''' Writes the shape contour in the given DXF model space.

    :param modelSpace: ezdxf model space to write into.
    :param concretLayerName: DXF layer name for concrete material.
    :param reinforcementLayerName: DXF layer name for steel material.
    '''
    regions= geomSection.getRegions
    for r in regions:
        vertices= r.getPolygon().getVertexList()
        points= list()
        for v in vertices:
            points.append((v.x,v.y,0.0))
        points.append(points[0]) # close region.
        modelSpace.add_lwpolyline(points, dxfattribs={"layer": concreteLayerName})
    # Draw reinforcement.
    reinforcement= geomSection.getReinfLayers
    for reinfLayer in reinforcement:
        rebars= reinfLayer.getReinfBars
        for b in rebars:
            ptPlot= b.getPos2d
            rPlot= b.diameter/2.0
            #labelPlot= str(int(round(b.diameter*1e3)))
            modelSpace.add_circle(center= (ptPlot.x, ptPlot.y), radius= rPlot, dxfattribs={"layer": reinforcementLayerName})
    

class BasicRectangularRCSection(RCSectionBase, section_properties.RectangularSection):
    '''
    Base class for rectangular reinforced concrete sections.

    :ivar shReinfZ:        object of type ShearReinforcement
                           defining the shear reinforcement in Z direction
    :ivar shReinfY:        object of type ShearReinforcement
                           defining the shear reinforcement in Y direction
    :ivar torsionReinf:    object of type TorsionReinforcement
                           defining the torsion reinforcement.
    :ivar swapReinforcementAxes: if true, swap the axes of reinforcement so the positive and negative reinforcement rows are placed rotated 90 degrees.
    '''
    def __init__(self, name= None, sectionDescr= None, width=0.25,depth=0.25,concrType=None, reinfSteelType=None, nDivIJ= 10, nDivJK= 10, swapReinforcementAxes= False):
        ''' Constructor.

        :param name: name of the section     
        :param sectionDescr: section description.
        :param width: section width.
        :param depth: section depth.
        :param concrType: type of concrete (e.g. EHE_materials.HA25)     
        :param reinfSteelType: type of reinforcement steel.
        :param swapReinforcementAxes: if true, swap the axes of reinforcement so the positive and negative reinforcement rows are placed rotated 90 degrees.
        '''
        RCSectionBase.__init__(self, sectionDescr= sectionDescr, concrType= concrType, reinfSteelType= reinfSteelType, nDivIJ= nDivIJ, nDivJK= nDivJK)
        section_properties.RectangularSection.__init__(self, name= name, b= width, h= depth)

            
        self.swapReinforcementAxes= swapReinforcementAxes
        # Transverse reinforcement (z direction)
        self.shReinfZ= ShearReinforcement(familyName= 'Vz')        

        # Transverse reinforcement (y direction)
        self.shReinfY= ShearReinforcement(familyName= 'Vy')
        # Torsion reinforcement.
        self.torsionReinf= TorsionReinforcement(familyName= 'T')
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        # retval= super(BasicRectangularRCSection, self).getDict()
        # retval.update(section_properties.RectangularSection.getDict(self))
        retval= dict()
        retval['rc_section_base']= RCSectionBase.getDict(self)
        retval['section_properties']= section_properties.RectangularSection.getDict(self)
        retval['swap_reinforcement_axes']= self.swapReinforcementAxes
        retval['shear_reinforcement_z']= self.shReinfZ
        retval['shear_reinforcement_y']= self.shReinfY
        retval['torsion_reinforcement']= self.torsionReinf
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        # super(BasicRectangularRCSection, self).setFromDict(dct)
        # section_properties.RectangularSection.setFromDict(self, dct)
        RCSectionBase.setFromDict(self, dct['rc_section_base'])
        section_properties.RectangularSection.setFromDict(self, dct['section_properties'])
        self.swapReinforcementAxes= dct['swap_reinforcement_axes']
        self.shReinfZ= dct['shear_reinforcement_z']
        self.shReinfY= dct['shear_reinforcement_y']
        self.torsionReinf= dct['torsion_reinforcement']
                
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(BasicRectangularRCSection, self).__eq__(other)
                if(retval):
                    retval= section_properties.RectangularSection.__eq__(self, other)
                if(retval):
                    retval= (self.shReinfZ== other.shReinfZ)
                if(retval):
                    retval= (self.shReinfY== other.shReinfY)
                if(retval):
                    retval= (self.torsionReinf== other.torsionReinf)
            else:
                retval= True
        else:
            retval= False
        return retval

    def getCopy(self):
        ''' Returns a deep enough copy of the object.'''
        retval= BasicRectangularRCSection(name= self.name, sectionDescr= self.sectionDescr, concrType= self.getConcreteType(), reinfSteelType= self.getReinfSteelType(), width= self.b, depth= self.h, nDivIJ= self.getNDivIJ(), nDivJK= self.getNDivJK())
        return retval

    def getShearReinfY(self):
        '''Return the shear reinforcement for Vy.'''
        retval= self.shReinfY
        if(self.swapReinforcementAxes):
            retval= self.shReinfZ
        return retval

    def getShearReinfZ(self):
        '''Return the shear reinforcement for Vz.'''
        retval= self.shReinfZ
        if(self.swapReinforcementAxes):
            retval= self.shReinfY
        return retval

    def getShearReinforcementArea(self):
        '''Return the total shear reinforcement area by member unit length.'''
        retval= self.getShearReinfY().getAs()
        retval+= self.getShearReinfZ().getAs()
        return retval

    def getTorsionReinforcementArea(self):
        '''Return the total torsion reinforcement area by member unit length.'''
        return self.torsionReinf.getAs()

    def getTotalReinforcementArea(self):
        ''' Return the total reinforcement area by member unit length.'''
        retval= self.getMainReinforcementArea()
        retval+= self.getShearReinforcementArea()
        retval+= self.getTorsionReinforcementArea()
        return retval
        
    def getW1z(self):
        ''' Section modulus of the gross section with respect to the most tensioned fiber.'''
        b= self.getWidth()
        h= self.getDepth()
        return b*h**2/6.0
        
    def getW1y(self):
        ''' Section modulus of the gross section with respect to the most tensioned fiber.'''
        b= self.getWidth()
        h= self.getDepth()
        return h*b**2/6.0

    def getW1(self, bendingAxis= 'z'):
        ''' Section modulus of the gross section with respect to the most tensioned fiber.

        :param bendingAxis: bending axis ('z' or 'y').
        '''
        retval= None
        if(bendingAxis=='z'):
            retval= self.getW1z()
        elif(bendingAxis=='y'):
            retval= self.getW1y()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= className+'.'+methodName+"; bending axis must be 'z' or 'y'"
            lmsg.error(className+'.'+methodName+errMsg)
        return retval
    
    def getDepth(self):
        ''' Return the depth of the section that corresponds to the reinforcement axes: (width if the reinforcement axes are swapped).'''
        retval= self.h
        if(self.swapReinforcementAxes):
            retval= self.b
        return retval
        
    def getWidth(self):
        ''' Return the width of the section that corresponds to the reinforcement axes: (depth if the reinforcement axes are swapped).'''
        retval= self.b
        if(self.swapReinforcementAxes):
            retval= self.h
        return retval
    
    def getRespT(self, preprocessor):
        '''Material for modeling torsional response of section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.getRespT(self, preprocessor= preprocessor, G= self.fiberSectionParameters.concrType.Gcm()) # Torsional response of the section.

    def getRespVy(self, preprocessor):
        '''Material for modeling Y shear response of section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.getRespVy(self, preprocessor, self.fiberSectionParameters.concrType.Gcm())

    def getRespVz(self, preprocessor):
        '''Material for modeling Z shear response of section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.getRespVz(self, preprocessor, self.fiberSectionParameters.concrType.Gcm())

    def getContour(self):
        ''' Return the vertices of the section contour.'''
        pMin= geom.Pos2d(-self.b/2,-self.h/2)
        pMax= geom.Pos2d(self.b/2,self.h/2)
        vertices= [pMin, geom.Pos2d(pMax.x, pMin.y), pMax, geom.Pos2d(pMin.x, pMax.y), pMin]
        return vertices

    def defConcreteRegion(self):
        ''' Define a rectangular region filled with concrete.
        '''
        regions= self.geomSection.getRegions
        rg= regions.newQuadRegion(self.fiberSectionParameters.concrDiagName)
        rg.nDivIJ= self.fiberSectionParameters.nDivIJ
        rg.nDivJK= self.fiberSectionParameters.nDivJK
        rg.pMin= geom.Pos2d(-self.b/2,-self.h/2)
        rg.pMax= geom.Pos2d(self.b/2,self.h/2)

    def clearConcreteRegions(self):
        ''' Clear previously defined concrete regions.'''
        if(self.geomSection):
            regions= self.geomSection.getRegions
            regions.clear()
    
    def defElasticSection1d(self, preprocessor, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for truss analysis.

        :param preprocessor: preprocessor of the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticSection1d(preprocessor, material= mat, overrideRho= overrideRho, reductionFactor= reductionFactor)
    
    def defElasticSection3d(self, preprocessor, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for 3D beam analysis

        :param preprocessor: preprocessor of the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticSection3d(preprocessor, material= mat, overrideRho= overrideRho, reductionFactor= reductionFactor)
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None, reductionFactor= 1.0):
        '''elastic section appropriate for 3D beam analysis, including shear 
           deformations

        :param preprocessor: XC preprocessor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
         '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticShearSection3d(preprocessor, material= mat, overrideRho= overrideRho, reductionFactor= reductionFactor)
    
    def defElasticSection2d(self, preprocessor, majorAxis= True, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for 2D beam analysis

        :param preprocessor: XC preprocessor for the finite element problem.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticSection2d(preprocessor, material= mat, majorAxis= majorAxis, overrideRho= overrideRho, reductionFactor= reductionFactor)
        
    def defElasticShearSection2d(self, preprocessor, majorAxis= True, overrideRho= None, reductionFactor= 1.0):
        '''elastic section appropriate for 2D beam analysis, including 
           shear deformations.

        :param preprocessor: XC preprocessor for the finite element problem.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticShearSection2d(preprocessor, material= mat, majorAxis= majorAxis, overrideRho= overrideRho, reductionFactor= reductionFactor)

    def defElasticMembranePlateSection(self, preprocessor, overrideRho= None, reductionFactor= 1.0):
        '''Constructs an elastic isotropic section material appropriate 
           for plate and shell analysis.

        :param preprocessor: XC preprocessor of the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticMembranePlateSection(preprocessor= preprocessor, material= mat, overrideRho= overrideRho, reductionFactor= reductionFactor)
    
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        section_properties.RectangularSection.report(self, os)
        super(BasicRectangularRCSection, self).report(os, indentation)
        indentation+= '  '
        os.write(indentation+'Transverse reinforcement (z direction):\n')
        self.getShearReinfZ().report(os, indentation+'  ')
        os.write(indentation+'Transverse reinforcement (y direction):\n')
        self.getShearReinfY().report(os, indentation+'  ')

    def latexReportGeometry(self, os= sys.stdout):
        ''' Write geometry data in LaTeX format.

        :param os: output stream.
        '''
        os.write('\\begin{tabular}{l}\n')
        os.write('width: \\\\\n')
        os.write('$b= '+'{0:.2f}'.format(self.b)+'\\ m$\\\\\n')
        os.write('depth: \\\\\n')
        os.write('$h= '+'{0:.2f}'.format(self.h)+'\\ m$\\\\\n')
        os.write('\\end{tabular} \\\\\n')

    def latexReportShearReinforcement(self, os= sys.stdout):
        ''' Write shear reinforcement report in LaTeX format.

        :param os: output stream.
        '''
        self.getShearReinfY().latexReport(width= self.b, os= os)
        self.getShearReinfZ().latexReport(width= self.h, os= os)       

class RCRectangularSection(BasicRectangularRCSection):
    ''' This class is used to define the variables that make up a reinforced 
        concrete section with top and bottom reinforcement layers.

    :ivar minCover:        minimum value of end or clear concrete cover of main 
                           bars from both the positive and negative faces
    :ivar negatvRebarRows: layers of main rebars in the local negative face of 
                           the section
    :ivar positvRebarRows: layers of main rebars in the local positive face of 
                           the section
    '''
    
    def __init__(self, name= None, sectionDescr= None, width=0.25, depth=0.25, concrType= None, reinfSteelType= None, nDivIJ= 10, nDivJK= 10, swapReinforcementAxes= False):
        ''' Constructor.

        :param name: name of the section 
        :param sectionDescr: section description.
        :param width: section width.
        :param depth: section depth.
        :param concrType: type of concrete (e.g. EHE_materials.HA25)     
        :param reinfSteelType: type of reinforcement steel.
        :param swapReinforcementAxes: if true, swap the axes of reinforcement so the positive and negative reinforcement rows are placed rotated 90 degrees.
        '''
        super(RCRectangularSection,self).__init__(name= name, sectionDescr= sectionDescr, width= width, depth= depth, concrType= concrType, reinfSteelType= reinfSteelType, nDivIJ= nDivIJ, nDivJK= nDivJK, swapReinforcementAxes= swapReinforcementAxes)
            
        # Longitudinal reinforcement
        self.minCover= 0.0 
        self.positvRebarRows= LongReinfLayers() # list of ReinfRow data (positive face)
        self.negatvRebarRows= LongReinfLayers() # list of ReinfRow data (negative face)
        
    def getDict(self):
        ''' Return a dictionary with the values of the object members.'''
        retval= super().getDict()
        retval.update({'min_cover':self.minCover, 'positive_rebar_rows':self.positvRebarRows, 'negative_rebar_rows':self.negatvRebarRows})
        return retval

    def setFromDict(self, dct):
        ''' Set the member values from those in the given dictionary.'''
        super().setFromDict(dct)
        self.minCover= dct['min_cover']
        self.positvRebarRows= dct['positive_rebar_rows']
        self.negatvRebarRows= dct['negative_rebar_rows']
            
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(RCRectangularSection, self).__eq__(other)
                if(retval):
                    retval= (self.minCover == other.minCover)
                if(retval):
                    retval= (self.shReinfZ == other.shReinfZ)
                if(retval):
                    retval= (self.shReinfY == other.shReinfY)
                if(retval):
                    retval= (self.positvRebarRows == other.positvRebarRows)
                if(retval):
                    retval= (self.negatvRebarRows == other.negatvRebarRows)
            else:
                retval= True
        else:
            retval= False
        return retval

    def flipReinforcement(self):
        ''' Flip the reinforcement top<-->bottom.'''
        self.positvRebarRows, self.negatvRebarRows= self.negatvRebarRows, self.positvRebarRows

    def getCopy(self):
        ''' Returns a deep enough copy of the object.'''
        retval= RCRectangularSection(name= self.name, sectionDescr= self.sectionDescr, concrType= self.getConcreteType(), reinfSteelType= self.getReinfSteelType(), width= self.b, depth= self.h, nDivIJ= self.getNDivIJ(), nDivJK= self.getNDivJK())
        return retval

    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        super(RCRectangularSection, self).report(os, indentation)
        indentation+= '  '
        os.write(indentation+'Bottom reinforcement: \n')
        self.positvRebarRows.report(os, indentation+'  ')
        os.write(indentation+'Top reinforcement: \n')
        self.negatvRebarRows.report(os, indentation+'  ')
        
    def appendRow(self, positiveReinf, rebarRow:ReinfRow):
        ''' Constructs append the giver ReinfRow to the reinforcement in the 
            positive (positiveReinf==True) or negative side of the section.

        :param positiveReinf: if true append to positive reinforcement otherwise
                              to negative reinforcement.
        :rebarRow: row of reinforcement bars.
        '''
        if(positiveReinf):
            self.positvRebarRows.append(rebarRow)
        else:
            self.negatvRebarRows.append(rebarRow)
        
    def appendPositiveRow(self, rebarRow:ReinfRow):
        ''' Constructs append the giver ReinfRow to the reinforcement in the 
            positive side of the section.

        :rebarRow: row of reinforcement bars.
        '''
        self.appendRow(positiveReinf= True, rebarRow= rebarRow)
        
    def appendNegativeRow(self, rebarRow:ReinfRow):
        ''' Constructs append the giver ReinfRow to the reinforcement in the 
            negative side of the section.

        :rebarRow: row of reinforcement bars.
        '''
        self.appendRow(positiveReinf= False, rebarRow= rebarRow)

    def appendLayer(self, positiveReinf, nominalCover, nRebars= None, rebarsDiam= None, nominalLatCover= None):
        ''' Constructs a ReinfRow with the given data and appends it to the
            reinforcement in the positive side of the section.

        :param positiveReinf: if true append to positive reinforcement otherwise
                              to negative reinforcement.
        :param nominalCover: nominal cover [m]
        :param nRebars: number of rebars, if None pick the last one.
        :param rebarsDiam: bar diameter [m], if None pick the last one.
        :param nominalLatCover: nominal lateral cover [m], if None pick the last one.
        '''
        if(nRebars is None): # if not specified.
            nBar= self.getNBar()
            if(nBar): # not empty.
                nRebars= nBar[-1] # pick the last one.
        if(rebarsDiam is None): # if not specified.
            diameters= self.getDiameters()
            if(diameters): # not empty.
                rebarsDiam= diameters[1] # pick the last one.
        if(nominalLatCover is None): # if not specified.
            nominalLatCovers= self.getNominalLatCover()
            if(nominalLatCovers):# not empty.
                nominalLatCover= nominalLatCovers[-1] # pick the last one.
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; lateral cover not defined.')
        if(self.swapReinforcementAxes):
            rowWidth= self.h
        else:
            rowWidth= self.b
        rr= ReinfRow(rebarsDiam= rebarsDiam, nRebars= nRebars, width= rowWidth, nominalCover= nominalCover, nominalLatCover= nominalLatCover)
        self.appendRow(positiveReinf= positiveReinf, rebarRow= rr)
        return rr
    
    def appendPositiveLayer(self, nominalCover, nRebars= None, rebarsDiam= None, nominalLatCover= None):
        ''' Constructs a ReinfRow with the given data and appends it to the
            reinforcement in the positive side of the section.

        :param nominalCover: nominal cover [m]
        :param nRebars: number of rebars, if None pick the last one.
        :param rebarsDiam: bar diameter [m], if None pick the last one.
        :param nominalLatCover: nominal lateral cover [m], if None pick the last one.
        '''
        return self.appendLayer(positiveReinf= True, nominalCover= nominalCover, nRebars= nRebars, rebarsDiam= rebarsDiam, nominalLatCover= nominalLatCover)
        
    def appendNegativeLayer(self, nominalCover, nRebars= None, rebarsDiam= None, nominalLatCover= None):
        ''' Constructs a ReinfRow with the given data and appends it to the
            reinforcement in the positive side of the section.

        :param nominalCover: nominal cover [m]
        :param nRebars: number of rebars, if None pick the last one.
        :param rebarsDiam: bar diameter [m], if None pick the last one.
        :param nominalLatCover: nominal lateral cover [m], if None pick the last one.
        '''
        return self.appendLayer(positiveReinf= False, nominalCover= nominalCover, nRebars= nRebars, rebarsDiam= rebarsDiam, nominalLatCover= nominalLatCover)
        
    def getAsPos(self):
        '''returns the cross-sectional area of the rebars in the positive face.'''
        return self.positvRebarRows.getAs()

    def getPosRowsCGcover(self):
        '''returns the distance from the center of gravity of the positive rebars
        to the positive face of the section.
        '''
        return self.positvRebarRows.getRowsCGcover()
    
    def hAsPos(self):
        '''Return the distance from the bottom fiber to the 
        centre of gravity of the rebars in the positive face.
        '''
        return self.getPosRowsCGcover()
    
    def getYAsPos(self):
        '''returns the local Y coordinate of the center of gravity of the rebars
           in the positive face.
        '''
        return self.h/2.0-self.getPosRowsCGcover()

    def getAsNeg(self):
        '''returns the cross-sectional area of the rebars in the negative face'''
        return self.negatvRebarRows.getAs()

    def getNegRowsCGcover(self):
        '''returns the distance from the center of gravity of the negative rebars
        to the negative face of the section 
        '''
        return self.negatvRebarRows.getRowsCGcover()

    def hAsNeg(self):
        '''Return the distance from the bottom fiber to the 
        centre of gravity of the rebars in the positive face.
        '''
        return self.h-self.getNegRowsCGcover()
    
    def getYAsNeg(self):
        '''returns the local Y coordinate of the center of gravity of the rebars
           in the negative face
        '''
        return -self.h/2.0+self.getNegRowsCGcover()

    def getAc(self):
        '''Returns the cross-sectional area of the section'''
        return self.b*self.h

    def getAreaHomogenizedSection(self):
        '''Return the area of the homogenized section.'''
        retval= self.getAc()
        n= self.getHomogenizationCoefficient()
        retval+= n*(self.getAsNeg()+self.getAsPos())
        return retval
    
    def hCOGHomogenizedSection(self):
        '''Return the distance from the bottom fiber to the 
        centre of gravity of the homogenized section.
        '''
        retval= self.h/2.0*self.getAc()
        n= self.getHomogenizationCoefficient()
        retval+= self.hAsPos()*n*self.getAsPos()
        retval+= self.hAsNeg()*n*self.getAsNeg()
        retval/=self.getAreaHomogenizedSection()
        return retval
    
    def getRoughVcuEstimation(self):
        '''returns a minimal value (normally shear strength will be greater)
           of the shear strength of the concrete section Vcu expressed
           in newtons.'''
        return 0.5e6*self.getAc() # 0.5 MPa
      
    def getI(self):
        ''' Returns the second moment of area about the middle axis parallel to 
        the width '''
        return 1/12.0*self.b*self.h**3

    def getPosReinforcementIz(self, hCOG, n= 1.0):
        ''' Return the second moment of inertia of the reinforcement in the
            positive side.

        :param hCOG: distance from the section bottom to its center of gravity.
        :param n: homogenizatrion coefficient.
        '''
        retval= 0.0
        for row in self.positvRebarRows:
            retval+= row.getI()
        retval*= n
        d= self.hAsPos()-hCOG
        retval+= (n-1)*self.getAsPos()*d**2 # Steiner.
        return retval
        
    def getNegReinforcementIz(self, hCOG, n= 1.0):
        ''' Return the second moment of inertia of the reinforcement in the
            negative side.

        :param hCOG: distance from the section bottom to its center of gravity.
        :param n: homogenizatrion coefficient.
        '''
        retval= 0.0
        for row in self.negatvRebarRows:
            retval+= row.getI()
        retval*= n
        d= self.hAsNeg()-hCOG
        retval+= (n-1)*self.getAsNeg()*d**2 # Steiner.
        return retval

    def getIzHomogenizedSection(self):
        '''returns the second moment of area about the axis parallel to 
        the section width through the center of gravity'''
        retval= self.getI() # Moment of inertia of the concrete section.
        # Position of the centroid.
        hCOGH= self.hCOGHomogenizedSection()
        # Eccentricity of the concrete section.
        d= self.hCOG()-hCOGH # distance from center of gravity.
        retval+= self.getAc()*d**2 # Steiner.
        # Moment of inertia of the reinforcement.
        n= self.getHomogenizationCoefficient()
        # Reinforcement on the possitive side.
        retval+= self.getPosReinforcementIz(hCOG= hCOGH, n= n)
        # Reinforcement on the possitive side.
        retval+= self.getNegReinforcementIz(hCOG= hCOGH, n= n)
        return retval
       
    def getIyHomogenizedSection(self):
        '''returns the second moment of area about the axis parallel to 
        the section depth through the center of gravity'''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        errMsg= '; not implemented yet.'
        lmsg.error(className+'.'+methodName+errMsg)
        # Need to compute the steel distribution along the z axis.
        return None
    
    def iyHomogenizedSection(self):
        '''Return the radius of gyration of the section around
           the axis parallel to the section depth that passes 
           through section centroid.
        '''
        return math.sqrt(self.getIyHomogenizedSection()/self.getAreaHomogenizedSection())
    
    def getIz_RClocalZax(self):
        '''returns the second moment of area about the middle axis parallel to 
        the width (RClocalZaxis)'''
        return 1/12.0*self.b*self.h**3

    def getIy_RClocalYax(self):
        '''returns the second moment of area about the middle axis parallel to 
        the depth (RClocalYaxis)'''
        return 1/12.0*self.h*self.b**3

    def getSPos(self):
        '''returns a list with the distance between bars for each row of bars in 
        local positive face.'''
        return self.positvRebarRows.getSpacings()

    def getSNeg(self):
        '''returns a list with the distance between bars for each row of bars in local negative face.'''
        return self.negatvRebarRows.getSpacings()

    def getDiamPos(self):
        '''returns a list with the bar diameter for each row of bars in local 
        positive face.'''
        return self.positvRebarRows.getDiameters()

    def getDiamNeg(self):
        '''returns a list with the bar diameter for each row of bars in local 
        negative face.'''
        return self.negatvRebarRows.getDiameters()

    def getDiameters(self):
        '''returns a list with the bar diameter for each row of bars.'''
        retval= self.getDiamPos()
        retval.extend(self.getDiamNeg())
        return retval

    def getNBarPos(self):
        '''returns a list with the number of bars for each row of bars in local 
        positive face.'''
        return self.positvRebarRows.getNBar()

    def getNBarNeg(self):
        '''returns a list with the number of bars for each row of bars in local 
        negative face.'''
        return self.negatvRebarRows.getNBar()

    def getNBar(self):
        '''returns a list with the number of bars for each row of bars.'''
        retval= self.positvRebarRows.getNBar()
        retval.extend(self.negatvRebarRows.getNBar())
        return retval

    def getCoverPos(self):
        '''returns a list with the cover of bars for each row of bars in local 
        positive face.'''
        return self.positvRebarRows.getCover()

    def getCoverNeg(self):
        '''returns a list with the cover of bars for each row of bars in local 
        negative face.'''
        return self.negatvRebarRows.getCover()

    def getCover(self):
        '''returns a list with the cover of bars for each row of bars in local 
        positive face.'''
        retval= self.getCoverPos()
        retval.extend(self.getCoverNeg())
        return retval

    def getLatCoverPos(self):
        '''returns a list with the lateral cover of bars for each row of bars in local positive face.'''
        return self.positvRebarRows.getLatCover()

    def getLatCoverNeg(self):
        '''returns a list with the lateral cover of bars for each row of bars in 
        local negative face.'''
        return self.negatvRebarRows.getLatCover()
    
    def getLatCover(self):
        '''returns a list with the lateral cover of bars for each row of bars.
        '''
        retval= self.positvRebarRows.getLatCover()
        retval.extend(self.negatvRebarRows.getLatCover())
        return retval
    
    def getNominalLatCover(self):
        '''returns a list with the lateral cover of bars for each row of bars.
        '''
        retval= self.positvRebarRows.getNominalLatCover()
        retval.extend(self.negatvRebarRows.getNominalLatCover())
        return retval

    def centerRebarsPos(self):
        '''centers in the width of the section the rebars placed in the positive face''' 
        return self.positvRebarRows.centerRebars(self.b)

    def centerRebarsNeg(self):
        '''centers in the width of the section the rebars placed in the negative 
        face''' 
        return self.negatRebarRows.centerRebars(self.b)

    def getMinCover(self):
        ''' return the minimal cover of the reinforcement.'''
        retval= 1e6
        posCover= self.getCoverPos()
        if(len(posCover)>0):
            retval= min(retval, min(posCover))
        negCover= self.getCoverNeg()
        if(len(negCover)>0):
            retval= min(retval, min(negCover))
        latPosCover= self.getLatCoverPos()
        if(len(latPosCover)>0):
            retval= min(retval, min(latPosCover))
        latNegCover= self.getLatCoverNeg()
        if(len(latNegCover)>0):
            retval= min(retval, min(latNegCover))
        if(retval==1e6): # Reinforcement not defined.
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; reinforcement not defined, returning default value.')
            retval= 0.035
        return retval

    def computeRebarPositions(self):
        ''' Compute the positions of the reinforcement.'''
        # Placement of the negative reinforcement.
        negPoints= list()
        halfDepth= self.h/2.0
        halfWidth= self.b/2.0
        if(self.swapReinforcementAxes):
            halfDepth, halfWidth= halfWidth, halfDepth
        ## Compute positions.
        for rbRow in self.negatvRebarRows.rebarRows:
            y= -halfDepth+rbRow.cover
            z1= -halfWidth+rbRow.latCover
            z2= halfWidth-rbRow.latCover
            if(self.swapReinforcementAxes):
                p1= geom.Pos2d(-y, z1)
                p2= geom.Pos2d(-y, z2)
            else:
                p1= geom.Pos2d(z1, y)
                p2= geom.Pos2d(z2, y)
            negPoints.append((p1,p2))
        # Placement of the positive reinforcement.
        posPoints= list()
        ## Compute positions.
        for rbRow in self.positvRebarRows.rebarRows:
            y= halfDepth-rbRow.cover
            z1= -halfWidth+rbRow.latCover
            z2= halfWidth-rbRow.latCover
            if(self.swapReinforcementAxes):
                p1= geom.Pos2d(-y, z1)
                p2= geom.Pos2d(-y, z2)
            else:
                p1= geom.Pos2d(z1, y)
                p2= geom.Pos2d(z2, y)
            posPoints.append((p1,p2))
        return negPoints, posPoints

    def defSectionGeometry(self, preprocessor, matDiagType):
        '''
        Define the XC section geometry object for a reinforced concrete section 

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                            ("k" for characteristic diagram, "d" for design diagram)
        '''
        self.defDiagrams(preprocessor, matDiagType)
        self.geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion()
        reinforcement= self.geomSection.getReinfLayers
        # Placement of the negative reinforcement.
        ## Compute positions.
        negPoints, posPoints= self.computeRebarPositions()
        ## Create reinforcement layers.
        self.negatvRebarRows.defStraightLayers(reinforcement,"neg",self.fiberSectionParameters.reinfDiagName,negPoints)
        self.positvRebarRows.defStraightLayers(reinforcement,"pos",self.fiberSectionParameters.reinfDiagName,posPoints)
        self.minCover= self.getMinCover()

    def clearSectionGeometry(self):
        ''' Clear the XC section geometry object previously defined for this
            section.

        '''
        # Clear concrete region.
        self.clearConcreteRegions()
        if(self.geomSection):
            self.geomSection.clear()
            self.geomSection= None
        self.clearDiagrams()
        
    def getTorsionalThickness(self):
        '''Return the section thickness for torsion.'''
        return min(self.b,self.h)/2.0
    
    def getStressCalculator(self):
        Ec= self.fiberSectionParameters.concrType.Ecm()
        Es= self.fiberSectionParameters.reinfSteelType.Es
        return sc.StressCalc(self.b,self.h,self.getPosRowsCGcover(),self.getNegRowsCGcover(),self.getAsPos(),self.getAsNeg(),Ec,Es)

    def defineMainReinforcement(self, nominalCover, fiStirr, topLayersDiameters= [12e-3, None], bottomLayersDiameters= [12e-3, None], lateralLayersDiameters= [10e-3, None], nRebarsHoriz= 3, nRebarsVert= 2):
        ''' Define the reinforcement of the given RC section.

        :param nominalCover: nominal concrete cover.
        :param fiStirr: diameter of the stirrups.
        :param topLayersDiameters: diameters of the first and second top reinforcement layers (if None => no layer).
        :param bottomLayersDiameters: diameters of the first and second bottom reinforcement layers (if None => no layer).
        :param lateralLayersDiameters: diameters of the exterior and interior lateral reinforcement layers (if diameter[i]==None => no i-th layer).
        :param nRebarsHoriz: number of horizontal rebars.
        :param nRebarsVert: number of vertical rebars.
        '''
        # Top face.
        firstTopLayer= self.appendPositiveLayer(nominalCover= nominalCover+fiStirr, nRebars= nRebarsHoriz, rebarsDiam= topLayersDiameters[0], nominalLatCover= nominalCover+fiStirr)
        if(topLayersDiameters[1]):
            newNominalCover= nominalCover+fiStirr+topLayersDiameters[0]+max(topLayersDiameters[1], 25e-3)
            secondTopLayer= self.appendPositiveLayer(nominalCover= newNominalCover, rebarsDiam= topLayersDiameters[1])
        else:
            secondTopLayer= None
        # Bottom face.
        firstBottomLayer= self.appendNegativeLayer(nominalCover= nominalCover+fiStirr, rebarsDiam= bottomLayersDiameters[0])
        if(bottomLayersDiameters[1]):
            newNominalCover= nominalCover+fiStirr+bottomLayersDiameters[0]+max(bottomLayersDiameters[1], 25e-3)
            secondBottomLayer= self.appendNegativeLayer(nominalCover= newNominalCover, rebarsDiam= bottomLayersDiameters[1])
        else:
            secondBottomLayer= None
        # Lateral reinforcement.
        covers= self.getCover()
        latTopCover= covers[0]+covers[1]+firstTopLayer.rebarsDiam
        if(self.swapReinforcementAxes):
            rowWidth= self.b
        else:
            rowWidth= self.h
        sLat=(rowWidth-2*latTopCover)/(nRebarsVert-1) # spacing.
        for i in range(nRebarsVert):
            self.appendPositiveLayer(nRebars= 2, rebarsDiam= lateralLayersDiameters[0], nominalCover= latTopCover+i*sLat)
        if(lateralLayersDiameters[1]):
            for i in range(nRebarsVert):
                self.appendPositiveLayer(nRebars= 2, rebarsDiam= lateralLayersDiameters[0], nominalCover= latTopCover+i*sLat, nominalLatCover= 3*nominalCover+fiStirr)
        return firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer
    
    def defineShearReinforcementYZ(self, nShReinfBranchesY= 2, fiStirrY= 8e-3, spacingY= 0.15, nShReinfBranchesZ= 2, fiStirrZ= 8e-3, spacingZ= 0.15):
        ''' Define the shear reinforcement of the RC section.

        :param nShReinfBranchesY: number of branches (Y direction).
        :param fiStirrY: diameter of the stirrups (Y direction).
        :param spacingY: stirrups spacing (Y direction).
        :param nShReinfBranchesZ: number of branches (Z direction).
        :param fiStirrZ: diameter of the stirrups (Z direction).
        :param spacingZ: stirrups spacing (Z direction).
        '''
        # Shear reinforcement.
        if(nShReinfBranchesY>0):
            self.shReinfY= ShearReinforcement(familyName= "sh1uy", nShReinfBranches= nShReinfBranchesY, areaShReinfBranch= math.pi*(fiStirrY)**2/4., shReinfSpacing= spacingY, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)
        if(nShReinfBranchesZ>0):
            self.shReinfZ= ShearReinforcement(familyName= "sh1z", nShReinfBranches= nShReinfBranchesZ, areaShReinfBranch= math.pi*(fiStirrZ)**2/4., shReinfSpacing= spacingZ, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

def compute_element_reinforcement(element):
    ''' Return a list containing the reinforced concrete sections from the
        properties defined in the elements argument. Those properties are:

        - baseSection: RCSectionBase derived object containing the geometry
                       and the material properties of the reinforcec concrete
                       section.
        - reinforcementUpVector: reinforcement "up" direction which defines
                                 the position of the positive reinforcement
                                 (bottom) and the negative reinforcement
                                 (up).
        - reinforcementIVector: (for slabs) direction corresponding to 
                                the first RC section
        - bottomReinforcement: LongReinfLayers objects defining the 
                               reinforcement at the bottom of the section.
        - topReinforcement: LongReinfLayers objects defining the 
                            reinforcement at the top of the section.
        - shearReinforcement: ShearReinforcement objects defining the 
                              reinforcement at the bottom of the section.
     
     :param element: element for which the reinforce concrete sections 
                     will be computed.
    '''
    reinforcementUpVector= element.getProp("reinforcementUpVector") # reinforcement "up" direction.
    baseSection= element.getProp('baseSection').getCopy()
    dim= element.getDimension
    if(dim==1):
        elementUpOrientation= element.getJVector3d(True) # initial geometry
        upOrientation= reinforcementUpVector.dot(elementUpOrientation)
        pR= element.getProp("bottomReinforcement")
        nR= element.getProp("topReinforcement")
        shR= None
        if(element.hasProp('shearReinforcement')):
           shR= element.getProp('shearReinforcement')
        if(upOrientation<0): # reverse position.
            pR, nR= nR, pR
        baseSection.positvRebarRows= pR
        baseSection.negatvRebarRows= nR
        if(shR):
            baseSection.shReinfY= shR
        retval= [baseSection, baseSection] # Section at element integration points.
    elif(dim==2):
        elementUpOrientation= element.getKVector3d(True) # initial geometry
        upOrientation= reinforcementUpVector.dot(elementUpOrientation)
        reinforcementIVector= element.getProp('reinforcementIVector') # direction of the reinforcement in the slab.
        elementIOrientation= element.getIVector3d(True) # initial geometry
        iOrientation= reinforcementIVector.dot(elementIOrientation)
        theta= reinforcementIVector.getAngle(elementIOrientation)
        pRI= element.getProp("bottomReinforcementI")
        nRI= element.getProp("topReinforcementI")
        shRI= None
        if(element.hasProp('shearReinforcementI')):
           shRI= element.getProp('shearReinforcementI')
        pRII= element.getProp("bottomReinforcementII")
        nRII= element.getProp("topReinforcementII")
        shRII= None
        if(element.hasProp('shearReinforcementII')):
            shRII= element.getProp('shearReinforcementII')
        if(abs(iOrientation)<0.7): # reverse reinforcement directions.
            pRI, pRII= pRII, pRI # positive reinforcement.
            nRI, nRII= nRII, nRI # negative reinforcement.
            shRI, shRII= shRII, shRI # shear reinforcement.
            theta-= math.pi/2.0
        if(upOrientation>0): # for 2D elements reverse top and bottom
                             # positions if dot product > 0.
            pRI, nRI= nRI, pRI # positive reinforcement.
            pRII, nRII= nRII, pRII # positive reinforcement.
            # shear reinforcement not affected.
        if((abs(iOrientation)>1e-3) and (abs(abs(iOrientation)-1.0)>1e-3)): # reinforcement not parallel nor perpendicular
            #element.setProp('theta', theta)
            pass
        baseSectionII= baseSection.getCopy()
        baseSection.name+= 'I'
        baseSection.positvRebarRows= pRI
        baseSection.negatvRebarRows= nRI
        if(shRI):
            baseSection.shReinfY= shRI
        baseSectionII.name+= 'II'
        baseSectionII.positvRebarRows= pRII
        baseSectionII.negatvRebarRows= nRII
        if(shRII):
            baseSectionII.shReinfY= shRII
        retval= [baseSection, baseSectionII]
    return retval
    
def compute_element_rc_sections(elements, propName= None):
    ''' Return a list containing the reinforced concrete sections computed
        from the values of the properties defined in the elements argument. 
        Those properties are:

        - baseSection: RCSectionBase derived object containing the geometry
                       and the material properties of the reinforcec concrete
                       section.
        - reinforcementUpVector: reinforcement "up" direction which defines
                                 the position of the positive reinforcement
                                 (bottom) and the negative reinforcement
                                 (up).
        - reinforcementIVector: (for slabs) direction corresponding to 
                                the first RC section
        - bottomReinforcement: LongReinfLayers objects defining the 
                               reinforcement at the bottom of the section.
        - topReinforcement: LongReinfLayers objects defining the 
                            reinforcement at the top of the section.
        - shearReinforcement: ShearReinforcement objects defining the 
                              reinforcement at the bottom of the section.
     
     :param elements: elements for which the reinforce concrete sections 
                      will be computed.
     :param propName: name of the property that stores the section names.
    '''
    retval= list()
    for el in elements:
        elementSections= compute_element_reinforcement(el)
        # Assign elements to each section.
        for i, eSection in enumerate(elementSections):
            if(eSection not in retval):
                eSection.elements= [(el.tag, i)]
                retval.append(eSection)
            else:
                idx= retval.index(eSection)
                retval[idx].elements.append((el.tag, i))
    # Rename the new sections.        
    for i, s in enumerate(retval):
        s.name+= str(i)
        s.sectionDescr+= ' '+str(i)

    if(not propName is None and (len(elements)>0)):
        # Assign the sections names to the elements
        preprocessor= elements[0].getDomain.getPreprocessor
        elemHandler= preprocessor.getElementHandler
        for elm in elements:
            elm.setProp(propName, ['','']) # Initialize property
        for sct in retval:
            for tple in sct.elements:
                # Each tuple has (element tag, section number).
                eTag= tple[0] # Element identifier.
                sectionIdx= tple[1] # Index of the sct section in the element.
                element= elemHandler.getElement(eTag)
                sectionNames= element.getProp(propName)
                if(sectionNames[sectionIdx]!=''):
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(methodName+'; element '+str(eTag) + ' has alreade section: '+sectionNames[sectionIdx])
                sectionNames[sectionIdx]= sct.name
                element.setProp(propName, sectionNames)
    return retval
