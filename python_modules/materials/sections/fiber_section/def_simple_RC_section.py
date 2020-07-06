# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import xc_base
import geom
import xc
from materials.sections import section_properties
from materials import typical_materials
import math
from materials.sections import stress_calc as sc
import sys
from misc_utils import log_messages as lmsg

# Classes defining reinforcement.

class ShearReinforcement(object):
    ''' Definition of the variables that make up a family of shear 
    reinforcing bars

    :ivar familyName:        name identifying the family of shear reinforcing bars
    :ivar nShReinfBranches:  number of effective branches 
    :ivar areaShReinfBranch: area of the shear reinforcing bar [in the unit of 
                             area of the model] 
    :ivar shReinfSpacing:    longitudinal distance between transverse 
                             reinforcements [in the unit of length of the model] 
    :ivar angAlphaShReinf:   angle between the shear reinforcing bars and the 
                             axis of the member
    :ivar angThetaConcrStruts: angle between the concrete's compression struts 
                             and the axis of the member
    '''
    def __init__(self,familyName= "noName",nShReinfBranches= 0.0,areaShReinfBranch= 0.0,shReinfSpacing= 0.2,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0):
        self.familyName= familyName # name identifying the family of shear reinforcing bars
        self.nShReinfBranches= nShReinfBranches # Number of effective branches
        self.areaShReinfBranch= areaShReinfBranch # Area of the shear reinforcing bar
        self.shReinfSpacing= shReinfSpacing # longitudinal distance between transverse reinforcements
        self.angAlphaShReinf= angAlphaShReinf # angle between the shear reinforcing bars and the axis of the member.
        self.angThetaConcrStruts= angThetaConcrStruts # angle between the concrete's compression struts and the axis of the member

    def getAs(self):
        '''returns the area per unit length of the family of shear reinforcements
        '''
        return self.nShReinfBranches*self.areaShReinfBranch/self.shReinfSpacing


class ReinfRow(object):
    ''' Definition of the variables that make up a family (row) of main 
    (longitudinal) reinforcing bars.

    :ivar rebarsDiam:    diameter of the bars (if omitted, the diameter is calculated from the rebar area) 
    :ivar areaRebar:     cross-sectional area of the bar (if omitted, the area is calculated from the rebar diameter)
    :ivar rebarsSpacing: spacing between bars (not considered if nRebars is defined)
    :ivar nRebars:  number of rebars to be placed in the row (>1)
    :ivar width: width of the cross-section (defautls to 1m)
    :ivar nominalCover:  nominal cover (defaults to 0.03m)
    :ivar nominalLatCover: nominal lateral cover (only considered if nRebars is defined, defaults to 0.03)
    '''
    def __init__(self,rebarsDiam=None,areaRebar= None,rebarsSpacing=None,nRebars=None,width=1.0,nominalCover=0.03,nominalLatCover=0.03):
        ''' Constructor.

            :param rebarsDiam:    diameter of the bars (if omitted, the diameter is calculated from the rebar area) 
            :param areaRebar:     cross-sectional area of the bar (if omitted, the area is calculated from the rebar diameter)
            :param rebarsSpacing: spacing between bars (not considered if nRebars is defined)
            :param nRebars:  number of rebars to be placed in the row (>1)
            :param width: width of the cross-section (defautls to 1 m)
            :param nominalCover:  nominal cover (defaults to 0.03m)
            :param nominalLatCover: nominal lateral cover (only considered if nRebars is defined, defaults to 0.03)
        '''
        if rebarsDiam:
            self.rebarsDiam= rebarsDiam
        elif areaRebar:
            self.areaRebar= areaRebar
            self.rebarsDiam=2*math.sqrt(areaRebar/math.pi)
        else:
            lmsg.warning('You must define either the diameter or the area of rebars')
        if areaRebar:
            self.areaRebar= areaRebar
        elif rebarsDiam:
            self.areaRebar=math.pi*rebarsDiam**2/4.
        else:
            lmsg.warning('You must define either the diameter or the area of rebars')
        if nRebars:
            self.nRebars= nRebars
            # if width==1.0:
            #     lmsg.warning('Spacing is calculated using a section width = 1 m')
            self.rebarsSpacing= (width-2*nominalLatCover-self.rebarsDiam)/(nRebars-1)
        elif rebarsSpacing:
            self.rebarsSpacing= rebarsSpacing
            nRebarsTeor= width/rebarsSpacing
            self.nRebars= int(math.floor(nRebarsTeor))
        else:
            lmsg.warning('You must define either the number of rebars or the rebar spacing')
        self.cover= nominalCover+self.rebarsDiam/2.0
        self.centerRebars(width)

    def getAs(self):
        '''returns the total cross-sectional area of reinforcing steel in the family
        '''
        return self.nRebars*self.areaRebar
      
    def centerRebars(self,width):
        '''center the row of rebars in the width of the section'''
        self.coverLat= (width-(self.nRebars-1)*self.rebarsSpacing)/2.0

    def defStraightLayer(self,reinforcement,code,diagramName,p1,p2):
        '''Definition of a straight reinforcement layer in the XC section geometry object 
        between the 2d positions p1 and p2.

        :param reinforcement: XC section geometry reinforcement.
        :param code: identifier for the layer.
        :param diagramName: name of the strain-stress diagram of the steel.
        :param p1: first point of the layer.
        :param p2: last point of the layer.
        '''
        if(self.nRebars>0):
            self.reinfLayer= reinforcement.newStraightReinfLayer(diagramName)
            self.reinfLayer.code= code
            self.reinfLayer.numReinfBars= self.nRebars
            self.reinfLayer.barDiameter= self.rebarsDiam
            self.reinfLayer.barArea= self.areaRebar
            self.reinfLayer.p1= p1
            self.reinfLayer.p2= p2
            return self.reinfLayer
        
    def defCircularLayer(self,reinforcement, code, diagramName, extRad, initAngle= 0.0, finalAngle= 2*math.pi):
        '''Definition of a circular reinforcement layer in the XC section geometry object 
        between the angle arguments.

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

def RebarRow2ReinfRow(rebarRow, width= 1.0, nominalLatCover= 0.03):
    ''' Returns a RebarRow object from a ReinfRow object
        as defined in the rebar_family module.

    :param rebarRow: RebarRow object.
    :param width: width of the cross-section (defautls to 1 m)
    :param nominalLatCover: nominal lateral cover (only considered if nRebars is defined, defaults to 0.03)
    '''
    return RebarRow(rebarsDiam= rebarRow.diam,rebarsSpacing= rebarRows.spacing,width= widht, nominalCover= rebarRow.cover, nominalLatCover= nominalLatCover)

class LongReinfLayers(object):
    ''' Layers of longitudinal reinforcement.'''
    def __init__(self, lst= None):
        ''' Constructor.'''
        if(lst):
            self.rebarRows= lst  # list of ReinfRow data
        else:
            self.rebarRows= list()
        self.reinfLayers= list()  # list of XC::StraightReinfLayer created.

    def __getitem__(self, index):
        '''Return the i-th reinforcement row.'''
        return self.rebarRows[index]

    def __len__(self):
        '''Return the number of reinforcement rows.'''
        return len(self.rebarRows)

    def append(self, rebarRow):
        ''' Append a reinforcement row to the list.'''
        self.rebarRows.append(rebarRow)
        
    def getAsRows(self):
        '''Returns a list with the cross-sectional area of the rebars in each row.'''
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
        return retval
        
    def getRowsCGcover(self):
        '''returns the distance from the center of gravity of the rebars
        to the face of the section 
        '''
        retval=0
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
        '''returns a list with the lateral cover of bars for each row of bars.'''
        retval=[]
        for rbRow in self.rebarRows:
            retval.append(rbRow.coverLat)
        return retval

    def centerRebars(self, b):
        '''centers in the width of the section the rebars.''' 
        for rbRow in self.rebarRows:
            rbRow.centerRebars(b)
            
    def defStraightLayers(self, reinforcement, code, diagramName, pointPairs):
        '''
        Definition of the reinforcement layers

        :param reinforcement: XC section reinforcement.
        :param code: identifier for the layer.
        :param diagramName: name of the strain-stress diagram of the steel.
        :param pointPairs: end points for each row.
        '''
        for rbRow, pts in zip(self.rebarRows, pointPairs):
            p1= pts[0]; p2= pts[1]
            self.reinfLayers.append(rbRow.defStraightLayer(reinforcement,code,diagramName,p1,p2))
            
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
            lmsg.warning('No longitudinal reinforcement.')

def rebLayer_mm(fi,s,c):
    '''Defines a layer of main reinforcement bars, given the spacement.

    :param fi: bar diameter [mm]
    :param s: spacing [mm]
    :param c: cover [mm]
    '''
    return ReinfRow(rebarsDiam=fi*1e-3,areaRebar= math.pi*(fi*1e-3)**2/4.0,rebarsSpacing=s*1e-3,width=1.0,nominalCover=c*1e-3)

def rebLayerByNumFi_mm(n,fi,c,latC,L):
    '''Defines a layer of  main reinforcement bars with a fixed number of rebars. Spacing is calculated
    so that the rebars (and two lateral covers) are inserted in the length L passed as parameter.

    :param n: number of rebars
    :param fi: bar diameter [mm]
    :param c: nominal cover [mm]
    :param latC: nominal lateral cover [mm]
    :param L: length where the n rebars and two lateral covers are inserted [mm]
    '''
    rl=ReinfRow(rebarsDiam=fi*1e-3,areaRebar= math.pi*(fi*1e-3)**2/4.0,nRebars=n,width=L*1e-3,nominalCover=c*1e-3,nominalLatCover=latC*1e-3)
    return rl

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
    def __init__(self, concrType=None, reinfSteelType=None, nDivIJ= 10, nDivJK= 10):
        self.concrType= concrType
        self.concrDiagName= None
        self.reinfSteelType= reinfSteelType
        self.reinfDiagName= None # Name of the uniaxial material
        self.nDivIJ= nDivIJ
        self.nDivJK= nDivJK

    def nDivCirc(self):
        '''Alias for nDivIJ when defining circular sections.'''
        return self.nDivIJ
    
    def nDivRad(self):
        '''Alias for nDivJK when defining circular sections.'''
        return self.nDivJK

    def getConcreteDiagram(self,preprocessor):
        return preprocessor.getMaterialHandler.getMaterial(self.concrDiagName)
      
    def getSteelDiagram(self,preprocessor):
        return preprocessor.getMaterialHandler.getMaterial(self.reinfDiagName)
      
    def getSteelEquivalenceCoefficient(self,preprocessor):
        tangHorm= self.getConcreteDiagram(preprocessor).getTangent()
        tangSteel= self.getSteelDiagram(preprocessor).getTangent()
        return tangSteel/tangHorm

    def defDiagrams(self,preprocessor,matDiagType):
        '''Stress-strain diagrams definition.

        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
        '''
        self.diagType= matDiagType
        if(self.diagType=="d"):
            if(self.concrType.matTagD<0):
                concreteMatTag= self.concrType.defDiagD(preprocessor)
            if(self.reinfSteelType.matTagD<0):
                reinfSteelMaterialTag= self.reinfSteelType.defDiagD(preprocessor)
            self.concrDiagName= self.concrType.nmbDiagD
            self.reinfDiagName= self.reinfSteelType.nmbDiagD
        elif(self.diagType=="k"):
            if(self.concrType.matTagK<0):
                concreteMatTag= self.concrType.defDiagK(preprocessor)
            if(self.reinfSteelType.matTagK<0):
                reinfSteelMaterialTag= self.reinfSteelType.defDiagK(preprocessor)
            self.concrDiagName= self.concrType.nmbDiagK
            self.reinfDiagName= self.reinfSteelType.nmbDiagK
            
    def defInteractionDiagramParameters(self, preprocessor):
        ''' Defines the parameters for interaction diagrams. '''
        self.idParams= xc.InteractionDiagramParameters()
        if(self.diagType=="d"):
            self.idParams.concreteTag= self.concrType.matTagD
            self.idParams.reinforcementTag= self.reinfSteelType.matTagD
        elif(self.diagType=="k"):
            self.idParams.concreteTag= self.concrType.matTagK
            self.idParams.reinforcementTag= self.reinfSteelType.matTagK
        return self.idParams

class RCSectionBase(object):
    '''
    Base class for reinforced concrete sections.

    :ivar sectionDescr: section description.
    :ivar fiberSectionParameters: Parameters needed to create a reinforced 
                                  concrete fiber section.
    :ivar fiberSectionRepr: fiber model of the section.
    '''
    def __init__(self, sectionDescr= None, concrType=None,reinfSteelType=None, nIJ= 10, nJK= 10):
        ''' Constructor.

        :param sectionDescr: section description.
        :param concrType: type of concrete (e.g. EHE_materials.HA25).     
        :param reinfSteelType: type of reinforcement steel.
        :param nIJ: number of cells in IJ (width or radial) direction.
        :param nJK: number of cells in JK (height or tangential) direction.
        '''
        self.sectionDescr= 'Text describing the role/position of the section in the structure.'
        if(sectionDescr):
            self.sectionDescr= sectionDescr
        self.fiberSectionParameters= RCFiberSectionParameters(concrType= concrType, reinfSteelType= reinfSteelType, nDivIJ= nIJ, nDivJK= nJK)
        self.fiberSectionRepr= None

    def gmSectionName(self):
        ''' returns the name of the geometric section'''
        return "geom"+self.sectionName
                
    def getConcreteDiagram(self,preprocessor):
        ''' Return the concrete stress-strain diagram.'''
        return self.fiberSectionParameters.getConcreteDiagram(preprocessor)
      
    def getSteelDiagram(self,preprocessor):
        ''' Return the reinforcing steel stress-strain diagram.'''
        return self.fiberSectionParameters.getSteelDiagram(preprocessor)
      
    def getSteelEquivalenceCoefficient(self,preprocessor):
        ''' Return the equivalence coefficient: Es/Ec.'''
        return self.fiberSectionParameters.getSteelEquivalenceCoefficien(preprocessor)

    def defDiagrams(self,preprocessor,matDiagType):
        '''Stress-strain diagrams definition.

        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
        '''
        return self.fiberSectionParameters.defDiagrams(preprocessor, matDiagType)

    def defShearResponse(self, preprocessor):
        ''' Define the shear/torsional response of the section.'''
        self.respT= self.getRespT(preprocessor) # Torsional response of the section.
        self.respVy= self.getRespVy(preprocessor)
        self.respVz= self.getRespVz(preprocessor)

    def defFiberSection(self,preprocessor):
        '''Define fiber section from geometry data.'''
        self.fs= preprocessor.getMaterialHandler.newMaterial("fiberSectionShear3d",self.sectionName)
        self.fiberSectionRepr= self.fs.getFiberSectionRepr()
        self.fiberSectionRepr.setGeomNamed(self.gmSectionName())
        self.fs.setupFibers()
        self.fs.setRespVyByName(self.respVyName())
        self.fs.setRespVzByName(self.respVzName())
        self.fs.setRespTByName(self.respTName())
        self.fs.setProp('sectionData',self)
        
    def defRCSection(self, preprocessor,matDiagType):
        '''
        Definition of an XC reinforced concrete section.

        :param matDiagType: type of stress-strain diagram 
                    ("k" for characteristic diagram, "d" for design diagram)
         '''
        self.defShearResponse(preprocessor)
        self.defSectionGeometry(preprocessor,matDiagType)
        self.defFiberSection(preprocessor)

    def isCircular(self):
        ''' Return true if it's a circular section.'''
        return False
        
    def defInteractionDiagramParameters(self, preprocessor):
        ''' parameters for interaction diagrams. '''
        return self.fiberSectionParameters.defInteractionDiagramParameters(preprocessor)

    def defInteractionDiagram(self,preprocessor):
        'Defines 3D interaction diagram.'
        if(not self.fiberSectionRepr):
            lmsg.error("defInteractionDiagram: fiber section representation for section: "+ self.sectionName + ";  not defined yet; use defRCSection method.\n")
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagram(self.sectionName,self.fiberSectionParameters.idParams)

    def defInteractionDiagramNMy(self,preprocessor):
        'Defines N-My interaction diagram.'
        if(not self.fiberSectionRepr):
            lmsg.error("defInteractionDiagramNMy: fiber section representation for section: "+ self.sectionName + ";  not defined yet; use defRCSection method.\n")
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagramNMy(self.sectionName,self.fiberSectionParameters.idParams)

    def defInteractionDiagramNMz(self,preprocessor):
        'Defines N-Mz interaction diagram.'
        if(not self.fiberSectionRepr):
            lmsg.error("defInteractionDiagramNMz: fiber section representation for section: "+ self.sectionName + ";  not defined yet; use defRCSection method.\n")
        self.defInteractionDiagramParameters(preprocessor)
        return preprocessor.getMaterialHandler.calcInteractionDiagramNMz(self.sectionName,self.fiberSectionParameters.idParams)

class BasicRectangularRCSection(RCSectionBase, section_properties.RectangularSection):
    '''
    Base class for rectangular reinforced concrete sections.

    :ivar sectionName:     name identifying the section
    :ivar shReinfZ:        record of type ShearReinforcement
                           defining the shear reinforcement in Z direction
    :ivar shReinfY:        record of type ShearReinforcement
                           defining the shear reinforcement in Y direction
    '''
    def __init__(self,name= 'noName', sectionDescr= None, width=0.25,depth=0.25,concrType=None,reinfSteelType=None):
        ''' Constructor.

        :param name: name of the section     
        :param sectionDescr: section description.
        :param width: section width.
        :param depth: section depth.
        :param concrType: type of concrete (e.g. EHE_materials.HA25)     
        :param reinfSteelType: type of reinforcement steel.
        '''
        RCSectionBase.__init__(self, sectionDescr= sectionDescr, concrType= concrType,reinfSteelType= reinfSteelType, nIJ= 10, nJK= 10)
        section_properties.RectangularSection.__init__(self,name,width,depth)

        # Transverse reinforcement (z direction)
        self.shReinfZ= ShearReinforcement()
        self.shReinfZ.familyName= "Vz"

        # Transverse reinforcement (y direction)
        self.shReinfY= ShearReinforcement()
        self.shReinfY.familyName= "Vy"

    def getShearReinfY(self):
        '''Return the shear reinforcement for Vy.'''
        return self.shReinfY

    def getShearReinfZ(self):
        '''Return the shear reinforcement for Vz.'''
        return self.shReinfZ
    
    def getRespT(self,preprocessor):
        '''Material for modeling torsional response of section'''
        return section_properties.RectangularSection.getRespT(self,preprocessor,self.fiberSectionParameters.concrType.Gcm()) # Torsional response of the section.

    def getRespVy(self,preprocessor):
        '''Material for modeling Y shear response of section'''
        return section_properties.RectangularSection.getRespVy(self,preprocessor,self.fiberSectionParameters.concrType.Gcm())

    def getRespVz(self,preprocessor):
        '''Material for modeling Z shear response of section'''
        return section_properties.RectangularSection.getRespVz(self,preprocessor,self.fiberSectionParameters.concrType.Gcm())

    def defConcreteRegion(self,geomSection):
        regions= geomSection.getRegions
        rg= regions.newQuadRegion(self.fiberSectionParameters.concrDiagName)
        rg.nDivIJ= self.fiberSectionParameters.nDivIJ
        rg.nDivJK= self.fiberSectionParameters.nDivJK
        rg.pMin= geom.Pos2d(-self.b/2,-self.h/2)
        rg.pMax= geom.Pos2d(self.b/2,self.h/2)


class RCRectangularSection(BasicRectangularRCSection):
    '''
    This class is used to define the variables that make up a reinforced 
    concrete section with top and bottom reinforcement layers.

    :ivar sectionName:     name identifying the section
    :ivar sectionDescr:    section description
    :ivar minCover:        minimum value of end or clear concrete cover of main 
                           bars from both the positive and negative faces
    :ivar negatvRebarRows: layers of main rebars in the local negative face of 
                           the section
    :ivar positvRebarRows: layers of main rebars in the local positive face of 
                           the section
    '''
    def __init__(self,name= 'noName', sectionDescr= None, width=0.25,depth=0.25,concrType=None,reinfSteelType=None):
        ''' Constructor.

        :param name: name of the section 
        :param sectionDescr: section description.
        :param width: section width.
        :param depth: section depth.
        :param concrType: type of concrete (e.g. EHE_materials.HA25)     
        :param reinfSteelType: type of reinforcement steel.
        '''
        super(RCRectangularSection,self).__init__(name,sectionDescr,width,depth,concrType,reinfSteelType)

        # Longitudinal reinforcement
        self.minCover= 0.0 
        self.positvRebarRows= LongReinfLayers()  #list of ReinfRow data (positive face)
        self.negatvRebarRows= LongReinfLayers() #list of ReinfRow data (negative face)

    def getAsPos(self):
        '''returns the cross-sectional area of the rebars in the positive face.'''
        return self.positvRebarRows.getAs()

    def getPosRowsCGcover(self):
        '''returns the distance from the center of gravity of the positive rebars
        to the positive face of the section.
        '''
        return self.positvRebarRows.getRowsCGcover()

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

    def getYAsNeg(self):
        '''returns the local Y coordinate of the center of gravity of the rebars
           in the negative face
        '''
        return -self.h/2.0+self.getNegRowsCGcover()

    def getAc(self):
        '''returns the cross-sectional area of the section'''
        return self.b*self.h
    
    def getRoughVcuEstimation(self):
        '''returns a minimal value (normally shear strength will be greater)
           of the shear strength of the concrete section Vcu expressed
           in newtons.'''
        return 500*self.getAc()
      
    def getI(self):
        '''returns the second moment of area about the middle axis parallel to 
        the width '''
        return 1/12.0*self.b*self.h**3

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

    def getNBarPos(self):
        '''returns a list with the number of bars for each row of bars in local 
        positive face.'''
        return self.positvRebarRows.getNBar()

    def getNBarNeg(self):
        '''returns a list with the number of bars for each row of bars in local 
        negative face.'''
        return self.negatvRebarRows.getNBar()

    def getCoverPos(self):
        '''returns a list with the cover of bars for each row of bars in local 
        positive face.'''
        return self.positvRebarRows.getCover()

    def getCoverNeg(self):
        '''returns a list with the cover of bars for each row of bars in local 
        negative face.'''
        return self.negatvRebarRows.getCover()

    def getLatCoverPos(self):
        '''returns a list with the lateral cover of bars for each row of bars in local positive face.'''
        return self.positvRebarRows.getLatCover()

    def getLatCoverNeg(self):
        '''returns a list with the lateral cover of bars for each row of bars in 
        local negative face.'''
        return self.negatvRebarRows.getLatCover()

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
        return retval

    def defSectionGeometry(self,preprocessor,matDiagType):
        '''
        Define the XC section geometry object for a reinforced concrete section 

        :param matDiagType: type of stress-strain diagram 
                     ("k" for characteristic diagram, "d" for design diagram)
        '''
        self.defDiagrams(preprocessor,matDiagType)
        self.geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion(self.geomSection)
        reinforcement= self.geomSection.getReinfLayers
        negPoints= list()
        for rbRow in self.negatvRebarRows.rebarRows:
            y= -self.h/2.0+rbRow.cover
            p1= geom.Pos2d(-self.b/2+rbRow.coverLat,y)
            p2= geom.Pos2d(self.b/2-rbRow.coverLat,y)
            negPoints.append((p1,p2))
        self.negatvRebarRows.defStraightLayers(reinforcement,"neg",self.fiberSectionParameters.reinfDiagName,negPoints)
        posPoints= list()
        for rbRow in self.positvRebarRows.rebarRows:
            y= self.h/2.0-rbRow.cover
            p1= geom.Pos2d(-self.b/2+rbRow.coverLat,y)
            p2= geom.Pos2d(self.b/2-rbRow.coverLat,y)
            posPoints.append((p1,p2))
        self.positvRebarRows.defStraightLayers(reinforcement,"pos",self.fiberSectionParameters.reinfDiagName,posPoints)
        self.minCover= self.getMinCover()

    def getTorsionalThickness(self):
        '''Return the section thickness for torsion.'''
        return min(self.b,self.h)/2.0
    
    def getStressCalculator(self):
        Ec= self.fiberSectionParameters.concrType.Ecm()
        Es= self.fiberSectionParameters.reinfSteelType.Es
        return sc.StressCalc(self.b,self.h,self.getPosRowsCGcover(),self.getNegRowsCGcover(),self.getAsPos(),self.getAsNeg(),Ec,Es)


