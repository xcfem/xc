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
from rough_calculations import ng_rc_section
import xc

class BasementWall(rw.RetainingWall):
  '''Basement retaining wall (propped cantilever).'''
  def __init__(self,name= 'prb',concreteCover=40e-3,stemBottomWidth=0.25,stemTopWidth=0.25,footingThickness= 0.25, concrete= None, steel= None):
    '''Constructor '''
    super(BasementWall,self).__init__(name,concreteCover, stemBottomWidth,stemTopWidth,footingThickness, concrete, steel)

  def getSectionIntStem(self):
    '''Returns RC section for reinforcement at stem interior.'''
    return ng_rc_section.RCSection(self.stemReinforcement[self.stemReinforcement.intStemIndex],self.concrete,self.b,self.stemBottomWidth)

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
    outputFile.write("\\bottomcaption{"+ self.name +" wall reinforcement} \\label{tb_"+self.name+"}\n")
    outputFile.write("\\tablefirsthead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{Reinforcements mur "+self.name+"}}\\\\\\hline\n}\n")
    outputFile.write("\\tablehead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{"+self.name+" (suite)}}\\\\\\hline\n}\n")
    outputFile.write("\\tabletail{\\hline \\multicolumn{1}{|r|}{../..}\\\\\\hline}\n")
    outputFile.write("\\tablelasttail{\\hline}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{supertabular}[H]{|l|}\n")
    outputFile.write("\\hline\n")

    #Coupe ExtStemBottomIndex. Reinforced concrete. Encastrement.
    CExtStemBottom= self.stemReinforcement.getSectionExtStemBottom()
    VdMaxEncastrement= self.internalForcesULS.VdMaxEncastrement(self.stemBottomWidth)
    MdMaxEncastrement= self.internalForcesULS.MdMaxEncastrement(self.footingThickness)
    outputFile.write("\\textbf{Reinforcement " + str(self.stemReinforcement.extStemBottomIndex) + " (outside reinforcement dowels) :} \\\\\n")
    NdEncastrement= 0.0 #we neglect axial force
    CExtStemBottom.writeResultFlexion(outputFile,NdEncastrement, MdMaxEncastrement,VdMaxEncastrement)
    CExtStemBottom.writeResultStress(outputFile,MdMaxEncastrement)

    # Exterior rebars in stem.
    yCoupeExtStem= self.internalForcesULS.getYStem(self.stemReinforcement.getBasicAnchorageLength(self.stemReinforcement.extStemBottomIndex, self.concrete))
    CExtStem= self.stemReinforcement.getSectionExtStem(yCoupeExtStem)

    # Rebars on footing top
    CTopFooting= self.footingReinforcement.getSectionTopFooting()
    NdTopFooting= 0.0 #we neglect axial force
    VdTopFooting= max(abs(self.internalForcesULS.VdFooting.positive[0]),abs(self.internalForcesULS.VdFooting.negative[0]))
    MdTopFooting= max(abs(self.internalForcesULS.MdFooting.positive[0]),abs(self.internalForcesULS.MdFooting.negative[0]))
    outputFile.write("\\textbf{Reinforcement "+str(self.footingReinforcement.topFootingIndex)+" (footing top reinforcement):}\\\\\n")
    CTopFooting.writeResultFlexion(outputFile,NdTopFooting,MdTopFooting,VdTopFooting)
    CTopFooting.writeResultStress(outputFile,MdTopFooting)

    CIntStemBottom= self.stemReinforcement.getSectionIntStemBottom()
    
    CSectionStemTop= self.stemReinforcement.getSectionStemTop()
    CSectionFootingBottom= self.footingReinforcement.getSectionFootingBottom()
    CSectionFootingBottomLongitudinal= self.footingReinforcement.getSectionFootingBottomLongitudinal()
    CSectionFootingTopLongitudinal= CSectionFootingBottomLongitudinal
    CSectionStemLongExt= self.stemReinforcement.getSectionStemLongExt()
    CSectionStemLongInt= CSectionStemLongExt

    # Interior rebars at stem bottom 
    outputFile.write("\\textbf{Reinforcement "+str(self.stemReinforcement.intStemBottomIndex)+" (inside reinforcement dowels):}\\\\\n")
    CIntStemBottom.writeResultCompression(outputFile,0.0,CSectionStemLongInt.tensionRebars.getAs())

    # Interior rebars at stem.
    outputFile.write("\\textbf{Reinforcement "+str(self.stemReinforcement.intStemIndex)+" (inside stem reinforcement):}\\\\\n")
    CIntStem= self.getSectionIntStem()
    VdMinMidStem= self.internalForcesULS.VdMinMidStem(self.stemBottomWidth)
    MdMinMidStem= self.internalForcesULS.MdMinMidStem(self.footingThickness)
    NdMidStem= 0.0 #we neglect axial force
    CIntStem.writeResultFlexion(outputFile,NdMidStem, MdMinMidStem,VdMinMidStem)
    CIntStem.writeResultStress(outputFile,MdMinMidStem)

    # Reinforcement at stem top.
    outputFile.write("\\textbf{Reinforcement "+str(self.stemReinforcement.topStemIndex)+" (stem top transverse reinforcement):}\\\\\n")
    CSectionStemTop.writeResultFlexion(outputFile,0.0,0.0,0.0)

    # Reinforcement at footing bottom.
    outputFile.write("\\textbf{Reinforcement "+str(self.footingReinforcement.bottomFootingIndex)+" (footing bottom transverse reinforcement):}\\\\\n")
    CSectionFootingBottom.writeResultCompression(outputFile,0.0,CSectionFootingBottomLongitudinal.tensionRebars.getAs())

    # Longitudinal reinforcement at footing bottom.
    outputFile.write("\\textbf{Reinforcement "+str(self.footingReinforcement.longBottomFootingIndex)+" (footing bottom longitudinal reinforcement):}\\\\\n")
    CSectionFootingBottomLongitudinal.writeResultTraction(outputFile,0.0)

    # Longitudinal reinforcement at footing top.
    outputFile.write("\\textbf{Reinforcement "+str(self.footingReinforcement.longTopFootingIndex)+" (footing top longitudinal reinforcement):}\\\\\n")
    CSectionFootingTopLongitudinal.writeResultTraction(outputFile,0.0)

    # Footing skin reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.footingReinforcement.skinFootingIndex)+" (footing skin reinforcement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #self.reinforcement[self.skinFootingIndex].writeRebars(outputFile,self.concrete,1e-5)

    # Stem exterior longitudinal reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.stemReinforcement.longExtStemIndex)+" (stem outside longitudinal reinforcement):}\\\\\n")
    CSectionStemLongExt.writeResultTraction(outputFile,0.0)

    # Stem interior longitudinal reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.stemReinforcement.longIntStemIndex)+" (stem inside longitudinal reinforcement):}\\\\\n")
    CSectionStemLongInt.writeResultTraction(outputFile,0.0)

    # Stem top skin reinforcement.
    outputFile.write("\\textbf{Reinforcement "+str(self.stemReinforcement.topSkinIndex)+" (stem top skin reinforcement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #self.reinforcement[self.topSkinIndex].writeRebars(outputFile,self.concrete,1e-5)
    outputFile.write("\\hline\n")
    outputFile.write("\\end{supertabular}\n")
    outputFile.write("\\end{center}\n")
    outputFile.close()
