# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"

'''
Definition of the variables that make up a reinforced concrete section 
with reinforcement symmetric in both directions (as usual in columns)
'''
import math
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections import section_properties

class ColumnMainReinforcement(object):
    ''' Main (longitudinal) rebars of a column.

    :ivar cover:          concrete clear cover of main reinforcement
    :ivar nRebarsWidth:   number of rebars in the width direction of the 
                      section (each face)
    :ivar areaRebarWidth: cross sectional area of each rebar in  width direction
    :ivar nRebarsDepth:   number of rebars in the depth direction of the 
                      section (each face)
    :ivar areaRebarDepth: cross sectional area of each rebar in  depth direction
    '''
    def __init__(self):
        self.cover= 0.030 
        self.nRebarsWidth= 5 
        self.areaRebarWidth= EHE_materials.Fi12 

        self.nRebarsDepth= 2 
        self.areaRebarDepth= EHE_materials.Fi10 


class RCRectangularColumnSection(def_simple_RC_section.BasicRectangularRCSection):
    '''Definition of the variables that make up a reinforced concrete section 
    with reinforcement symmetric in both directions (as usual in columns)

    :ivar mainBars: object of type ColumnMainReinforcement defining
                    longitudinal reinforcement.
    '''

    def __init__(self, sectionDescr= None):
      super(RCColumnSection,self).__init__(sectionDescr= sectionDescr)
      self.mainBars= ColumnMainReinforcement()

    def defSectionGeometry(self,matDiagType):
        '''Returns a reinforced concrete section with reinforcement 
        symmetric in both directions (as usual in columns)

        :param matDiagType: type of stress-strain diagram (="k" for characteristic diagram, ="d" for design diagram)
        '''
        self.defDiagrams(preprocessor,matDiagType)

        self.geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion(self.geomSection)

        reinforcement= sectionGeom.getReinfLayers
        p1= geom.Pos2d(-width/2+cover,-depth/2+cover) # bottom layer (side -).
        p2= geom.Pos2d(width/2-cover,-depth/2+cover)

        self.reinforcementInf= reinforcement.newStraightReinfLayer(reinfDiagName)
        self.reinforcementInf.code= "infWidth"
        self.reinforcementInf.numReinfBars= self.mainBars.nRebarsWidth
        self.reinforcementInf.barArea= self.mainBars.areaRebarWidth
        self.reinforcementInf.p1= geom.Pos2d(-width/2+cover,-depth/2+cover) # bottom layer (side -).
        self.reinforcementInf.p2= geom.Pos2d(width/2-cover,-depth/2+cover)

        self.reinforcementSup= reinforcement.newStraightReinfLayer(reinfDiagName)
        self.reinforcementSup.code= "supWidth"
        self.reinforcementSup.numReinfBars= self.mainBars.nRebarsWidth
        self.reinforcementSup.barArea= self.mainBars.areaRebarWidth
        self.reinforcementSup.p1= geom.Pos2d(-width/2+cover,depth/2+cover) # top layer (side +).
        self.reinforcementSup.p2= geom.Pos2d(width/2-cover,depth/2+cover)

        rebarsSpacingCanto= (depth-2*cover)/(nRebarsDepth+1)

        self.reinforcementCIzq= reinforcement.newStraightReinfLayer(reinfDiagName)
        self.reinforcementCIzq.code= "leftDepth"
        self.reinforcementCIzq.numReinfBars= self.mainBars.nRebarsDepth
        self.reinforcementCIzq.barArea= self.mainBars.areaRebarDepth
        self.reinforcementCIzq.p1= geom.Pos2d(-width/2+cover,-depth/2+cover+rebarsSpacingCanto) # Left side reinforcement.
        self.reinforcementCIzq.p2= geom.Pos2d(-width/2+cover,depth/2-cover-rebarsSpacingCanto)

        self.reinforcementCDer= reinforcement.newStraightReinfLayer(reinfDiagName)
        self.reinforcementCDer.code= "rightDepth"
        self.reinforcementCDer.numReinfBars= self.mainBars.nRebarsDepth
        self.reinforcementCDer.barArea= self.mainBars.areaRebarDepth
        self.reinforcementCDer.p1= geom.Pos2d(width/2-cover,-depth/2+cover+rebarsSpacingCanto) # Right side reinforcement.
        self.reinforcementCDer.p2= geom.Pos2d(width/2-cover,depth/2-cover-rebarsSpacingCanto)

class RCCircularSection(def_simple_RC_section.RCSectionBase, section_properties.CircularSection):
    '''
    Base class for rectangular reinforced concrete sections.

    :ivar sectionName: name identifying the section
    :ivar mainReinf: layers of main reinforcement.
    :ivar shReinf:  record of type ShearReinforcement
                    defining the shear reinforcement.
    '''
    def __init__(self,name= 'noName', sectionDescr= None, Rext= 0.25, Rint= 0.0, concrType=None, reinfSteelType=None):
        ''' Constructor.

        :param name: name of the section.
        :param Rext: external radius (defaults to 0.25).
        :param Rint: internal radius (defaults to 0.0).
        :param concrType: type of concrete (e.g. EHE_materials.HA25)     
        :param reinfSteelType: type of reinforcement steel.
        '''
        def_simple_RC_section.RCSectionBase.__init__(self,sectionDescr= sectionDescr, concrType= concrType,reinfSteelType= reinfSteelType, nIJ= 20, nJK= 5)
        section_properties.CircularSection.__init__(self,name,Rint= Rint,Rext= Rext)

        # Longitudinal reinforcement.
        self.mainReinf= def_simple_RC_section.LongReinfLayers()  #list of ReinfRow data (positive face)
        
        # Transverse reinforcement.
        self.shReinf= def_simple_RC_section.ShearReinforcement()
        self.shReinf.familyName= "V"
        
    def isCircular(self):
        ''' Return true if it's a circular section.'''
        return True
    
    def getShearReinfY(self):
        '''Return the shear reinforcement for Vy.'''
        return self.shReinf

    def getShearReinfZ(self):
        '''Return the shear reinforcement for Vz.'''
        return self.shReinf
    
    def getRespT(self,preprocessor):
        '''Material for modeling torsional response of section'''
        return section_properties.CircularSection.getRespT(self,preprocessor,self.fiberSectionParameters.concrType.Gcm()) # Torsional response of the section.

    def getRespVy(self,preprocessor):
        '''Material for modeling Y shear response of section'''
        return section_properties.CircularSection.getRespVy(self,preprocessor,self.fiberSectionParameters.concrType.Gcm())

    def getRespVz(self,preprocessor):
        '''Material for modeling Z shear response of section'''
        return section_properties.CircularSection.getRespVz(self,preprocessor,self.fiberSectionParameters.concrType.Gcm())
        
    def defConcreteRegion(self,geomSection):
        regions= geomSection.getRegions
        rg= regions.newCircularRegion(self.fiberSectionParameters.concrDiagName)
        rg.nDivCirc= self.fiberSectionParameters.nDivCirc()
        rg.nDivRad= self.fiberSectionParameters.nDivRad()
        rg.extRad= self.Rext
        rg.intRad= self.Rint
        rg.initAngle= 0.0
        rg.finalAngle= 2*math.pi

    def getMinCover(self):
        ''' return the minimal cover of the reinforcement.'''
        return self.mainReinf.getMinCover()
        
    def defSectionGeometry(self,preprocessor,matDiagType):
        '''
        Define the XC section geometry object for this reinforced concrete section 

        :param matDiagType: type of stress-strain diagram 
                     ("k" for characteristic diagram, "d" for design diagram)
        '''
        self.defDiagrams(preprocessor,matDiagType)
        self.geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion(self.geomSection)
        reinforcement= self.geomSection.getReinfLayers
        self.mainReinf.defCircularLayers(reinforcement, "reinf", self.fiberSectionParameters.reinfDiagName, self.Rext)

        self.minCover= self.getMinCover()

    def getTorsionalThickness(self):
        '''Return the section thickness for torsion.'''
        return section_properties.CircularSection.getThickness(self)
        
