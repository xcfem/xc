# -*- coding: utf-8 -*-
''' Classes defining main reinforcement.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import sys
from misc_utils import log_messages as lmsg


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
            self.setSpacing(rebarsSpacing= rebarsSpacing, width= width, nominalLatCover= nominalLatCover)
            
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
            
    def clear(self):
        ''' Clears all the members of this object.'''
        self.rebarsDiam= None
        self.areaRebar= None
        self.rebarsSpacing= None
        self.nRebars= None
        self.width= None
        self.cover= None
        if(hasattr(self, 'latCover')):
            self.latCover= None
        if(hasattr(self, 'reinfLayer')):
            self.reinfLayer.clear()
            self.reinfLayer= None
        
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
        availableWidth= width-2*nominalLatCover-self.rebarsDiam
        self.rebarsSpacing= availableWidth/(nRebars-1)
        
    def setSpacing(self, rebarsSpacing:float, width:float, nominalLatCover:float):
        ''' Set the space between rebar axes.

        :param rebarsSpacing: spacing between bars.
        :param width: width occupied by the rebars.
        :param nominalLatCover: nominal lateral cover.
        '''
        self.rebarsSpacing= rebarsSpacing
        availableWidth= width-2*nominalLatCover-self.rebarsDiam
        nRebarsTeor= availableWidth/rebarsSpacing+1
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
      
    def centerRebars(self, sectionWidth):
        '''center the row of rebars in the width of the section

        :param sectionWidth: section width.
        '''
        rowWidth= (self.nRebars-1)*self.rebarsSpacing
        self.latCover= (sectionWidth-rowWidth)/2.0

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
        for i, (rbRow, pts) in enumerate(zip(self.rebarRows, pointPairs)):
            p1= pts[0]; p2= pts[1]
            rowLayerCode= layerCode+'_'+str(i)
            self.reinfLayers.append(rbRow.defStraightLayer(reinforcement= reinforcement, layerCode= rowLayerCode, diagramName= diagramName, p1= p1, p2= p2))
            
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
        for reinfLayer in self.reinfLayers:
            reinfLayer.clear()
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

# Convenience functions.

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

def rebLayer_mm(fi,s,c):
    '''Defines a layer of main reinforcement bars, given the spacing.

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
    '''Defines a layer of main reinforcement bars, given the spacing.

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
