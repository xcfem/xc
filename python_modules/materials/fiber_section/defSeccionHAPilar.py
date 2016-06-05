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

from materials.ehe import areaBarrasEHE
from materials.fiber_section import defSeccionHASimple

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
    self.areaRebarWidth= areaBarrasEHE.Fi12 

    self.nRebarsDepth= 2 
    self.areaRebarDepth= areaBarrasEHE.Fi10 


class RecordRCColumnSection(defSeccionHASimple.BasicRecordRCSection):
  '''Definition of the variables that make up a reinforced concrete section 
  with reinforcement symmetric in both directions (as usual in columns)
  
  :ivar sectionName:     name identifying the section
  :ivar sectionDescr:    section description
  :ivar concrType:       type of concrete (e.g. hormigonesEHE.HA25)     
  :ivar concrDiagName:   name identifying the characteristic stress-strain diagram of the concrete material
  :ivar depth:           cross-section depth
  :ivar width:           cross-section width
  :ivar nDivIJ:          number of cells in IJ (width) direction
  :ivar nDivJK:          number of cells in JK  (height) direction
  :ivar fiberSectionRepr:
  :ivar reinfSteelType:  type of reinforcement steel
  :ivar reinfDiagName:   name identifying the characteristic stress-strain diagram
                     of the reinforcing steel material
  :ivar shReinfZ:        record of type.defRCSimpleSection.RecordShearReinforcement()
                      defining the shear reinforcement in Z direction
  :ivar shReinfY:        record of type.defRCSimpleSection.RecordShearReinforcement()
                     defining the shear reinforcement in Y direction
  :ivar cover:          concrete clear cover of main reinforcement
  :ivar nRebarsWidth:   number of rebars in the width direction of the 
                    section (each face)
  :ivar areaRebarWidth: cross sectional area of each rebar in  width direction
  :ivar nRebarsDepth:   number of rebars in the depth direction of the 
                    section (each face)
  :ivar areaRebarDepth: cross sectional area of each rebar in the depth direction
  '''

  def __init__(self):
    super(RecordRCColumnSection,self).__init__()
    self.mainBars= ColumnMainReinforcement()

  def defGeomRCColumnSection(self,matDiagType):
    '''Returns a reinforced concrete section with reinforcement 
    symmetric in both directions (as usual in columns)
    Parameters:
   :param matDiagType: type of stress-strain diagram (="k" for characteristic diagram, 
                   ="d" for design diagram)
    '''
    self.defDiagrams(preprocessor,matDiagType)

    geomSection= preprocessor.getMaterialLoader.newSectionGeometry(self.gmSectionName())
    self.defConcreteRegion(geomSection)

    reinforcement= sectionGeom.getReinfLayers
    p1= geom.Pos2d(-width/2+cover,-depth/2+cover) # Armadura inferior (cara -).
    p2= geom.Pos2d(width/2-cover,-depth/2+cover)

    self.reinforcementInf= reinforcement.newStraightReinfLayer(reinfDiagName)
    self.reinforcementInf.codigo= "widthInf"
    self.reinforcementInf.numReinfBars= nRebarsWidth
    self.reinforcementInf.barArea= areaRebarWidth
    self.reinforcementInf.p1= geom.Pos2d(-width/2+cover,-depth/2+cover) # Armadura inferior (cara -).
    self.reinforcementInf.p2= geom.Pos2d(width/2-cover,-depth/2+cover)

    self.reinforcementSup= reinforcement.newStraightReinfLayer(reinfDiagName)
    self.reinforcementSup.codigo= "widthSup"
    self.reinforcementSup.numReinfBars= nRebarsWidth
    self.reinforcementSup.barArea= areaRebarWidth
    self.reinforcementSup.p1= geom.Pos2d(-width/2+cover,depth/2+cover) # Armadura superior (cara +).
    self.reinforcementSup.p2= geom.Pos2d(width/2-cover,depth/2+cover)

    rebarsSpacingCanto= (depth-2*cover)/(nRebarsDepth+1)

    self.reinforcementCIzq= reinforcement.newStraightReinfLayer(reinfDiagName)
    self.reinforcementCIzq.codigo= "depthIzda"
    self.reinforcementCIzq.numReinfBars= nRebarsDepth
    self.reinforcementCIzq.barArea= areaRebarDepth
    self.reinforcementCIzq.p1= geom.Pos2d(-width/2+cover,-depth/2+cover+rebarsSpacingCanto) # Armadura lateral izquierda.
    self.reinforcementCIzq.p2= geom.Pos2d(-width/2+cover,depth/2-cover-rebarsSpacingCanto)

    self.reinforcementCDer= reinforcement.newStraightReinfLayer(reinfDiagName)
    self.reinforcementCDer.codigo= "depthDcha"
    self.reinforcementCDer.numReinfBars= nRebarsDepth
    self.reinforcementCDer.barArea= areaRebarDepth
    self.reinforcementCDer.p1= geom.Pos2d(width/2-cover,-depth/2+cover+rebarsSpacingCanto) # Armadura lateral derecha.
    self.reinforcementCDer.p2= geom.Pos2d(width/2-cover,depth/2-cover-rebarsSpacingCanto)

