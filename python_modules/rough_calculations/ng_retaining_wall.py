# -*- coding: utf-8 -*-
from __future__ import division

'''Routines for cantilever retaining walls design.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
from postprocess.reports import common_formats as fmt
from postprocess.reports import draw_schema_armature_mur as draw_schema
from postprocess import get_reactions
import math
import scipy.interpolate
import matplotlib
#matplotlib.use('PS')
import matplotlib.pyplot as plt
from materials import typical_materials
from materials import section_properties
from materials.sia262 import SIA262_materials
from model.geometry import retaining_wall_geometry
from rough_calculations import ng_rebar_def
from rough_calculations import ng_rc_section
import os
from miscUtils import LogMessages as lmsg
import geom
import xc
from solution import predefined_solutions

def filterRepeatedValues(yList,mList,vList):
  sz= len(yList)

  mapM={}
  mapV= {}
  for i in range(0,sz):
    y= abs(yList[i])
    mapM[y]= mList[i]
    mapV[y]= vList[i]

  retY= list()
  retM= list()
  retV= list()
  for y in mapM:
    retY.append(y)
  retY.sort()
  for y in retY:
    retM.append(mapM[y])
    retV.append(mapV[y])
  return retY, retM, retV

class InternalForces(object):
  '''Internal forces for a retaining wall obtained.'''
  def __init__(self,y,mdMax,vdMax,MdSemelle,VdSemelle):
    self.y, self.mdMax, self.vdMax= filterRepeatedValues(y,mdMax,vdMax)
    self.interpolate()
    self.stemHeight= self.y[-1]
    print 'stemHeight= ', self.stemHeight
    self.MdSemelle= MdSemelle
    self.VdSemelle= VdSemelle
  def interpolate(self):
    self.mdMaxVoile= scipy.interpolate.interp1d(self.y,self.mdMax)
    self.vdMaxVoile= scipy.interpolate.interp1d(self.y,self.vdMax)    
  def __imul__(self,f):
    for m in self.mdMax:
      m*=f
    for v in self.vdMax:
      v*=f
    self.interpolate()
    self.MdSemelle*=f
    self.VdSemelle*=f
    return self
  def clone(self):
    return InternalForces(self.y, self.mdMax, self.vdMax,self.MdSemelle,self.VdSemelle)
  def __mul__(self, f):
    retval= self.clone()
    retval*= f
    return retval
  def __rmul__(self,f):
    return self*f
  def MdEncastrement(self,footingThickness):
    '''Bending moment (envelope) at stem base.'''
    yEncastrement= self.stemHeight-footingThickness/2.0
    return self.Md(yEncastrement)
  def VdEncastrement(self,epaisseurEncastrement):
    '''Shear force (envelope) at stem base.'''
    yV= self.stemHeight-epaisseurEncastrement
    return abs(self.vdMaxVoile(yV))
  def Vd(self, yCoupe):
    '''Shear (envelope) at height yCoupe.'''
    return abs(self.vdMaxVoile(yCoupe))
  def Md(self, yCoupe):
    '''Bending moment (envelope) at height yCoupe.'''
    return abs(self.mdMaxVoile(yCoupe))
  def getYVoile(self,hCoupe):
    return self.stemHeight-hCoupe
  def writeGraphic(self,fileName):
    '''Draws a graphic of internal forces (envelopes) in
       the wall stem.'''
    z= []
    for yi in self.y:
      z.append(self.stemHeight-yi)
    m= []
    for mi in self.mdMax:
      m.append(mi/1e3)
    v= []
    for vi in self.vdMax:
      v.append(vi/1e3)
    plt.plot(m,z,'-', v, z,'--')
    plt.legend(['Md (kN m/m)', 'Vd (kN/m)'], loc='best')
    plt.title("Efforts internes.")
    plt.savefig(fileName)
    plt.close()

class RetainingWallReinforcement(dict):
  ''' Simplified reinforcement for a cantilever retaining wall.'''
  def __init__(self,concreteCover=40e-3, steel= SIA262_materials.B500B):
    '''Constructor '''
    super(RetainingWallReinforcement, self).__init__()
    self.concreteCover= concreteCover
    #Materials.
    self.steel= steel
    #Default reinforcement
    AdefA= ng_rebar_def.RebarFamily(self.steel,8e-3,0.15,concreteCover)
    Adef= AdefA
    for i in range(1,15):
      self[i]= Adef
    # #Armature de peau semelle
    # R= self.footingThickness-2*self.concreteCover-8e-3
    # n= math.ceil(R/0.15)+1
    # ecart= R/(n-1)
    # self[10]= FamNBars(self.steel,n,8e-3,ecart,concreteCover)
    # #Armature couronnement.
    # R= self.stemTopWidth-2*self.concreteCover-8e-3
    # n= math.ceil(R/0.15)+1
    # ecart= R/(n-1)
    # self[13]= FamNBars(self.steel,n,8e-3,ecart,concreteCover)
    
  def setArmature(self,index,armature):
    '''Assigns armature.'''
    self[index]= armature

  def getArmature(self,index):
    '''Return armature at index.'''
    return self[index]
    
class WallStabilityResults(object):
  def __init__(self,wall,combinations,foundationSoilModel,gammaR= 1):
    self.Foverturning= 1e15
    self.FoverturningComb= ''
    self.Fsliding= 1e15
    self.FslidingComb= ''
    self.Fbearing= 1e15
    self.FbearingComb= ''
    for comb in combinations:
      reactions= wall.resultComb(comb)
      R= reactions.getResultant()
      Foverturning= wall.getOverturningSafetyFactor(R,gammaR)
      if(Foverturning<self.Foverturning):
        self.Foverturning= Foverturning
        self.FoverturningComb= comb
      Fsliding= wall.getSlidingSafetyFactor(R,gammaR,foundationSoilModel)
      if(Fsliding<self.Fsliding):
        self.Fsliding= Fsliding
        self.FslidingComb= comb
      Fbearing= wall.getBearingPressureSafetyFactor(R,foundationSoilModel,1.0)
      if(Fbearing<self.Fbearing):
        self.Fbearing= Fbearing
        self.FbearingComb= comb
  def writeOutput(self,outputFile):
    '''Write results in LaTeX format.'''
    outputFile.write("  Renversement:  & " + fmt.Factor.format(self.Foverturning) +" & 1.00 & "+self.FoverturningComb+'\\\\\n')
    outputFile.write("  Glissement:  & " + fmt.Factor.format(self.Fsliding) +" & 1.00 & "+self.FslidingComb+'\\\\\n')
    outputFile.write("  Poinçonnement:  & " + fmt.Factor.format(self.Fbearing) +" & 1.00 & "+self.FbearingComb+'\\\\\n')

class WallULSResults(object):
  def __init__(self,internalForces):
    self.internalForces= internalForces

class WallSLSResults(WallULSResults):
  def __init__(self,internalForces,rotation, rotationComb):
    super(WallSLSResults,self).__init__(internalForces)
    self.rotation= rotation
    self.rotationComb= rotationComb
    
class RetainingWall(retaining_wall_geometry.CantileverRetainingWallGeometry):
  '''Cantilever retaining wall.'''
  b= 1.0

  def __init__(self,name= 'prb',concreteCover=40e-3,stemBottomWidth=0.25,stemTopWidth=0.25,footingThickness= 0.25):
    '''Constructor '''
    super(RetainingWall,self).__init__(name,stemBottomWidth,stemTopWidth,footingThickness)
    #Materials.
    self.concrete= SIA262_materials.c25_30
    self.reinforcement= RetainingWallReinforcement(concreteCover)
    
  def getBasicAnchorageLength(self,index):
    '''Returns basic anchorage length for the reinforcement at "index".''' 
    return self.reinforcement.getArmature(index).getBasicAnchorageLength(self.concrete)

  def getSection1(self):
    '''Returns RC section for armature in position 1.'''
    return ng_rc_section.RCSection(self.reinforcement[1],self.concrete,self.b,self.stemBottomWidth)
  def getSection2(self,y):
    '''Returns RC section for armature in position 2.'''
    c= self.getDepth(y)
    return ng_rc_section.RCSection(self.reinforcement[2],self.concrete,self.b,c)
  def getSection3(self):
    '''Returns RC section for armature in position 3.'''
    return ng_rc_section.RCSection(self.reinforcement[3],self.concrete,self.b,self.footingThickness)
  def getSection4(self):
    '''Returns RC section for armature in position 4.'''
    return ng_rc_section.RCSection(self.reinforcement[4],self.concrete,self.b,self.stemBottomWidth)
  def getSection6(self):
    '''Returns RC section for armature in position 6.'''
    return ng_rc_section.RCSection(self.reinforcement[6],self.concrete,self.b,self.stemTopWidth)
  def getSection7(self):
    '''Returns RC section for armature in position 7.'''
    return ng_rc_section.RCSection(self.reinforcement[7],self.concrete,self.b,self.footingThickness)
  def getSection8(self):
    '''Returns RC section for armature in position 8.'''
    return ng_rc_section.RCSection(self.reinforcement[8],self.concrete,self.b,self.footingThickness)
  def getSection11(self):
    '''Returns RC section for armature in position 11.'''
    return ng_rc_section.RCSection(self.reinforcement[11],self.concrete,self.b,(self.stemTopWidth+self.stemBottomWidth)/2.0)

  def setULSInternalForcesEnvelope(self,wallInternalForces):
    '''Assigns the ultimate limit state infernal forces envelope for the stem.'''
    if(hasattr(self,'stemHeight')):
      if(self.getWFStemHeigth()!=wallInternalForces.stemHeight):
        lmsg.warning('stem height (' + str(self.stemHeight) + ' m) different from length of internal forces envelope law('+ str(wallInternalForces.stemHeight)+ ' m') 
    else:
      self.stemHeight= wallInternalForces.stemHeight-self.footingThickness/2.0
    self.internalForcesULS= wallInternalForces

  def setSLSInternalForcesEnvelope(self,wallInternalForces):
    '''Assigns the serviceability limit state infernal forces envelope for the stem.'''
    if(hasattr(self,'stemHeight')):
      if(self.getWFStemHeigth()!=wallInternalForces.stemHeight):
        lmsg.warning('stem height (' + str(self.stemHeight) + ' m) different from length of internal forces envelope law('+ str(wallInternalForces.stemHeight)+ ' m') 
    else:
      self.stemHeight= wallInternalForces.stemHeight
    self.internalForcesSLS= wallInternalForces
    
  def writeDef(self,pth,outputFile):
    '''Write wall definition in LaTeX format.'''
    pathFiguraEPS= pth+self.name+".eps"
    pathFiguraPDF= pth+self.name+".pdf"
    self.internalForcesULS.writeGraphic(pathFiguraEPS)
    os.system("convert "+pathFiguraEPS+" "+pathFiguraPDF)
    outputFile.write("\\begin{table}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{tabular}[H]{|l|}\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{1}{|c|}{\\textsc{"+self.name+"}}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\begin{tabular}{c|l}\n")
    outputFile.write("\\begin{minipage}{85mm}\n")
    outputFile.write("\\vspace{2mm}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\includegraphics[width=80mm]{"+self.name+"}\n")
    outputFile.write("\\end{center}\n")
    outputFile.write("\\vspace{1pt}\n")
    outputFile.write("\\end{minipage} & \n")
    self.writeGeometry(outputFile)
    outputFile.write("\\end{tabular} \\\\\n")

    outputFile.write("\\hline\n")
    outputFile.write("\\begin{tabular}{llll}\n")
    outputFile.write("\\multicolumn{3}{c}{\\textsc{Matériels}}\\\\\n")
    outputFile.write("  Béton: " + self.concrete.materialName +" & ")
    outputFile.write("  Acier: " + self.reinforcement.steel.materialName +" & ")
    outputFile.write("  ConcreteCover: "+ fmt.Diam.format(self.reinforcement.concreteCover*1e3)+ " mm\\\\\n")
    outputFile.write("\\end{tabular} \\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\end{tabular}\n")
    outputFile.write("\\caption{Matériels et dimensions mur "+ self.name +"} \\label{tb_def_"+self.name+"}\n")
    outputFile.write("\\end{center}\n")
    outputFile.write("\\end{table}\n")


  def writeResult(self,pth):
    '''Write reinforcement verification results in LaTeX format.'''
    outputFile= open(pth+self.name+".tex","w")
    self.writeDef(pth,outputFile)
    outputFile.write("\\bottomcaption{Calcul armatures mur "+ self.name +"} \\label{tb_"+self.name+"}\n")
    outputFile.write("\\tablefirsthead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{Armatures mur "+self.name+"}}\\\\\\hline\n}\n")
    outputFile.write("\\tablehead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{"+self.name+" (suite)}}\\\\\\hline\n}\n")
    outputFile.write("\\tabletail{\\hline \\multicolumn{1}{|r|}{../..}\\\\\\hline}\n")
    outputFile.write("\\tablelasttail{\\hline}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{supertabular}[H]{|l|}\n")
    outputFile.write("\\hline\n")

    #Coupe 1. Béton armé. Encastrement.
    C1= self.getSection1()
    VdEncastrement= self.internalForcesULS.VdEncastrement(self.stemBottomWidth)
    MdEncastrement= self.internalForcesULS.MdEncastrement(self.footingThickness)
    outputFile.write("\\textbf{Armature 1 (armature extérieure en attente) :} \\\\\n")
    NdEncastrement= 0.0 #we neglect axial force
    C1.writeResultFlexion(outputFile,NdEncastrement, MdEncastrement,VdEncastrement)
    C1.writeResultStress(outputFile,self.internalForcesSLS.MdEncastrement(self.footingThickness))

    #Coupe 2. Béton armé. Voile
    yCoupe2= self.internalForcesULS.getYVoile(self.getBasicAnchorageLength(1))
    C2= self.getSection2(yCoupe2)
    Nd2= 0.0 #we neglect axial force
    Vd2= self.internalForcesULS.Vd(yCoupe2)
    Md2= self.internalForcesULS.Md(yCoupe2)
    outputFile.write("\\textbf{Armature 2 (armature extériéure voile):}\\\\\n")
    C2.writeResultFlexion(outputFile,Nd2,Md2,Vd2)
    C2.writeResultStress(outputFile,self.internalForcesSLS.Md(yCoupe2))

    #Coupe 3. Béton armé. Semelle
    C3= self.getSection3()
    Nd3= 0.0 #we neglect axial force
    Vd3= self.internalForcesULS.VdSemelle
    Md3= self.internalForcesULS.MdSemelle
    outputFile.write("\\textbf{Armature 3 (armature supérieure semelle):}\\\\\n")
    C3.writeResultFlexion(outputFile,Nd3,Md3,Vd3)
    C3.writeResultStress(outputFile,self.internalForcesSLS.MdSemelle)

    C4= self.getSection4()
    C5= C4
    C6= self.getSection6()
    C7= self.getSection7()
    C8= self.getSection8()
    C9= C8
    C11= self.getSection11()
    C12= C11

    #Coupe 4. armature intérieure en attente. Encastrement voile 
    outputFile.write("\\textbf{Armature 4 (armature intérieure en attente):}\\\\\n")
    C4.writeResultCompression(outputFile,0.0,C12.tensionRebars.getAs())

    #Coupe 5. armature intérieure en voile.
    outputFile.write("\\textbf{Armature 5 (armature intérieure en voile):}\\\\\n")
    C5.writeResultCompression(outputFile,0.0,C12.tensionRebars.getAs())

    #Coupe 6. armature couronnement.
    outputFile.write("\\textbf{Armature 6 (armature couronnement):}\\\\\n")
    C6.writeResultFlexion(outputFile,0.0,0.0,0.0)

    #Coupe 7. armature inférieure semelle.
    outputFile.write("\\textbf{Armature 7 (armature trsv. inférieure semelle):}\\\\\n")
    C7.writeResultCompression(outputFile,0.0,C8.tensionRebars.getAs())

    #Coupe 8. armature long. inférieure semelle.
    outputFile.write("\\textbf{Armature 8 (armature long. inférieure semelle):}\\\\\n")
    C8.writeResultTraction(outputFile,0.0)

    #Coupe 9. armature long. supérieure semelle.
    outputFile.write("\\textbf{Armature 9 (armature long. supérieure semelle):}\\\\\n")
    C9.writeResultTraction(outputFile,0.0)

    #Armature 10. armature de peau semelle.
    outputFile.write("\\textbf{Armature 10 (armature de peau semelle):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #writeRebars(outputFile,self.concrete,self.reinforcement[10],1e-5)

    #Coupe 11. armature long. extérieure voile.
    outputFile.write("\\textbf{Armature 11 (armature long. extérieure voile):}\\\\\n")
    C11.writeResultTraction(outputFile,0.0)

    #Coupe 12. armature long. intérieure voile.
    outputFile.write("\\textbf{Armature 12 (armature long. intérieure voile):}\\\\\n")
    C12.writeResultTraction(outputFile,0.0)

    #Armature 13. armature long. couronnement.
    outputFile.write("\\textbf{Armature 13 (armature long. couronnement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #writeRebars(outputFile,self.concrete,self.reinforcement[13],1e-5)
    outputFile.write("\\hline\n")
    outputFile.write("\\end{supertabular}\n")
    outputFile.write("\\end{center}\n")
    outputFile.close()

  def drawSchema(self,pth):
    '''Retaining wall scheme drawing in LaTeX format.'''
    outputFile= open(pth+'schema_'+self.name+".tex","w")
    outputFile.write("\\begin{figure}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write(draw_schema.hdr)
    for l in draw_schema.lines:
      outputFile.write(l)

    defStrings= {}
    defStrings[1]= self.getSection1().tensionRebars.getDefStrings()
    yCoupe2= self.internalForcesULS.getYVoile(self.getBasicAnchorageLength(1))
    defStrings[2]= self.getSection2(yCoupe2).tensionRebars.getDefStrings()
    defStrings[3]= self.getSection3().tensionRebars.getDefStrings()
    defStrings[4]= self.getSection4().tensionRebars.getDefStrings()
    defStrings[5]= self.getSection4().tensionRebars.getDefStrings() #C5==C4
    defStrings[6]= self.getSection6().tensionRebars.getDefStrings()
    defStrings[7]= self.getSection7().tensionRebars.getDefStrings()
    defStrings[8]= self.getSection8().tensionRebars.getDefStrings()
    defStrings[9]= self.getSection8().tensionRebars.getDefStrings() #C9==C8
    #defStrings[10]= self.getSection10().tensionRebars.getDefStrings()
    defStrings[11]= self.getSection11().tensionRebars.getDefStrings()
    defStrings[12]= self.getSection11().tensionRebars.getDefStrings() #C12==C11

    rebarAnno= draw_schema.getRebarAnnotationLines(defStrings)
    for l in rebarAnno:
      outputFile.write(l)
    outputFile.write(draw_schema.tail)
    outputFile.write("\\end{center}\n")
    outputFile.write("\\caption{Schéma armatures mur "+ self.name +"} \\label{fg_"+self.name+"}\n")
    outputFile.write("\\end{figure}\n")
    
  def createFEProblem(self, title):
    self.FEModel= xc.ProblemaEF()
    self.FEModel.title= 'Retaining wall A'
    return self.FEModel
    
  def genMesh(self,nodes,springMaterials):
    self.defineWireframeModel(nodes)
    nodes.newSeedNode()
    preprocessor= self.modelSpace.preprocessor    
    trfs= preprocessor.getTransfCooLoader
    transformationName= self.name+'LinearTrf'
    self.trf= trfs.newLinearCrdTransf2d(transformationName)
    wallMatData= typical_materials.MaterialData(name=self.name+'Concrete',E=self.concrete.getEcm(),nu=0.2,rho=2500)
    foundationSection= section_properties.RectangularSection(self.name+"FoundationSection",self.b,self.footingThickness)
    foundationMaterial= foundationSection.defSeccShElastica2d(preprocessor,wallMatData) #Foundation elements material.
    elementSize= 0.2
    seedElemLoader= preprocessor.getElementLoader.seedElemLoader
    seedElemLoader.defaultMaterial= foundationSection.sectionName
    seedElemLoader.defaultTransformation= transformationName
    seedElem= seedElemLoader.newElement("elastic_beam_2d",xc.ID([0,0]))
    self.wallSet= preprocessor.getSets.defSet("wallSet")
    self.heelSet= preprocessor.getSets.defSet("heelSet")
    self.toeSet= preprocessor.getSets.defSet("toeSet")
    self.foundationSet= preprocessor.getSets.defSet("foundationSet")
    for lineName in ['heel','toe']:
      l= self.wireframeModelLines[lineName]
      l.setElemSize(elementSize)
      l.genMesh(xc.meshDir.I)
      for e in l.getElements():
        self.foundationSet.getElements.append(e)
        self.wallSet.getElements.append(e)
        if(lineName=='heel'):
          self.heelSet.getElements.append(e)
        else:
          self.toeSet.getElements.append(e)
    self.foundationSet.fillDownwards()
    
    stemSection= section_properties.RectangularSection(self.name+"StemSection",self.b,(self.stemTopWidth+self.stemBottomWidth)/2.0)
    stemMaterial= stemSection.defSeccShElastica2d(preprocessor,wallMatData) #Stem elements material.
    self.stemSet= preprocessor.getSets.defSet("stemSet")
    for lineName in ['stem']:
      l= self.wireframeModelLines[lineName]
      l.setElemSize(elementSize)
      seedElemLoader.defaultMaterial= stemSection.sectionName
      l.genMesh(xc.meshDir.I)
      for e in l.getElements():
        y= -e.getPosCentroid(True).y
        h= self.getDepth(y)
        stemSection.h= h
        e.sectionProperties= stemSection.getCrossSectionProperties2D(wallMatData)
        self.stemSet.getElements.append(e)
        self.wallSet.getElements.append(e)
    # Springs on nodes.
    self.foundationSet.computeTributaryLengths(False)
    self.fixedNodes= []
    elasticBearingNodes= self.foundationSet.getNodes
    kX= springMaterials[0] #Horizontal
    kSx= kX.E
    kY= springMaterials[1] #Vertical
    kSy= kY.E
    lngTot= 0.0
    for n in elasticBearingNodes:
      lT= n.getTributaryLength()
      lngTot+= lT
      #print "tag= ", n.tag, " lT= ", lT
      #print "before k= ", kY.E
      kX.E= kSx*lT
      kY.E= kSy*lT
      idNodoFijo, idElem= self.modelSpace.setBearing(n.tag,["kX","kY"])
      self.fixedNodes.append(nodes.getNode(idNodoFijo))
    self.stemSet.fillDownwards()
    self.wallSet.fillDownwards()

  def createSelfWeightLoads(self,rho= 2500, grav= 9.81):
    '''Create the loads of the concrete weight.'''
    for e in self.wallSet.getElements:
      selfWeightLoad= grav*rho*e.sectionProperties.A
      e.vector2dUniformLoadGlobal(xc.Vector([0.0, -selfWeightLoad]))
  def createDeadLoad(self,heelFillDepth,toeFillDepth,rho= 2000, grav= 9.81):
    '''Create the loads of earth self weigth.'''
    for e in self.heelSet.getElements:
      heelFillLoad= grav*rho*heelFillDepth
      e.vector2dUniformLoadGlobal(xc.Vector([0.0, -heelFillLoad]))
    for e in self.toeSet.getElements:
      toeFillLoad= grav*rho*toeFillDepth
      e.vector2dUniformLoadGlobal(xc.Vector([0.0, -toeFillLoad]))

  def createEarthPressureLoadOnStem(self,pressureModel,vDir= xc.Vector([-1.0,0.0]),Delta= 0.0):
    '''Create the loads of the earth pressure over the stem.

        Args:
       :pressureModel: (obj) earth pressure model.
       :vDir: (xc.Vector) direction for the pressures.
    '''
    pressureModel.appendLoadToCurrentLoadPattern(self.stemSet,vDir,iCoo= 1,delta= Delta)
    
  def createEarthPressureLoadOnHeelEnd(self,pressureModel):
    '''Create the loads of the earth pressure over the vertical face
       at the end of the heel.

        Args:
       :pressureModel: (obj) earth pressure model.
    '''
    n= self.wireframeModelPoints['heelEnd'].getNode()
    z= n.getInitialPos2d.y
    force= pressureModel.getPressure(z)*self.footingThickness
    loadVector= force*xc.Vector([-1.0,0.0,0.0])
    n.newLoad(loadVector)
    
  def createEarthPressureLoadOnToeEnd(self,pressureModel):
    '''Create the loads of the earth pressure over the vertical face
       at the end of the toe.

        Args:
       :pressureModel: (obj) earth pressure model.
    '''
    n= self.wireframeModelPoints['toeEnd'].getNode()
    z= n.getInitialPos2d.y
    force= pressureModel.getPressure(z)*self.footingThickness
    loadVector= force*xc.Vector([1.0,0.0,0.0])
    n.newLoad(loadVector)

  def createBackFillPressures(self,pressureModel,Delta= 0.0):
    '''Create backfill earth pressures over the wall.

        Args:
       :pressureModel: (obj) earth pressure model for the backfill.
    '''
    self.createEarthPressureLoadOnStem(pressureModel,Delta= Delta)
    self.createEarthPressureLoadOnHeelEnd(pressureModel)
    
  def createFrontFillPressures(self,pressureModel,Delta= 0.0):
    '''Create front fill earth pressures over the wall.

        Args:
       :pressureModel: (obj) earth pressure model for the backfill.
    '''
    self.createEarthPressureLoadOnStem(pressureModel,xc.Vector([1.0,0.0]),Delta= Delta)
    self.createEarthPressureLoadOnToeEnd(pressureModel)
    
  def createVerticalLoadOnHeel(self,loadOnBackFill):
    '''Create the loads over the heel dues to a load acting on the backfill.

        Args:
       :loadOnBackFill: (obj) load acting on the backfill.
    '''
    loadOnBackFill.appendVerticalLoadToCurrentLoadPattern(self.heelSet,xc.Vector([0.0,-1.0]),0,1)

  def createPressuresFromLoadOnBackFill(self, loadOnBackFill,Delta= 0.0):
    '''Create the pressures on the stem and on the heel dues to 
       a load acting on the backfill.

        Args:
       :loadOnBackFill: (obj) load acting on the backfill.
    '''
    self.createEarthPressureLoadOnStem(loadOnBackFill,Delta= Delta) #Pressures on stem.
    self.createEarthPressureLoadOnHeelEnd(loadOnBackFill) #Force on heel end.
    self.createVerticalLoadOnHeel(loadOnBackFill) #Vertical load on heel.
    
  def createLoadOnTopOfStem(self,loadVector):
    '''Create a loac acting on the node at the top of the stem.
        Args:
       :loadVector: (vector) vector defining the load.
    '''
    n= self.wireframeModelPoints['stemTop'].getNode()
    n.newLoad(loadVector)
  def getMononobeOkabeDryOverpressure(self,backFillModel,kv,kh,delta_ad= 0,beta= 0, Kas= None, g= 9.81):
    ''' Return overpressure due to seismic action according to Mononobe-Okabe

      Args:
      :backFillModel: back fill terrain model
      :kv: seismic coefficient of vertical acceleration.
      :kh: seismic coefficient of horizontal acceleration.
      :delta_ad: angle of friction soil - structure.
      :beta: slope inclination of backfill.
    '''
    H= self.getTotalHeight()
    psi= math.radians(90) #back face inclination of the structure (< PI/2)
    return backFillModel.getMononobeOkabeDryOverpressure(H, kv, kh, psi, delta_ad, beta, Kas,g)/H
    
  def getReactions(self):
    '''Return the reactions on the foundation.'''
    return get_reactions.Reactions(self.modelSpace.preprocessor,self.fixedNodes)

  def getEccentricity(self,R):
    '''Return the eccenctricity of the loads acting on the retaining wall.

        Args:
       :R: (SVD3d) resultant of the loads acting on the retaining wall.
    '''
    foundationPlane= self.getFoundationPlane()
    zml= R.zeroMomentLine(1e-5).getXY2DProjection() #Resultant line of action.
    p= foundationPlane.getIntersectionWithLine(zml)[0] # Intersection with
                                                       # foundation plane.
    foundationCenterPos2D= self.getFoundationCenterPosition()
    return p.x-foundationCenterPos2D.x #eccentricity

  def getFoundationRotation(self):
    '''Returns the rotation of the foundation.'''
    n0= self.wireframeModelPoints['toeEnd'].getNode()
    n1= self.wireframeModelPoints['heelEnd'].getNode()
    b= self.getFootingWidth()
    delta= n1.getDisp[1]-n0.getDisp[1]
    return math.atan(delta/b)
    
    
  def getOverturningSafetyFactor(self,R,gammaR):
    '''Return the factor of safety against overturning.

        Args:
       :R: (SVD3d) resultant of the loads acting on the retaining wall.
       :gammaR: (float) partial resistance reduction factor.
    '''
    e= self.getEccentricity(R) #eccentricity
    b= self.getFootingWidth()
    bReduced= 2*(b/2.0+e)
    return b/(3*(-e)*gammaR)

  def getSlidingSafetyFactor(self,R,gammaR,foundationSoilModel):
    '''Return the factor of safety against sliding.

        Args:
            :R: (SVD3d) resultant of the loads acting on the retaining wall.
            :gammaR: partial resistance reduction factor.
            :foundationSoilModel: (FrictionalCohesionalSoil) soil model.
            :gammaMPhi: (float) partial reduction factor for internal friction angle of the soil.
            :gammaMc: (float) partial reduction factor for soil cohesion.
    '''
    foundationPlane= self.getFoundationPlane()
    alphaAngle= math.atan(foundationPlane.getSlope())
    F= R.getResultante()
    F2D= geom.Vector2d(F.x,F.y)
    Ftang= foundationPlane.getVector2dProj(F2D)
    Fnormal= F2D-Ftang
    #Sliding strength
    e= self.getEccentricity(R) #eccentricity
    b= self.getFootingWidth()
    bReduced= 2*(b/2.0+e)
    Rd= Fnormal.getModulo()*math.tan(foundationSoilModel.getDesignPhi())+foundationSoilModel.getDesignC()*bReduced/math.cos(alphaAngle)
    return Rd/Ftang.getModulo()/gammaR   

  def getBearingPressureSafetyFactor(self,R,foundationSoilModel,toeFillDepth,q= 0.0):
    ''' Return the factor of safety against bearing capacity of the soil.

        Args:
            :toeFillDepth: (float) depht of the soil filling over the toe.
            :q: (float) uniform load over the filling.
    '''
    D= self.getFoundationDepth(toeFillDepth)
    Beff= self.b
    e= self.getEccentricity(R) #eccentricity
    b= self.getFootingWidth()
    bReduced= 2*(b/2.0+e)
    F= R.getResultante()
    qu= foundationSoilModel.qu(q,D,self.b,bReduced,F.y,0.0,F.x)
    sigma= F.y/bReduced
    return qu/sigma

  def getStemYCoordinates(self):
    y= list()
    for e in self.stemSet.getElements:
      n1= e.getNodes[0]
      y.append(n1.getInitialPos2d.y)
    n2= e.getNodes[1]
    y.append(n2.getInitialPos2d.y)
    return y

  def getStemInternalForces(self):
    md= list()
    vd= list()
    for e in self.stemSet.getElements:
      md.append(e.getMz1)
      vd.append(e.getVy1)
    md.append(e.getMz2)
    vd.append(e.getVy2)
    return md, vd

  def getHeelInternalForces(self):
    md= 1.0e15
    vd= 1.0e15
    for e in self.heelSet.getElements:
      md= min(md,e.getMz1)
      vd= min(vd,e.getVy1)
    return md, vd
  
  def resultComb(self,nmbComb):
    '''Solution and result retrieval routine.'''
    preprocessor= self.FEModel.getPreprocessor   
    preprocessor.resetLoadCase()
    preprocessor.getLoadLoader.getLoadCombinations.addToDomain(nmbComb)
    #Solution
    solution= predefined_solutions.SolutionProcedure()
    analysis= solution.simpleStaticLinear(self.FEModel)
    result= analysis.analyze(1)
    reactions= self.getReactions()
    preprocessor.getLoadLoader.getLoadCombinations.removeFromDomain(nmbComb)
    return reactions
  
  def performStabilityAnalysis(self,combinations,foundationSoilModel): 
    return WallStabilityResults(self,combinations,foundationSoilModel)

  def getEnvelopeInternalForces(self,envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel):
    md, vd= self.getStemInternalForces()
    tmpMd= [max(l1, l2) for l1, l2 in zip(envelopeMd, md)]
    envelopeMd= tmpMd
    tmpVd= [max(l1, l2) for l1, l2 in zip(envelopeVd, vd)]
    envelopeVd= tmpVd
    mdHeel, vdHeel= self.getHeelInternalForces()
    envelopeMdHeel= min(mdHeel,envelopeMdHeel)
    envelopeVdHeel= min(vdHeel,envelopeVdHeel)
    return envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel 

  def performSLSAnalysis(self,combinations):
    rotation= 1e15
    rotationComb= ''
    y= self.getStemYCoordinates()
    envelopeMd= [0]*len(y)
    envelopeVd= [0]*len(y)
    envelopeMdHeel= 1.0e15
    envelopeVdHeel= 1.0e15
    for comb in combinations:
      reactions= self.resultComb(comb)
      envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel= self.getEnvelopeInternalForces(envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel)
      rot= self.getFoundationRotation()
      if(rot<rotation):
        rotation= rot
        rotationComb= comb
    internalForces= InternalForces(y,envelopeMd, envelopeVd, abs(envelopeMdHeel), abs(envelopeVdHeel))
    return WallSLSResults(internalForces,rotation, rotationComb)

  def performULSAnalysis(self,combinations):
    y= self.getStemYCoordinates()
    envelopeMd= [0]*len(y)
    envelopeVd= [0]*len(y)
    envelopeMdHeel= 1.0e15
    envelopeVdHeel= 1.0e15
    for comb in combinations:
      reactions= self.resultComb(comb)
      envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel= self.getEnvelopeInternalForces(envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel)
    internalForces= InternalForces(y,envelopeMd, envelopeVd, abs(envelopeMdHeel), abs(envelopeVdHeel))
    return WallULSResults(internalForces)
