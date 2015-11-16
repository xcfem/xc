# -*- coding: utf-8 -*-

'''gridModelData.py: model generation based on a grid of 3D positions. Data structures.'''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from materials import typical_materials
from model import predefined_spaces
from model import define_apoyos
import ijkGrid as grid

class NamedObjectsMap(dict):
  '''dictionary of objects which have a name.'''
  def __init__(self, objs):
    for o in objs:
      self.add(o)
  def add(self,obj):
    self[obj.name]= obj

class MaterialData(object):
  def __init__(self,name,E,nu,rho):
    self.name= name
    self.E= E
    self.nu= nu
    self.rho= rho

class DeckMaterialData(MaterialData):
  def __init__(self,name,E,nu,rho,thickness):
    super(DeckMaterialData,self).__init__(name,E,nu,rho)
    self.thickness= thickness
  def getAreaDensity(self):
    return self.rho*self.thickness
  def setup(self,preprocessor):
    typical_materials.defElasticMembranePlateSection(preprocessor,self.name,self.E,self.nu,self.getAreaDensity(),self.thickness)

class MaterialDataMap(NamedObjectsMap):
  '''Material data dictionary.'''
  def __init__(self,materialDataList):
    super(MaterialDataMap,self).__init__(materialDataList)
  def setup(self, preprocessor):
    for key in self:
      self[key].setup(preprocessor)

class IJKRangeList(object):
  ''' a named IJK range list.'''
  def __init__(self, name, grid, ranges= list()):
    self.name= name
    self.grid= grid
    self.ranges= list()

  def append(self,ijkRange):
    self.ranges.append(ijkRange)

  def applyLoadVector(self, dicSup, loadVector):
    i= 0
    for r in self.ranges:
      nmbrSet=self.name+str(i)
      s= self.grid.applyLoadInRange(r,dicSup,nmbrSet,loadVector)
      i+= 1
    return i

  def applyEarthPressure(self, dicSup, earthPressure):
    i= 0
    for r in self.ranges:
      nmbrSet=self.name+str(i)
      s= self.grid.applyEarthPressure(ijkRange,dicSup,nmbrSet,earthPressure)
      i+= 1
    return i

class MaterialSurface(IJKRangeList):
  '''Surface defined by a range list, a material and an element type and size.'''
  def __init__(self,name, grid, material,elemType,elemSize):
    super(MaterialSurface,self).__init__(name,grid,list())
    self.material= material
    self.elemType= elemType
    self.elemSize= elemSize

  def generateAreas(self, dicSup):
    self.lstSup= list()
    for ijkRange in self.ranges:
      self.lstSup+= self.grid.generateAreas(ijkRange,dicSup)
    for s in self.lstSup:
       s.setElemSizeIJ(self.elemSize,self.elemSize)

  def generateMesh(self, seedElementLoader):
    seedElementLoader.defaultMaterial= self.material.name
    elem= seedElementLoader.newElement(self.elemType,xc.ID([0,0,0,0]))
    for s in self.lstSup:
       s.genMesh(xc.meshDir.I)

  def applyVector3dUniformLoadGlobal(self,loadVector):
    for s in self.lstSup:
      elSup=s.getElements()
      for el in elSup:
        el.vector3dUniformLoadGlobal(loadVector)    

  def applyStrainGradient(self,loadPattern,nabla):
    for s in self.lstSup:
      elSup=s.getElements()
      for el in elSup:
        eleLoad= loadPattern.newElementalLoad("shell_strain_load")
        eleLoad.elementTags= xc.ID([el.tag])
        eleLoad.setStrainComp(0,3,nabla)
        eleLoad.setStrainComp(1,3,nabla)
        eleLoad.setStrainComp(2,3,nabla)
        eleLoad.setStrainComp(3,3,nabla)
  
class MaterialSurfacesMap(NamedObjectsMap):
  '''MaterialSurfaces dictionary.'''
  def __init__(self,surfaces):
    super(MaterialSurfacesMap,self).__init__(surfaces)
  def generateAreas(self, dicSup):
    for key in self:
      self[key].generateAreas(dicSup)
  def generateMesh(self, preprocessor, dicSup, firstElementTag= 1):
    self.generateAreas(dicSup)
    preprocessor.getCad.getSurfaces.conciliaNDivs()
    nodes= preprocessor.getNodeLoader
    predefined_spaces.gdls_resist_materiales3D(nodes)
    nodes.newSeedNode()
    seedElemLoader= preprocessor.getElementLoader.seedElemLoader
    seedElemLoader.defaultTag= firstElementTag
    for key in self:
      self[key].generateMesh(seedElemLoader)

class ConstrainedRanges(IJKRangeList):
  '''Region of constrained nodes
     [uX, uY, uZ,rotX, rotY, rotZ]
     uX, uY, uZ: translations in the X, Y and Z directions; 
     rotX, rotY, rotZ: rotations about the X, Y and Z axis
     -free- means no constraint
  '''
  def __init__(self,name, grid, constraints):
    super(ConstrainedRanges,self).__init__(name,grid,list())
    self.constraints= constraints
  def generateContraintsInLines(self):
    constraints= self.grid.prep.getConstraintLoader
    for rng in self.ranges:
      lstLin= self.grid.getLstLinRange(rng)
      for l in lstLin:
        setnod=l.getNodes()
        for n in setnod:
          for i in range(0,6):
            coac= self.constraints[i]
            if(coac <> 'free'):
              constraints.newSPConstraint(n.tag,i,coac) 


class ConstrainedRangesMap(NamedObjectsMap):
  '''Constrained ranges dictionary.'''
  def __init__(self,cranges):
    super(ConstrainedRangesMap,self).__init__(cranges)
  def generateContraintsInLines(self):
    for key in self:
      self[key].generateContraintsInLines()

class ElasticFoundationRanges(IJKRangeList):
  '''Region resting on springs (Winkler model)'''
  def __init__(self,name, grid, wModulus, cRoz):
    ''' wModulus: Winkler modulus.
        cRoz: fraction of the Winkler modulus to apply in the contact plane.'''
    super(ElasticFoundationRanges,self).__init__(name,grid,list())
    self.wModulus= wModulus
    self.cRoz= cRoz

class ElasticFoundationRangesMap(NamedObjectsMap):
  '''Constrained ranges dictionary.'''
  def __init__(self,efranges):
    super(ElasticFoundationRangesMap,self).__init__(efranges)
  def generateSprings(self, prep, dicSup):
    #Apoyo elástico en el terreno
    #Materiales elásticos (los incializamos aquí para luego aplicar el módulo elástico que corresponda a cada nudo)
    self.muellX=typical_materials.defElasticMaterial(prep,'muellX',0.5)
    self.muellY=typical_materials.defElasticMaterial(prep,'muellY',0.5)
    self.muellZ=typical_materials.defElasticMaterial(prep,'muellZ',1)
    for key in self.keys():
      apel= self[key]
      cBal= apel.wModulus
      cRozam= apel.cRoz
      i= 0
      for r in apel.ranges:
        nmbrSet= key+str(i)
        s= apel.grid.getSetInRange(r,dicSup,nmbrSet)
        #print nmbrSet,s.getNodes.size
        s.resetTributarias()
        s.calculaAreasTributarias(False)
        sNod=s.getNodes
        idElem= apel.grid.prep.getElementLoader.defaultTag
        for n in sNod:
          arTribNod=n.getAreaTributaria()
          self.muellX.E=cRozam*cBal*arTribNod
          self.muellY.E=cRozam*cBal*arTribNod
          self.muellZ.E=cBal*arTribNod
          #print n.tag,arTribNod,muellX.E,muellY.E,muellZ.E
          nn= define_apoyos.defApoyoXYZ(apel.grid.prep,n.tag,idElem,'muellX','muellY','muellZ')
          idElem+= 1
        i+=1

class LoadOnSurfaces(object):
  '''Load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, surfaces):
    self.name= name
    self.surfaces= surfaces

class InertialLoadOnSurfaces(LoadOnSurfaces):
  '''Inertial load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, surfaces, accel):
    super(InertialLoadOnSurfaces,self).__init__(name, surfaces)
    self.acceleration= accel

  def applyLoad(self):
    vectorAceleracion= xc.Vector(self.acceleration)
    #print ec, key, vectorAceleracion[0], vectorAceleracion[1], vectorAceleracion[2]
    #peso propio de elementos tipo shell
    for csup in self.surfaces:
      mat= csup.material
      masaUnit= mat.getAreaDensity()
      vectorCarga=masaUnit*vectorAceleracion
      csup.applyVector3dUniformLoadGlobal(vectorCarga)

class PressureLoadOnSurfaces(LoadOnSurfaces):
  '''Pressure load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, surfaces, loadVector):
    super(PressureLoadOnSurfaces,self).__init__(name, surfaces)
    self.loadVector= loadVector

  def applyLoad(self,dicSup):
    loadVector= xc.Vector(self.loadVector)
    self.surfaces.applyLoadVector(dicSup,loadVector)

class EarthPressureOnSurfaces(LoadOnSurfaces):
  '''Earth pressure over a list of surfaces (defined as range lists).'''
  def __init__(self,name, surfaces, earthPressure):
    super(EarthPressureOnSurfaces,self).__init__(name, surfaces)
    self.earthPressure= earthPressure

  def applyEarthPressure(self,dicSup):
    self.surfaces.applyEarthPressure(dicSup,self.earthPressure)


class StrainLoadOnSurfaces(LoadOnSurfaces):
  '''Inertial load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, surfaces, epsilon):
    super(StrainLoadOnSurfaces,self).__init__(name, surfaces)
    self.epsilon= epsilon


class StrainGradientLoadOnSurfaces(StrainLoadOnSurfaces):
  '''Inertial load over a list of surfaces (defined as range lists).'''
  def __init__(self,name, surfaces,epsilon):
    super(StrainGradientLoadOnSurfaces,self).__init__(name, surfaces, epsilon)

  def applyLoad(self,lp):
    for csup in self.surfaces:
      #print csup
      mat= csup.material
      esp= mat.thickness
      nabla= self.epsilon/esp
      csup.applyStrainGradient(lp,nabla)

class LoadOnSurfacesMap(NamedObjectsMap):
  '''Map of inertial load over surfaces.'''
  def __init__(self,efranges):
    super(LoadOnSurfacesMap,self).__init__(efranges)

class LoadState(object):
  def __init__(self,name, cInerc= list(), cUnifSup= list(), cUnifXYZSup= list(), qPuntual= list(), eTierr= list(), eHidrostVol= list(), gradTemp= list()):
    self.name= name
    self.cInerc= cInerc
    self.cUnifSup= cUnifSup
    self.cUnifXYZSup= cUnifXYZSup
    self.qPuntual= qPuntual
    self.eTierr= eTierr
    self.eHidrostVol= eHidrostVol
    self.gradTemp= gradTemp
  def applyInertialLoads(self):
    for pp in self.cInerc:
      print 'cInerc:', pp.name
      pp.applyLoad()
  def applyUniformLoads(self,dicSup):
    for load in self.cUnifSup:
      print 'cUnifSup:', load.name
      load.applyLoad(dicSup)
    for load in self.cUnifXYZSup:
      print 'cUnifXYZSup:', load.name
      load.applyLoad(dicSup)

  def applyPunctualLoads(self):
    #Cargas lineales
    #Cargas puntuales
    for cpunt in self.qPuntual:
      print 'qPuntual:', cpunt
      for i in range(len(qPuntual[cpunt]['coordPto'])):
        cpto=qPuntual[cpunt]['coordPto'][i]
        #print key,cpto
        posP= geom.Pos3d(cpto[0],cpto[1],cpto[2])
        nod=totNod.getNearestNode(posP)
        vectorCarga=xc.Vector(qPuntual[cpunt]['qPunt'])
        lPatterns[key].newNodalLoad(nod.tag,vectorCarga)

  def applyEarthPressureLoads(self,dicSup):
    for ep in self.eTierr:
      ep.applyEarthPressure(dicSup)

  def applyTemperatureGradient(self,lp):
    for gt in self.gradTemp:
      print 'gradTemp:', gt.name
      gt.applyLoad(lp)

  def applyLoads(self,lp,dicSup):
    #Peso propio
    self.applyInertialLoads()
  
    #Cargas uniformes (presiones) sobre superficies
    #Cargas uniformes sobre superficies con componentes XYZ
    self.applyUniformLoads(dicSup)
  
    #Cargas lineales
    #Cargas puntuales
    self.applyPunctualLoads()
  
    #Empuje de tierras
    self.applyEarthPressureLoads(dicSup)
  
    #Gradiente de temperatura
    self.applyTemperatureGradient(lp)


class LoadStateMap(NamedObjectsMap):
  def __init__(self,loadStates):
    super(LoadStateMap,self).__init__(loadStates)
  def applyLoads(self,preprocessor,dicSup):
    cargas= preprocessor.getLoadLoader
    casos= cargas.getLoadPatterns
    ts= casos.newTimeSeries("constant_ts","ts")   #Load modulation.
    casos.currentTimeSeries= "ts"
    retval= dict()
    for key in self.keys():
      print '   ***   ',key,'   ***   '
      retval[key]= casos.newLoadPattern("default",key)
      cargas.getLoadPatterns.currentLoadPattern= key
      loadState= self[key]
      #Peso propio
      loadState.applyLoads(retval[key],dicSup)
    return retval

class GridModel(object):
  def __init__(self,preprocessor):
    self.prep= preprocessor

  def newMaterialSurface(self,name,material,elemType,elemSize):
    return MaterialSurface(name, self.grid, material,elemType,elemSize)

  def setMaterials(self,materialDataList):
    self.materialData= MaterialDataMap(materialDataList)
    return self.materialData

  def setMaterialSurfacesMap(self,materialSurfaceList):
    self.conjSup= MaterialSurfacesMap(materialSurfaceList)
    return self.conjSup

  def setGrid(self,xList,yList,zList):
    self.grid= grid.ijkGrid(self.prep,xList,yList,zList)
    return self.grid

  def newConstrainedRanges(self,name,constraints):
    return ConstrainedRanges(name, self.grid, constraints)

  def setConstrainedRangesMap(self,constrainedRangesList):
    self.constrainedRanges= ConstrainedRangesMap(constrainedRangesList)
    return self.constrainedRanges

  def newElasticFoundationRanges(self,name, wModulus, cRoz):
    return ElasticFoundationRanges(name, self.grid, wModulus, cRoz)

  def setElasticFoundationRangesMap(self,elasticFoundationRangesList):
    self.elasticFoundationRanges= ElasticFoundationRangesMap(elasticFoundationRangesList)
    return self.elasticFoundationRanges

  def setLoadStates(self,loadStateMap):
    self.loadStates= loadStateMap

  def generateMesh(self):
    self.grid.generatePoints() #Key points generation.
    self.materialData.setup(self.prep) #Material definition.
    self.dicSup= dict() #Surfaces dictionary.
    self.conjSup.generateMesh(self.prep,self.dicSup)
    self.constrainedRanges.generateContraintsInLines()
    self.elasticFoundationRanges.generateSprings(self.prep,self.dicSup)
    self.lPatterns= self.loadStates.applyLoads(self.prep,self.dicSup)
    for cs in self.conjSup:
      nbrset='set'+cs
      self.lPatterns[nbrset]= grid.setEntLstSurf(self.prep,self.conjSup[cs].lstSup,nbrset)
    return self.dicSup
