# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

'''Routines for cantilever retaining walls design.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from rough_calculations import ng_retaining_wall as rw
import xc

class BasementWall(rw.RetainingWall):
  '''Basement retaining wall (propped cantilever).'''
  def __init__(self,name= 'prb',concreteCover=40e-3,stemBottomWidth=0.25,stemTopWidth=0.25,footingThickness= 0.25, concrete= None, steel= None):
    '''Constructor '''
    super(BasementWall,self).__init__(name,concreteCover, stemBottomWidth,stemTopWidth,footingThickness, concrete, steel)

  def getSectionIntStem(self):
    '''Returns RC section for reinforcement at stem interior.'''
    return ng_rc_section.RCSection(self.reinforcement[self.intStemIndex],self.concrete,self.b,self.stemBottomWidth)

  def genMesh(self,nodes,springMaterials):
    '''Generate finite element mesh.'''
    super(BasementWall,self).genMesh(nodes, springMaterials)
    stemLine= self.wireframeModelLines['stem']
    # pin stem head
    headNode= stemLine.lastNode
    headNode.fix(xc.ID([0]),xc.Vector([0.0]))

  def writeResult(self,pth):
    '''Write reinforcement verification results in LaTeX format.'''
    outputFile= open(pth+self.name+".tex","w")
    self.writeDef(pth,outputFile)
    self.stability_results.writeOutput(outputFile,self.name)
    self.sls_results.writeOutput(outputFile,self.name)
    outputFile.write("\\bottomcaption{Calcul armatures mur "+ self.name +"} \\label{tb_"+self.name+"}\n")
    outputFile.write("\\tablefirsthead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{Reinforcements mur "+self.name+"}}\\\\\\hline\n}\n")
    outputFile.write("\\tablehead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{"+self.name+" (suite)}}\\\\\\hline\n}\n")
    outputFile.write("\\tabletail{\\hline \\multicolumn{1}{|r|}{../..}\\\\\\hline}\n")
    outputFile.write("\\tablelasttail{\\hline}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{supertabular}[H]{|l|}\n")
    outputFile.write("\\hline\n")

    #Coupe ExtStemBottomIndex. Reinforced concrete. Encastrement.
    CExtStemBottom= self.getSectionExtStemBottom()
    VdMaxEncastrement= self.internalForcesULS.VdMaxEncastrement(self.stemBottomWidth)
    MdMaxEncastrement= self.internalForcesULS.MdMaxEncastrement(self.footingThickness)
    outputFile.write("\\textbf{Reinforcement " + str(extStemBottomIndex) + " (armature extérieure en attente) :} \\\\\n")
    NdEncastrement= 0.0 #we neglect axial force
    CExtStemBottom.writeResultFlexion(outputFile,NdEncastrement, MdMaxEncastrement,VdMaxEncastrement)
    CExtStemBottom.writeResultStress(outputFile,MdMaxEncastrement)

    # Exterior rebars in stem.
    yCoupeExtStem= self.internalForcesULS.getYStem(self.getBasicAnchorageLength(self.extStemBottomIndex))
    CExtStem= self.getSectionExtStem(yCoupeExtStem)

    # Rebars on footing top
    CTopFooting= self.getSectionTopFooting()
    NdTopFooting= 0.0 #we neglect axial force
    VdTopFooting= max(abs(self.internalForcesULS.VdSemelle.positive[0]),abs(self.internalForcesULS.VdSemelle.negative[0]))
    MdTopFooting= max(abs(self.internalForcesULS.MdSemelle.positive[0]),abs(self.internalForcesULS.MdSemelle.negative[0]))
    outputFile.write("\\textbf{Reinforcement "+str(self.topFootingIndex)+" (armature supérieure semelle):}\\\\\n")
    CTopFooting.writeResultFlexion(outputFile,NdTopFooting,MdTopFooting,VdTopFooting)
    CTopFooting.writeResultStress(outputFile,MdTopFooting)

    CIntStemBottom= self.getSectionIntStemBottom()
    
    CSectionStemTop= self.getSectionStemTop()
    CSectionFootingBottom= self.getSectionFootingBottom()
    CSectionFootingBottomLongitudinal= self.getSectionFootingBottomLongitudinal()
    CSectionFootingTopLongitudinal= CSectionFootingBottomLongitudinal
    CSectionStemLongExt= self.getSectionStemLongExt()
    CSectionStemLongInt= CSectionStemLongExt

    # Interior rebars at stem bottom 
    outputFile.write("\\textbf{Reinforcement "+str(self.intStemBottomIndex)+" (armature intérieure en attente):}\\\\\n")
    CIntStemBottom.writeResultCompression(outputFile,0.0,CSectionStemLongInt.tensionRebars.getAs())

    # Interior rebars at stem.
    outputFile.write("\\textbf{Reinforcement "+str(self.intStemIndex)+" (armature intérieure en voile):}\\\\\n")
    CIntStem= self.getSectionIntStem()
    VdMinMidStem= self.internalForcesULS.VdMinMidStem(self.stemBottomWidth)
    MdMinMidStem= self.internalForcesULS.MdMinMidStem(self.footingThickness)
    NdMidStem= 0.0 #we neglect axial force
    CIntStem.writeResultFlexion(outputFile,NdMidStem, MdMinMidStem,VdMinMidStem)
    CIntStem.writeResultStress(outputFile,MdMinMidStem)

    # Reinforcement at stem top.
    outputFile.write("\\textbf{Reinforcement "+str(self.topStemIndex)+" (armature couronnement):}\\\\\n")
    CSectionStemTop.writeResultFlexion(outputFile,0.0,0.0,0.0)

    # Reinforcement at footing bottom.
    outputFile.write("\\textbf{Reinforcement "+str(self.bottomFootingIndex)+" (armature trsv. inférieure semelle):}\\\\\n")
    CSectionFootingBottom.writeResultCompression(outputFile,0.0,CSectionFootingBottomLongitudinal.tensionRebars.getAs())

    # Longitudinal reinforcement at footing bottom.
    outputFile.write("\\textbf{Reinforcement "+str(self.longBottomFootingIndex)+" (armature long. inférieure semelle):}\\\\\n")
    CSectionFootingBottomLongitudinal.writeResultTraction(outputFile,0.0)

    # Longitudinal reinforcement at footing top.
    outputFile.write("\\textbf{Reinforcement "+str(self.longTopFootingIndex)+" (armature long. supérieure semelle):}\\\\\n")
    CSectionFootingTopLongitudinal.writeResultTraction(outputFile,0.0)

    # Footing skin reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.skinFootingIndex)+" (armature de peau semelle):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #self.reinforcement[self.skinFootingIndex].writeRebars(outputFile,self.concrete,1e-5)

    # Stem exterior longitudinal reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.longExtStemIndex)+" (armature long. extérieure voile):}\\\\\n")
    CSectionStemLongExt.writeResultTraction(outputFile,0.0)

    # Stem interior longitudinal reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.longIntStemIndex)+" (armature long. intérieure voile):}\\\\\n")
    CSectionStemLongInt.writeResultTraction(outputFile,0.0)

    # Stem top skin reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.topSkinIndex)+" (armature long. couronnement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #self.reinforcement[self.topSkinIndex].writeRebars(outputFile,self.concrete,1e-5)
    outputFile.write("\\hline\n")
    outputFile.write("\\end{supertabular}\n")
    outputFile.write("\\end{center}\n")
    outputFile.close()
