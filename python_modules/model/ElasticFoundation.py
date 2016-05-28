# -*- coding: utf-8 -*-
from __future__ import division

import xc_base
import geom
from materials import typical_materials
from model import define_apoyos
from xcVtk.malla_ef import Fields

'''Generation of springs for nodes at foundation.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

class ElasticFoundation(object):
  '''Region resting on springs (Winkler elastic foundation)
  Attributes:
    wModulus: Winkler modulus of the foundation (springs in Z direction)
    cRoz:     fraction of the Winkler modulus to apply for friction in
              the contact plane (springs in X, Y directions)
  '''
  def __init__(self,wModulus,cRoz):
    self.wModulus= wModulus
    self.cRoz= cRoz
  def createMaterials(self,preprocessor,name):
    #Materiales elásticos (los incializamos aquí para luego aplicar el módulo elástico que corresponda a cada nudo)
    self.xSpringName= name + '_xSpring'
    self.xSpring=typical_materials.defElasticMaterial(preprocessor,self.xSpringName,0.1)
    self.ySpringName= name + '_ySpring'
    self.ySpring=typical_materials.defElasticMaterial(preprocessor,self.ySpringName,0.1)
    self.zSpringName= name + '_zSpring'
    self.zSpring=typical_materials.defElasticMaterial(preprocessor,self.zSpringName,1)
  def generateSprings(self,xcSet):
    '''Creates the springs at the nodes.'''
    self.foundationSet= xcSet #Set with elastic supported elements
    self.springs= list() #spring elements.
    self.foundationSet.resetTributarias()
    self.foundationSet.calculaAreasTributarias(False)
    sNod= self.foundationSet.getNodes
    preprocessor= self.foundationSet.getPreprocessor
    self.createMaterials(preprocessor,self.foundationSet.name)
    idElem= preprocessor.getElementLoader.defaultTag
    for n in sNod:
      arTribNod=n.getAreaTributaria()
      self.xSpring.E= self.cRoz*self.wModulus*arTribNod
      self.ySpring.E= self.cRoz*self.wModulus*arTribNod
      self.zSpring.E= self.wModulus*arTribNod
      nn= define_apoyos.defApoyoXYZ(preprocessor,n.tag,idElem,self.xSpringName,self.ySpringName,self.zSpringName)
      self.springs.append(preprocessor.getElementLoader.getElement(idElem))
      idElem+= 1
  def getCentroid(self):
    '''Returns the geometric baricenter of the springs.'''
    dx= 0.0; dy= 0.0; dz= 0.0
    A= 0.0
    for e in self.springs:
      n= e.getNodes[1]
      a= n.getAreaTributaria()
      pos= n.getInitialPos3d
      A+= a
      dx+= a*pos[0]; dy+= a*pos[1]; dz+= a*pos[2]
    dx/=A; dy/=A; dz/=A
    return geom.Pos3d(dx,dy,dz)
  def calcPressures(self):
    ''' Foundation pressures over the soil. Calculates pressures
       and forces in the free nodes of the springs
       (those that belongs to both the spring and the foundation)
       and stores these values as properties of those nodes:
       property 'soilPressure:' [xStress,yStress,zStress]
       property 'soilReaction:' [xForce,yForce,zForce]'''
    self.svdReac= geom.SVD3d()
    f3d= geom.Vector3d(0.0,0.0,0.0)
    m3d= geom.Vector3d(0.0,0.0,0.0)
    for e in self.springs:
      n= e.getNodes[1]
      rf= e.getResistingForce()
      a= n.getAreaTributaria()
      if(len(rf)==6):
        f3d= geom.Vector3d(rf[0],rf[1],0.0)
        m3d= geom.Vector3d(0.0,0.0,rf[2])
      else: #len(rf)==12
        f3d= geom.Vector3d(rf[0],rf[1],rf[2])
        m3d= geom.Vector3d(rf[3],rf[4],rf[5])
      pos= n.getInitialPos3d
      self.svdReac+= geom.SVD3d(pos,f3d,m3d)

      #print "dispZ= ", n.getDisp[2]*1e3, "mm"
      n.setProp('soilPressure',[f3d.x/a,f3d.y/a,f3d.z/a])
      n.setProp('soilReaction',[f3d.x,f3d.y,f3d.z])
    return self.svdReac.reduceTo(self.getCentroid())

  def displayPressures(self, defDisplay, caption,fUnitConv,unitDescription):
    '''Display foundation pressures.'''
    reac= self.calcPressures()

    field= Fields.ExtrapolatedScalarField('soilPressure','getProp',self.foundationSet,component=2,fUnitConv= fUnitConv)
    field.display(defDisplay,caption= caption+' '+unitDescription)
