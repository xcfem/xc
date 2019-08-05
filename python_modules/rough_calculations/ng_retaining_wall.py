# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

'''Routines for cantilever retaining walls design.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
from postprocess.reports import common_formats as fmt
from postprocess.reports import draw_wall_reinforcement_scheme as draw_schema
from postprocess import get_reactions
import math
import scipy.interpolate
import matplotlib
import matplotlib.pyplot as plt
from materials import typical_materials
from materials.sections import section_properties
from model.geometry import retaining_wall_geometry
from rough_calculations import ng_rc_section
import os
from miscUtils import LogMessages as lmsg
import geom
import xc
from solution import predefined_solutions

def filterRepeatedValues(yList,mList,vList):
  sz= len(yList)

  mapM={}
  mapV={}
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
  def __init__(self,y,mdEnvelope,vdEnvelope,MdFooting,VdFooting):
    self.mdEnvelope= mdEnvelope
    self.vdEnvelope= vdEnvelope
    self.mdEnvelope.filterRepeatedValues()
    self.vdEnvelope.filterRepeatedValues()
    self.interpolate()
    self.stemHeight= self.y[-1]
    self.MdFooting= MdFooting
    self.VdFooting= VdFooting
  def interpolate(self):
    self.y= self.mdEnvelope.yValues
    self.mdMaxStem= scipy.interpolate.interp1d(self.y,self.mdEnvelope.positive)
    self.mdMinStem= scipy.interpolate.interp1d(self.y,self.mdEnvelope.negative)
    self.vdMaxStem= scipy.interpolate.interp1d(self.y,self.vdEnvelope.positive)    
    self.vdMinStem= scipy.interpolate.interp1d(self.y,self.vdEnvelope.negative)    
  def __imul__(self,f):
    for m in self.mdMax:
      m*=f
    for v in self.vdMax:
      v*=f
    self.interpolate()
    self.MdFooting*=f
    self.VdFooting*=f
    return self
  def clone(self):
    return InternalForces(self.mdEnvelope, self.vdEnvelope,self.MdFooting,self.VdFooting)
  def __mul__(self, f):
    retval= self.clone()
    retval*= f
    return retval
  def __rmul__(self,f):
    return self*f
  def MdMaxEncastrement(self,footingThickness):
    '''Bending moment (envelope) at stem base.'''
    yEncastrement= self.stemHeight-footingThickness/2.0
    return self.MdMax(yEncastrement)
  def VdMaxEncastrement(self,epaisseurEncastrement):
    '''Shear force (envelope) at stem base.'''
    yV= self.stemHeight-epaisseurEncastrement
    return abs(self.vdMaxStem(yV))
  def MdMaxMidStem(self,footingThickness):
    '''Max. bending moment (envelope) at the middle of the stem.'''
    yMidStem= (self.stemHeight-footingThickness/2.0)/2.0
    return self.MdMax(yMidStem)
  def VdMaxMidStem(self,footingThickness):
    '''Max. shear (envelope) at the middle of the stem.'''
    yMidStem= (self.stemHeight-footingThickness/2.0)/2.0
    return self.VdMax(yMidStem)
  def MdMinMidStem(self,footingThickness):
    '''Min. bending moment (envelope) at the middle of the stem.'''
    yMidStem= (self.stemHeight-footingThickness/2.0)/2.0
    return self.MdMin(yMidStem)
  def VdMinMidStem(self,footingThickness):
    '''Min. shear (envelope) at the middle of the stem.'''
    yMidStem= (self.stemHeight-footingThickness/2.0)/2.0
    return self.VdMin(yMidStem)
  def VdMax(self, yCoupe):
    '''Max. shear (envelope) at height yCoupe.'''
    return abs(self.vdMaxStem(yCoupe))
  def MdMax(self, yCoupe):
    '''Max. bending moment (envelope) at height yCoupe.'''
    return abs(self.mdMaxStem(yCoupe))
  def VdMin(self, yCoupe):
    '''Min. shear (envelope) at height yCoupe.'''
    return abs(self.vdMinStem(yCoupe))
  def MdMin(self, yCoupe):
    '''Min bending moment (envelope) at height yCoupe.'''
    return abs(self.mdMinStem(yCoupe))
  def getYStem(self,hCoupe):
    return self.stemHeight-hCoupe
  def writeGraphic(self,fileName):
    '''Draws a graphic of internal forces (envelopes) in
       the wall stem.'''
    z= []
    for yi in self.y:
      z.append(self.stemHeight-yi)
    mPos= []
    for mi in self.mdEnvelope.positive:
      mPos.append(mi/1e3)
    mNeg= []
    for mi in self.mdEnvelope.negative:
      mNeg.append(mi/1e3)
    vPos= []
    for vi in self.vdEnvelope.positive:
      vPos.append(vi/1e3)
    vNeg= []
    for vi in self.vdEnvelope.negative:
      vNeg.append(vi/1e3)
    plt.plot(mPos,z,'+', vPos, z,'^',mNeg,z,'-', vNeg, z,'v')
    plt.legend(['Md+ (kN m/m)', 'Vd+ (kN/m)','Md- (kN m/m)', 'Vd- (kN/m)'], loc='best')
    plt.title("Internal forces.")
    plt.savefig(fileName)
    plt.close()

class ReinforcementMap(dict):
  ''' Simplified reinforcement for a cantilever retaining wall.'''
  def __init__(self,concreteCover=40e-3, steel= None,rebarFamilyTemplate= None):
    '''Constructor '''
    super(ReinforcementMap, self).__init__()
    self.concreteCover= concreteCover
    #Materials.
    self.steel= steel
    #Default reinforcement
    for i in range(1,15):
      self[i]= rebarFamilyTemplate
    # #Footing skin reinforcement
    # R= self.footingThickness-2*self.concreteCover-8e-3
    # n= math.ceil(R/0.15)+1
    # ecart= R/(n-1)
    # self[self.skinFootingIndex]= FamNBars(self.steel,n,8e-3,ecart,concreteCover)
    # #Stem top reinforcement
    # R= self.stemTopWidth-2*self.concreteCover-8e-3
    # n= math.ceil(R/0.15)+1
    # ecart= R/(n-1)
    # self[self.topSkinIndex]= FamNBars(self.steel,n,8e-3,ecart,concreteCover)
    
  def setReinforcement(self,index,reinforcement):
    '''Set reinforcement.'''
    self[index]= reinforcement

  def getReinforcement(self,index):
    '''Return reinforcement at index.'''
    return self[index]
  
  def getBasicAnchorageLength(self,index, concrete):
    '''Returns basic anchorage length for the reinforcement at "index".''' 
    return self.getReinforcement(index).getBasicAnchorageLength(concrete)
    
class WallStabilityResults(object):
  def __init__(self,wall,combinations,foundationSoilModel,sg_adm= None, gammaR= 1):
    self.Foverturning= 1e15
    self.FoverturningComb= ''
    self.Fsliding= 1e15
    self.FslidingComb= ''
    self.Fbearing= 1e15
    self.FbearingComb= ''
    self.FadmPressure= 1e15
    self.FadmPressureComb= ''
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
      if(sg_adm):
        FadmPressure= wall.getAdmissiblePressureSafetyFactor(R,sg_adm)
        if(FadmPressure<self.FadmPressure):
          self.FadmPressure= FadmPressure
          self.FadmPressureComb= comb
  def writeOutput(self,outputFile,name):
    '''Write results in LaTeX format.'''
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{tabular}[H]{|l|c|c|c|}\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{4}{|c|}{\\textsc{wall: "+name+" stability check}}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("Vérification:  & $F_{disp}$ & $F_{req}$ & Combination\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("Overturning:  & " + fmt.Factor.format(self.Foverturning) +" & 1.00 & "+self.FoverturningComb+'\\\\\n')
    outputFile.write("Sliding:  & " + fmt.Factor.format(self.Fsliding) +" & 1.00 & "+self.FslidingComb+'\\\\\n')
    outputFile.write("Bearign capacity:  & " + fmt.Factor.format(self.Fbearing) +" & 1.00 & "+self.FbearingComb+'\\\\\n')
    if(self.FadmPressureComb!=''):
      outputFile.write("Adm. pressure:  & " + fmt.Factor.format(self.FadmPressure) +" & 1.00 & "+self.FadmPressureComb+'\\\\\n')
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{4}{|l|}{$F_{avail.}$: available security.}\\\\\n")
    outputFile.write("\\multicolumn{4}{|l|}{$F_{req}$: required security.}\\\\\n")
    
    outputFile.write("\\hline\n")
    outputFile.write("\\end{tabular}\n")
    outputFile.write("\\end{center}\n")

class WallULSResults(object):
  def __init__(self,internalForces):
    self.internalForces= internalForces

class WallSLSResults(WallULSResults):
  def __init__(self,internalForces,rotation, rotationComb):
    super(WallSLSResults,self).__init__(internalForces)
    self.rotation= rotation
    self.rotationComb= rotationComb
  def writeOutput(self,outputFile,name):
    '''Write results in LaTeX format.'''
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{tabular}[H]{|l|c|c|c|}\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{3}{|c|}{\\textsc{Wall: "+name+" rotation check}}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("$\\beta_{disp} (\\permil)$ & $\\beta_{req}(\\permil)$ & Combination\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write(fmt.Factor.format(self.rotation*1000) +" & 2.00 & "+self.rotationComb+'\\\\\n')
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{3}{|l|}{$\\beta_{disp}$: wall maximum computed rotation.}\\\\\n")
    outputFile.write("\\multicolumn{3}{|l|}{$\\beta_{req}$: wall maximum admissible rotation.}\\\\\n")
    
    outputFile.write("\\hline\n")
    outputFile.write("\\end{tabular}\n")
    outputFile.write("\\end{center}\n")

class Envelope(object):
  def __init__(self, yValues):
    '''Constructor.'''
    self.yValues= yValues
    size= len(self.yValues)
    self.positive= [-1.0e23]*size
    self.negative= [1.0e23]*size
  def __str__(self):
    retval= str(self.yValues)+'\n'
    retval+= str(self.positive)+'\n'
    retval+= str(self.negative)+'\n'
    return retval
  def update(self,values):
    '''Update envelopes.'''
    self.positive= [max(l1, l2) for l1, l2 in zip(self.positive, values)]
    self.negative= [min(l1, l2) for l1, l2 in zip(self.negative, values)]
  def filterRepeatedValues(self):
    '''Filter repeated values.'''
    sz= len(self.yValues)

    mapPos= dict()
    mapNeg= dict()
    for i in range(0,sz):
      y= abs(self.yValues[i])
      mapPos[y]= self.positive[i]
      mapNeg[y]= self.negative[i]

    retY= list()
    retPos= list()
    for y in mapPos:
      retY.append(y)
    retY.sort()
    for y in retY:
      retPos.append(mapPos[y])
    self.positive= retPos
      
    retY= list()
    retNeg= list()
    for y in mapNeg:
      retY.append(y)
    retY.sort()
    for y in retY:
      retNeg.append(mapNeg[y])
    self.negative= retNeg
    self.yValues= retY

class StemReinforcement(ReinforcementMap):
  '''Stem reinforcement. '''
  extStemBottomIndex= 1 # exterior reinforcement at stem bottom.
  extStemIndex= 2 # exterior reinforcement at stem.
  intStemBottomIndex= 4 # interior reinforcement at stem.
  intStemIndex= 5 # interior reinforcement at stem.
  topStemIndex= 6 # reinforcement at stem top.
  longExtStemIndex= 11 # exterior longitudinal reinforcement at stem.
  longIntStemIndex= 12 # interior longitudinal reinforcement at stem.
  topSkinIndex= 13 # longitudinal skin reinforcement at stem top.

  def __init__(self,wallGeom, concreteCover, steel):
    '''Constructor.'''
    super(StemReinforcement,self).__init__(concreteCover, steel)
    self.wallGeom= wallGeom
        
  def getSectionExtStemBottom(self):
    '''Returns RC section for exterior reinforcement at stem bottom.'''
    return ng_rc_section.RCSection(self[self.extStemBottomIndex],self.wallGeom.concrete,self.wallGeom.b,self.wallGeom.stemBottomWidth)
  def getSectionExtStem(self,y):
    '''Returns RC section for exterior reinforcement at stem.'''
    c= self.wallGeom.getDepth(y)
    return ng_rc_section.RCSection(self[self.extStemIndex],self.wallGeom.concrete,self.wallGeom.b,c)
  def getSectionIntStemBottom(self):
    '''Returns RC section for interior reinforcement at stem bottom.'''
    return ng_rc_section.RCSection(self[self.intStemBottomIndex],self.wallGeom.concrete, self.wallGeom.b,self.wallGeom.stemBottomWidth)
  def getSectionStemTop(self):
    '''Returns RC section for reinforcement at stem top.'''
    return ng_rc_section.RCSection(self[self.topStemIndex],self.wallGeom.concrete, self.wallGeom.b,self.wallGeom.stemTopWidth)
  def getSectionStemLongExt(self):
    '''Returns RC section for loingitudinal reinforcement in stem exterior.'''
    return ng_rc_section.RCSection(self[self.longExtStemIndex],self.wallGeom.concrete,self.wallGeom.b,(self.wallGeom.stemTopWidth+self.wallGeom.stemBottomWidth)/2.0)
  def getSectionStemLongInt(self):
    '''Returns RC section for loingitudinal reinforcement in stem interior.'''
    return ng_rc_section.RCSection(self[self.longIntStemIndex],self.wallGeom.concrete,self.wallGeom.b,(self.wallGeom.stemTopWidth+self.wallGeom.stemBottomWidth)/2.0)
  def writeResult(self, outputFile):
    '''Write stem reinforcement verification results in LaTeX format.'''

    # Exterior reinforcement in stem bottom.
    CExtStemBottom= self.getSectionExtStemBottom()
    VdMaxEncastrement= self.wallGeom.internalForcesULS.VdMaxEncastrement(self.wallGeom.stemBottomWidth)
    MdMaxEncastrement= self.wallGeom.internalForcesULS.MdMaxEncastrement(self.wallGeom.footingThickness)
    outputFile.write("\\textbf{Reinforcement "+str(self.extStemBottomIndex)+" (ouside reinforcement dowels) :} \\\\\n")
    NdEncastrement= 0.0 #we neglect axial force
    CExtStemBottom.writeResultFlexion(outputFile,NdEncastrement, MdMaxEncastrement,VdMaxEncastrement)
    CExtStemBottom.writeResultStress(outputFile,MdMaxEncastrement)

    # Exterior reinforcement in stem.
    yCoupeExtStem= self.wallGeom.internalForcesULS.getYStem(self.getBasicAnchorageLength(self.extStemBottomIndex, self.wallGeom.concrete))
    CExtStem= self.getSectionExtStem(yCoupeExtStem)
    NdExtStem= 0.0 #we neglect axial force
    VdExtStem= self.wallGeom.internalForcesULS.VdMax(yCoupeExtStem)
    MdExtStem= self.wallGeom.internalForcesULS.MdMax(yCoupeExtStem)
    outputFile.write("\\textbf{Reinforcement "+str(self.extStemIndex)+" (outside stem reinforcement):}\\\\\n")
    CExtStem.writeResultFlexion(outputFile,NdExtStem,MdExtStem,VdExtStem)
    CExtStem.writeResultStress(outputFile,MdExtStem)

    # Interior reinforcement at stem bottom 
    CIntStemBottom= self.getSectionIntStemBottom()
    CIntStem= CIntStemBottom
    CSectionStemTop= self.getSectionStemTop()
    CSectionStemLongExt= self.getSectionStemLongExt()
    CSectionStemLongInt= CSectionStemLongExt

    outputFile.write("\\textbf{Reinforcement "+str(self.intStemBottomIndex)+" (inside reinforcement dowels):}\\\\\n")
    CIntStemBottom.writeResultCompression(outputFile,0.0,CSectionStemLongInt.tensionRebars.getAs())

    # Exterior reinforcement at stem.
    outputFile.write("\\textbf{Reinforcement "+str(self.intStemIndex)+" (inside vertical stem reinforcement):}\\\\\n")
    CIntStem.writeResultCompression(outputFile,0.0,CSectionStemLongInt.tensionRebars.getAs())

    # Reinforcement at stem top.
    outputFile.write("\\textbf{Reinforcement "+str(self.topStemIndex)+" (top stem reinforcement):}\\\\\n")
    CSectionStemTop.writeResultFlexion(outputFile,0.0,0.0,0.0)

    # Stem exterior longitudinal reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.longExtStemIndex)+" (outside stem longitudinal reinforcement):}\\\\\n")
    CSectionStemLongExt.writeResultTraction(outputFile,0.0)

    # Stem interior longitudinal reinforcement.
    outputFile.write("\\textbf{Reinforcement  "+str(self.longIntStemIndex)+" (inside stem longitudinal reinforcement):}\\\\\n")
    CSectionStemLongInt.writeResultTraction(outputFile,0.0)

    # Stem top skin reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.topSkinIndex)+" (longitudinal stem top reinforcement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #self[self.topSkinIndex].writeRebars(outputFile,self.wallGeom.concrete,1e-5)
    
  def drawSchema(self,defStrings):
    '''Stem data for wall scheme drawing in LaTeX format.'''
    
    defStrings[self.extStemBottomIndex]= self.getSectionExtStemBottom().tensionRebars.getDefStrings()
    yCoupeExtStem= self.wallGeom.internalForcesULS.getYStem(self.getBasicAnchorageLength(self.extStemBottomIndex, self.wallGeom.concrete))
    defStrings[self.extStemIndex]= self.getSectionExtStem(yCoupeExtStem).tensionRebars.getDefStrings()
    defStrings[self.intStemBottomIndex]= self.getSectionIntStemBottom().tensionRebars.getDefStrings()
    defStrings[self.intStemIndex]= self.getSectionIntStemBottom().tensionRebars.getDefStrings() #CIntStem==CIntStemBottom
    defStrings[self.topStemIndex]= self.getSectionStemTop().tensionRebars.getDefStrings()
    defStrings[self.longExtStemIndex]= self.getSectionStemLongExt().tensionRebars.getDefStrings()
    defStrings[self.longIntStemIndex]= defStrings[self.longExtStemIndex]#self.getSectionStemLongInt().tensionRebars.getDefStrings() #CSectionStemLongInt==CSectionStemLongExt

class FootingReinforcement(ReinforcementMap):
  '''Footing reinforcement. '''
  topFootingIndex= 3 # transverse reinforcement at wall footing top.
  bottomFootingIndex= 7 # transverse reinforcement at wall footing bottom.
  longBottomFootingIndex= 8 # longitudinal reinforcement at wall footing bottom.
  longTopFootingIndex= 9 # longitudinal reinforcement at wall footing bottom.
  skinFootingIndex= 10 # skin reinforcement at wall footing.

  def __init__(self,wallGeom, concreteCover, steel):
    '''Constructor.'''
    super(FootingReinforcement,self).__init__(concreteCover, steel)
    self.wallGeom= wallGeom

  def getSectionTopFooting(self):
    '''Returns RC section for reinforcement on footing top.'''
    return ng_rc_section.RCSection(self[self.topFootingIndex],self.wallGeom.concrete,self.wallGeom.b,self.wallGeom.footingThickness)
  def getSectionFootingBottom(self):
    '''Returns RC section for reinforcement at footing bottom.'''
    return ng_rc_section.RCSection(self[self.bottomFootingIndex],self.wallGeom.concrete,self.wallGeom.b,self.wallGeom.footingThickness)
  def getSectionFootingBottomLongitudinal(self):
    '''Returns RC section for longitudinal reinforcement at footing bottom.'''
    return ng_rc_section.RCSection(self[self.longBottomFootingIndex],self.wallGeom.concrete,self.wallGeom.b,self.wallGeom.footingThickness)
  def writeResult(self, outputFile):
    '''Write reinforcement verification results in LaTeX format.'''
    
    # Reinforcement on footing top
    CTopFooting= self.getSectionTopFooting()
    NdTopFooting= 0.0 #we neglect axial force
    VdTopFooting= self.wallGeom.internalForcesULS.VdFooting
    MdTopFooting= self.wallGeom.internalForcesULS.MdFooting
    outputFile.write("\\textbf{Reinforcement "+str(self.topFootingIndex)+" (footing top reinforcement):}\\\\\n")
    CTopFooting.writeResultFlexion(outputFile,NdTopFooting,MdTopFooting,VdTopFooting)
    CTopFooting.writeResultStress(outputFile,self.wallGeom.internalForcesSLS.MdFooting)

    CSectionFootingBottom= self.getSectionFootingBottom()
    CSectionFootingBottomLongitudinal= self.getSectionFootingBottomLongitudinal()
    CSectionFootingTopLongitudinal= CSectionFootingBottomLongitudinal

    # Reinforcemnt at footing bottom.
    outputFile.write("\\textbf{Reinforcement "+str(self.bottomFootingIndex)+" (footing bottom transverse reinforcement):}\\\\\n")
    CSectionFootingBottom.writeResultCompression(outputFile,0.0,CSectionFootingBottomLongitudinal.tensionRebars.getAs())

    # Longitudinal reinforcement at footing bottom.
    outputFile.write("\\textbf{Reinforcement "+str(self.longBottomFootingIndex)+" (footing bottom longitudinal reinforcement):}\\\\\n")
    CSectionFootingBottomLongitudinal.writeResultTraction(outputFile,0.0)

    # Longitudinal reinforcement at footing top.
    outputFile.write("\\textbf{Reinforcement "+str(self.longTopFootingIndex)+" (footing top longitudinal reinforcement):}\\\\\n")
    CSectionFootingTopLongitudinal.writeResultTraction(outputFile,0.0)

    # Footing skin reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.skinFootingIndex)+" (foogint skin reinforcement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #self[self.skinFootingIndex].writeRebars(outputFile,self.wallGeom.concrete,1e-5)

  def drawSchema(self,defStrings):
    '''Footing scheme drawing in LaTeX format.'''

    defStrings[self.topFootingIndex]= self.getSectionTopFooting().tensionRebars.getDefStrings()
    defStrings[self.bottomFootingIndex]= self.getSectionFootingBottom().tensionRebars.getDefStrings()
    defStrings[self.longBottomFootingIndex]= self.getSectionFootingBottomLongitudinal().tensionRebars.getDefStrings()
    defStrings[self.longTopFootingIndex]= self.getSectionFootingBottomLongitudinal().tensionRebars.getDefStrings() #CSectionFootingTopLongitudinal==CSectionFootingBottomLongitudinal
    #defStrings[self.skinFootingIndex]= self.getSectionFootingSkin().tensionRebars.getDefStrings()

  
class RetainingWall(retaining_wall_geometry.CantileverRetainingWallGeometry):
  '''Cantilever retaining wall.'''
  b= 1.0

  
  def __init__(self,name= 'prb',concreteCover=40e-3,stemBottomWidth=0.25,stemTopWidth=0.25,footingThickness= 0.25, concrete= None, steel= None):
    '''Constructor '''
    super(RetainingWall,self).__init__(name,stemBottomWidth,stemTopWidth,footingThickness)
    #Materials.
    self.concrete= concrete
    self.stemReinforcement= StemReinforcement(self,concreteCover, steel)
    self.footingReinforcement= FootingReinforcement(self,concreteCover, steel)
    
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
    pathFiguraJPEG= pth+self.name+".jpeg"
    #self.internalForcesULS.writeGraphic(pathFiguraEPS)
    self.internalForcesULS.writeGraphic(pathFiguraJPEG)
    os.system("convert "+pathFiguraJPEG+" "+pathFiguraEPS)
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
    outputFile.write("\\multicolumn{3}{c}{\\textsc{Materials}}\\\\\n")
    outputFile.write("  Concrete: " + self.concrete.materialName +" & ")
    outputFile.write("  Steel: " + self.stemReinforcement.steel.materialName +" & ")
    outputFile.write("  Concrete cover: "+ fmt.Diam.format(self.stemReinforcement.concreteCover*1e3)+ " mm\\\\\n")
    outputFile.write("\\end{tabular} \\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\end{tabular}\n")
    outputFile.write("\\caption{Wall materials and dimensions "+ self.name +"} \\label{tb_def_"+self.name+"}\n")
    outputFile.write("\\end{center}\n")
    outputFile.write("\\end{table}\n")

  def writeResult(self,pth):
    '''Write reinforcement verification results in LaTeX format.'''
    outputFile= open(pth+self.name+".tex","w")
    self.writeDef(pth,outputFile)
    self.stability_results.writeOutput(outputFile,self.name)
    self.sls_results.writeOutput(outputFile,self.name)
    outputFile.write("\\bottomcaption{Wall "+ self.name +" reinforcement} \\label{tb_"+self.name+"}\n")
    outputFile.write("\\tablefirsthead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{Reinforcements mur "+self.name+"}}\\\\\\hline\n}\n")
    outputFile.write("\\tablehead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{"+self.name+" (suite)}}\\\\\\hline\n}\n")
    outputFile.write("\\tabletail{\\hline \\multicolumn{1}{|r|}{../..}\\\\\\hline}\n")
    outputFile.write("\\tablelasttail{\\hline}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{supertabular}[H]{|l|}\n")
    outputFile.write("\\hline\n")

    self.stemReinforcement.writeResult(outputFile)
    self.footingReinforcement.writeResult(outputFile)

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
    self.stemReinforcement.drawSchema(defStrings)
    self.footingReinforcement.drawSchema(defStrings)
    
    rebarAnno= draw_schema.getRebarAnnotationLines(defStrings)
    for l in rebarAnno:
      outputFile.write(l)
    outputFile.write(draw_schema.tail)
    outputFile.write("\\end{center}\n")
    outputFile.write("\\caption{Wall "+ self.name +" reinforcement scheme} \\label{fg_"+self.name+"}\n")
    outputFile.write("\\end{figure}\n")
    
  def createFEProblem(self, title):
    '''Create finite element problem.'''
    self.feProblem= xc.FEProblem()
    self.feProblem.title= title
    return self.feProblem
    
  def genMesh(self,nodes,springMaterials):
    '''Generate finite element mesh.'''
    self.defineWireframeModel(nodes)
    nodes.newSeedNode()
    preprocessor= self.modelSpace.preprocessor    
    trfs= preprocessor.getTransfCooHandler
    transformationName= self.name+'LinearTrf'
    self.trf= trfs.newLinearCrdTransf2d(transformationName)
    wallMatData= typical_materials.MaterialData(name=self.name+'Concrete',E=self.concrete.getEcm(),nu=0.2,rho=2500)
    foundationSection= section_properties.RectangularSection(self.name+"FoundationSection",self.b,self.footingThickness)
    foundationMaterial= foundationSection.defElasticShearSection2d(preprocessor,wallMatData) #Foundation elements material.
    elementSize= 0.2
    seedElemHandler= preprocessor.getElementHandler.seedElemHandler
    seedElemHandler.defaultMaterial= foundationSection.sectionName
    seedElemHandler.defaultTransformation= transformationName
    seedElem= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))
    self.wallSet= preprocessor.getSets.defSet("wallSet")
    self.heelSet= preprocessor.getSets.defSet("heelSet")
    self.toeSet= preprocessor.getSets.defSet("toeSet")
    self.foundationSet= preprocessor.getSets.defSet("foundationSet")
    for lineName in ['heel','toe']:
      l= self.wireframeModelLines[lineName]
      l.setElemSize(elementSize)
      l.genMesh(xc.meshDir.I)
      for e in l.elements:
        self.foundationSet.elements.append(e)
        self.wallSet.elements.append(e)
        if(lineName=='heel'):
          self.heelSet.elements.append(e)
        else:
          self.toeSet.elements.append(e)
    self.foundationSet.fillDownwards()
    
    stemSection= section_properties.RectangularSection(self.name+"StemSection",self.b,(self.stemTopWidth+self.stemBottomWidth)/2.0)
    stemMaterial= stemSection.defElasticShearSection2d(preprocessor,wallMatData) #Stem elements material.
    self.stemSet= preprocessor.getSets.defSet("stemSet")
    for lineName in ['stem']:
      l= self.wireframeModelLines[lineName]
      l.setElemSize(elementSize)
      seedElemHandler.defaultMaterial= stemSection.sectionName
      l.genMesh(xc.meshDir.I)
      for e in l.elements:
        y= -e.getPosCentroid(True).y
        h= self.getDepth(y)
        stemSection.h= h
        e.sectionProperties= stemSection.getCrossSectionProperties2D(wallMatData)
        self.stemSet.elements.append(e)
        self.wallSet.elements.append(e)
    # Springs on nodes.
    self.foundationSet.computeTributaryLengths(False)
    self.fixedNodes= []
    elasticBearingNodes= self.foundationSet.nodes
    kX= springMaterials[0] #Horizontal
    kSx= kX.E
    kY= springMaterials[1] #Vertical
    kSy= kY.E
    lngTot= 0.0
    for n in elasticBearingNodes:
      lT= n.getTributaryLength()
      lngTot+= lT
      #print("tag= ", n.tag, " lT= ", lT)
      #print("before k= ", kY.E)
      kX.E= kSx*lT
      kY.E= kSy*lT
      fixedNode, newElem= self.modelSpace.setBearing(n.tag,["kX","kY"])
      self.fixedNodes.append(fixedNode)
    self.stemSet.fillDownwards()
    self.wallSet.fillDownwards()

  def createSelfWeightLoads(self,rho= 2500, grav= 9.81):
    '''Create the loads of the concrete weight.'''
    for e in self.wallSet.elements:
      selfWeightLoad= xc.Vector([0.0, -grav*rho*e.sectionProperties.A])
      e.vector2dUniformLoadGlobal(selfWeightLoad)
  def createDeadLoad(self,heelFillDepth,toeFillDepth,rho= 2000, grav= 9.81):
    '''Create the loads of earth self weigth.'''
    heelFillLoad= xc.Vector([0.0, -grav*rho*heelFillDepth])
    for e in self.heelSet.elements:
      e.vector2dUniformLoadGlobal(heelFillLoad)
    toeFillLoad= xc.Vector([0.0, -grav*rho*toeFillDepth])
    for e in self.toeSet.elements:
      e.vector2dUniformLoadGlobal(toeFillLoad)

  def createEarthPressureLoadOnStem(self,pressureModel,vDir= xc.Vector([-1.0,0.0]),Delta= 0.0):
    '''Create the loads of the earth pressure over the stem.

       :param pressureModel: (obj) earth pressure model.
       :param vDir: (xc.Vector) direction for the pressures.
    '''
    pressureModel.appendLoadToCurrentLoadPattern(self.stemSet,vDir,iCoo= 1,delta= Delta)
    
  def createEarthPressureLoadOnHeelEnd(self,pressureModel):
    '''Create the loads of the earth pressure over the vertical face
       at the end of the heel.

       :param pressureModel: (obj) earth pressure model.
    '''
    n= self.wireframeModelPoints['heelEnd'].getNode()
    z= n.getInitialPos2d.y
    force= pressureModel.getPressure(z)*self.footingThickness
    loadVector= force*xc.Vector([-1.0,0.0,0.0])
    n.newLoad(loadVector)
    
  def createEarthPressureLoadOnToeEnd(self,pressureModel):
    '''Create the loads of the earth pressure over the vertical face
       at the end of the toe.

       :param pressureModel: (obj) earth pressure model.
    '''
    n= self.wireframeModelPoints['toeEnd'].getNode()
    z= n.getInitialPos2d.y
    force= pressureModel.getPressure(z)*self.footingThickness
    loadVector= force*xc.Vector([1.0,0.0,0.0])
    n.newLoad(loadVector)

  def createBackFillPressures(self,pressureModel,Delta= 0.0):
    '''Create backfill earth pressures over the wall.

       :param pressureModel: (obj) earth pressure model for the backfill.
    '''
    self.createEarthPressureLoadOnStem(pressureModel,Delta= Delta)
    self.createEarthPressureLoadOnHeelEnd(pressureModel)
    
  def createFrontFillPressures(self,pressureModel,Delta= 0.0):
    '''Create front fill earth pressures over the wall.

       :param pressureModel: (obj) earth pressure model for the backfill.
    '''
    self.createEarthPressureLoadOnStem(pressureModel,xc.Vector([1.0,0.0]),Delta= Delta)
    self.createEarthPressureLoadOnToeEnd(pressureModel)
    
  def createVerticalLoadOnHeel(self,loadOnBackFill):
    '''Create the loads over the heel dues to a load acting on the backfill.

       :param loadOnBackFill: (obj) load acting on the backfill.
    '''
    loadOnBackFill.appendVerticalLoadToCurrentLoadPattern(self.heelSet,xc.Vector([0.0,-1.0]),0,1)

  def createPressuresFromLoadOnBackFill(self, loadOnBackFill,Delta= 0.0):
    '''Create the pressures on the stem and on the heel dues to 
       a load acting on the backfill.

       :param loadOnBackFill: (obj) load acting on the backfill.
    '''
    self.createEarthPressureLoadOnStem(loadOnBackFill,Delta= Delta) #Pressures on stem.
    self.createEarthPressureLoadOnHeelEnd(loadOnBackFill) #Force on heel end.
    self.createVerticalLoadOnHeel(loadOnBackFill) #Vertical load on heel.
    
  def createLoadOnTopOfStem(self,loadVector):
    '''Create a loac acting on the node at the top of the stem.

       :param loadVector: (vector) vector defining the load.
    '''
    n= self.wireframeModelPoints['stemTop'].getNode()
    n.newLoad(loadVector)
  def getMononobeOkabeDryOverpressure(self,backFillModel,kv,kh,delta_ad= 0,beta= 0, Kas= None, g= 9.81):
    ''' Return overpressure due to seismic action according to Mononobe-Okabe

      :param backFillModel: back fill terrain model
      :param kv: seismic coefficient of vertical acceleration.
      :param kh: seismic coefficient of horizontal acceleration.
      :param delta_ad: angle of friction soil - structure.
      :param beta: slope inclination of backfill.
    '''
    H= self.getTotalHeight()
    psi= math.radians(90) #back face inclination of the structure (< PI/2)
    return backFillModel.getMononobeOkabeDryOverpressure(H, kv, kh, psi, delta_ad, beta, Kas,g)/H
    
  def getReactions(self):
    '''Return the reactions on the foundation.'''
    return get_reactions.Reactions(self.modelSpace.preprocessor,self.fixedNodes)

  def getEccentricity(self,R):
    '''Return the eccenctricity of the loads acting on the retaining wall.

     :param R: (SlidingVectorsSystem3d) resultant of the loads acting on the retaining wall.
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

     :param R: (SlidingVectorsSystem3d) resultant of the loads acting on the retaining wall.
     :param gammaR: (float) partial resistance reduction factor.
    '''
    e= self.getEccentricity(R) #eccentricity
    b= self.getFootingWidth()
    bReduced= 2*(b/2.0+e)
    return b/(3*(-e)*gammaR)

  def getSlidingSafetyFactor(self,R,gammaR,foundationSoilModel):
    '''Return the factor of safety against sliding.

     :param R: (SlidingVectorsSystem3d) resultant of the loads acting on the retaining wall.
     :param gammaR: partial resistance reduction factor.
     :param foundationSoilModel: (FrictionalCohesionalSoil) soil model.
     :param gammaMPhi: (float) partial reduction factor for internal friction angle of the soil.
     :param gammaMc: (float) partial reduction factor for soil cohesion.
    '''
    foundationPlane= self.getFoundationPlane()
    alphaAngle= math.atan(foundationPlane.getSlope())
    F= R.getResultant()
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

     :param R: force on the bearing plane.
     :param foundationSoilModel: soil model for the Brinch-Hansen analysis.
     :param toeFillDepth: (float) depht of the soil filling over the toe.
     :param q: (float) uniform load over the filling.
    '''
    D= self.getFoundationDepth(toeFillDepth)
    Beff= self.b
    e= self.getEccentricity(R) #eccentricity
    b= self.getFootingWidth()
    bReduced= 2*(b/2.0+e)
    F= R.getResultant()
    qu= foundationSoilModel.qu(q,D,self.b,bReduced,F.y,0.0,F.x)
    sigma= F.y/bReduced
    return qu/sigma
  
  def getAdmissiblePressureSafetyFactor(self,R,sg_adm):
    ''' Return the factor of safety against bearing capacity of the soil.

     :param R: force on the bearing plane.
     :param toeFillDepth: (float) depht of the soil filling over the toe.
     :param q_adm: (float) admissible bearing pressure.
    '''
    Beff= self.b
    e= self.getEccentricity(R) #eccentricity
    b= self.getFootingWidth()
    bReduced= 2*(b/2.0+e)
    F= R.getResultant()
    sigma= F.y/bReduced
    return sg_adm/sigma

  def getStemYCoordinates(self):
    y= list()
    for e in self.stemSet.elements:
      n1= e.getNodes[0]
      y.append(n1.getInitialPos2d.y)
    n2= e.getNodes[1]
    y.append(n2.getInitialPos2d.y)
    return y

  def getStemInternalForces(self):
    md= list()
    vd= list()
    for e in self.stemSet.elements:
      md.append(e.getMz1)
      vd.append(e.getVy1)
    md.append(e.getMz2)
    vd.append(e.getVy2)
    return md, vd

  def getHeelInternalForces(self):
    md= 1.0e15
    vd= 1.0e15
    for e in self.heelSet.elements:
      md= min(md,e.getMz1)
      vd= min(vd,e.getVy1)
    return md, vd
  
  def resultComb(self,nmbComb):
    '''Solution and result retrieval routine.'''
    preprocessor= self.feProblem.getPreprocessor   
    preprocessor.resetLoadCase()
    preprocessor.getLoadHandler.getLoadCombinations.addToDomain(nmbComb)
    #Solution
    solution= predefined_solutions.SolutionProcedure()
    analysis= solution.simpleStaticLinear(self.feProblem)
    result= analysis.analyze(1)
    reactions= self.getReactions()
    preprocessor.getLoadHandler.getLoadCombinations.removeFromDomain(nmbComb)
    return reactions
  
  def performStabilityAnalysis(self,combinations,foundationSoilModel, sg_adm= None): 
    self.stability_results= WallStabilityResults(self, combinations, foundationSoilModel, sg_adm)
    return self.stability_results

  def getEnvelopeInternalForces(self,envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel):
    md, vd= self.getStemInternalForces()
    envelopeMd.update(md)
    envelopeVd.update(vd)
    mdHeel, vdHeel= self.getHeelInternalForces()
    envelopeMdHeel.update([mdHeel])
    envelopeVdHeel.update([vdHeel])
    return envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel 

  def performSLSAnalysis(self,combinations):
    rotation= 1e15
    rotationComb= ''
    y= self.getStemYCoordinates()
    envelopeMd= Envelope(y)
    envelopeVd= Envelope(y)
    envelopeMdHeel= Envelope([0.0])
    envelopeVdHeel= Envelope([0.0])
    for comb in combinations:
      reactions= self.resultComb(comb)
      envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel= self.getEnvelopeInternalForces(envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel)
      rot= self.getFoundationRotation()
      if(rot<rotation):
        rotation= rot
        rotationComb= comb
    internalForces= InternalForces(y,envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel)
    self.sls_results= WallSLSResults(internalForces,rotation, rotationComb)
    return self.sls_results

  def performULSAnalysis(self,combinations):
    y= self.getStemYCoordinates()
    envelopeMd= Envelope(y)
    envelopeVd= Envelope(y)
    envelopeMdHeel= Envelope([0.0])
    envelopeVdHeel= Envelope([0.0])
    for comb in combinations:
      reactions= self.resultComb(comb)
      envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel= self.getEnvelopeInternalForces(envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel)
    internalForces= InternalForces(y,envelopeMd, envelopeVd, envelopeMdHeel, envelopeVdHeel)
    self.uls_results= WallULSResults(internalForces)
    return self.uls_results

