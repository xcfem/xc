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

from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section

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

    def __init__(self):
      super(RCColumnSection,self).__init__()
      self.mainBars= ColumnMainReinforcement()

    def defSectionGeometry(self,matDiagType):
        '''Returns a reinforced concrete section with reinforcement 
        symmetric in both directions (as usual in columns)

        :param matDiagType: type of stress-strain diagram (="k" for characteristic diagram, ="d" for design diagram)
        '''
        self.defDiagrams(preprocessor,matDiagType)

        geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion(geomSection)

        reinforcement= sectionGeom.getReinfLayers
        p1= geom.Pos2d(-width/2+cover,-depth/2+cover) # bottom layer (side -).
        p2= geom.Pos2d(width/2-cover,-depth/2+cover)

        self.reinforcementInf= reinforcement.newStraightReinfLayer(reinfDiagName)
        self.reinforcementInf.codigo= "infWidth"
        self.reinforcementInf.numReinfBars= self.mainBars.nRebarsWidth
        self.reinforcementInf.barArea= self.mainBars.areaRebarWidth
        self.reinforcementInf.p1= geom.Pos2d(-width/2+cover,-depth/2+cover) # bottom layer (side -).
        self.reinforcementInf.p2= geom.Pos2d(width/2-cover,-depth/2+cover)

        self.reinforcementSup= reinforcement.newStraightReinfLayer(reinfDiagName)
        self.reinforcementSup.codigo= "supWidth"
        self.reinforcementSup.numReinfBars= self.mainBars.nRebarsWidth
        self.reinforcementSup.barArea= self.mainBars.areaRebarWidth
        self.reinforcementSup.p1= geom.Pos2d(-width/2+cover,depth/2+cover) # top layer (side +).
        self.reinforcementSup.p2= geom.Pos2d(width/2-cover,depth/2+cover)

        rebarsSpacingCanto= (depth-2*cover)/(nRebarsDepth+1)

        self.reinforcementCIzq= reinforcement.newStraightReinfLayer(reinfDiagName)
        self.reinforcementCIzq.codigo= "leftDepth"
        self.reinforcementCIzq.numReinfBars= self.mainBars.nRebarsDepth
        self.reinforcementCIzq.barArea= self.mainBars.areaRebarDepth
        self.reinforcementCIzq.p1= geom.Pos2d(-width/2+cover,-depth/2+cover+rebarsSpacingCanto) # Left side reinforcement.
        self.reinforcementCIzq.p2= geom.Pos2d(-width/2+cover,depth/2-cover-rebarsSpacingCanto)

        self.reinforcementCDer= reinforcement.newStraightReinfLayer(reinfDiagName)
        self.reinforcementCDer.codigo= "rightDepth"
        self.reinforcementCDer.numReinfBars= self.mainBars.nRebarsDepth
        self.reinforcementCDer.barArea= self.mainBars.areaRebarDepth
        self.reinforcementCDer.p1= geom.Pos2d(width/2-cover,-depth/2+cover+rebarsSpacingCanto) # Right side reinforcement.
        self.reinforcementCDer.p2= geom.Pos2d(width/2-cover,depth/2-cover-rebarsSpacingCanto)

class RCCircularSection(RCSectionBase, section_properties.CircularSection):
    '''
    Base class for rectangular reinforced concrete sections.

    :ivar sectionName: name identifying the section
    :ivar mainReinf: layers of main reinforcement.
    :ivar shReinf:  record of type ShearReinforcement
                    defining the shear reinforcement.
    '''
    def __init__(self,name= 'noName', Rext= 0.25, Rint= 0.0, concrType=None, reinfSteelType=None):
        ''' Constructor.

        :param name: name of the section.
        :param Rext: external radius (defaults to 0.25).
        :param Rint: internal radius (defaults to 0.0).
        :param concrType: type of concrete (e.g. EHE_materials.HA25)     
        :param reinfSteelType: type of reinforcement steel.
        '''
        RCSectionBase.__init__(self,concrType= concrType,reinfSteelType= reinfSteelType, nIJ= 10, nJK= 10)
        section_properties.CircularSection.__init__(self,name,width,depth)

        # Longitudinal reinforcement.
        self.mainReinf= LongReinfLayers()  #list of ReinfRow data (positive face)
        
        # Transverse reinforcement.
        self.shReinf= ShearReinforcement()
        self.shReinf.familyName= "V"
