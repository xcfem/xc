# -*- coding: utf-8 -*-
''' RCRectangularSection class.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import math
from misc_utils import log_messages as lmsg
from materials.sections import stress_calc as sc
from materials.sections.fiber_section import basic_rectangular_rc_section
from materials.sections.fiber_section import main_reinforcement
from materials.sections.fiber_section import shear_reinforcement

class RCRectangularSection(basic_rectangular_rc_section.BasicRectangularRCSection):
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
        self.positvRebarRows= main_reinforcement.LongReinfLayers() # list of ReinfRow data (positive face)
        self.negatvRebarRows= main_reinforcement.LongReinfLayers() # list of ReinfRow data (negative face)
        
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


    def clear(self):
        ''' Clears all the members of this object.'''
        self.minCover= None
        self.positvRebarRows.clear()
        self.positvRebarRows= None
        self.negatvRebarRows.clear()
        self.negatvRebarRows= None
        super().clear(self)
            
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
        
    def appendRow(self, positiveReinf, rebarRow:main_reinforcement.ReinfRow):
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
        
    def appendPositiveRow(self, rebarRow:main_reinforcement.ReinfRow):
        ''' Constructs append the giver ReinfRow to the reinforcement in the 
            positive side of the section.

        :rebarRow: row of reinforcement bars.
        '''
        self.appendRow(positiveReinf= True, rebarRow= rebarRow)
        
    def appendNegativeRow(self, rebarRow:main_reinforcement.ReinfRow):
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
        rr= main_reinforcement.ReinfRow(rebarsDiam= rebarsDiam, nRebars= nRebars, width= rowWidth, nominalCover= nominalCover, nominalLatCover= nominalLatCover)
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
        ''' Compute the positions of the reinforcement.
        '''
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

    def getMainReinforcementArea(self):
        ''' Return the total area of the main reinforcement.'''
        retval= 0.0
        if(hasattr(self, 'geomSection')):
           retval= self.geomSection.getReinfLayers.getAreaGrossSection()
        else:
            if(self.negatvRebarRows):
                retval+= self.negatvRebarRows.getAs()
            if(self.positvRebarRows):
                retval+= self.positvRebarRows.getAs()
        return retval
            
    def defSectionGeometry(self, preprocessor, matDiagType, twoDimensional= False):
        '''
        Define the XC section geometry object for a reinforced concrete section 

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                            ("k" for characteristic diagram, "d" for design diagram)
        :param twoDimensional: if true set only one division on IJ direction.
        '''
        self.defDiagrams(preprocessor, matDiagType)
        self.geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion(twoDimensional= twoDimensional)
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
            self.shReinfY= shear_reinforcement.ShearReinforcement(familyName= "sh1uy", nShReinfBranches= nShReinfBranchesY, areaShReinfBranch= math.pi*(fiStirrY)**2/4., shReinfSpacing= spacingY, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)
        if(nShReinfBranchesZ>0):
            self.shReinfZ= shear_reinforcement.ShearReinforcement(familyName= "sh1z", nShReinfBranches= nShReinfBranchesZ, areaShReinfBranch= math.pi*(fiStirrZ)**2/4., shReinfSpacing= spacingZ, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

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
