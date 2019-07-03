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

  def getSection5(self):
    '''Returns RC section for armature in position 5.'''
    return ng_rc_section.RCSection(self.reinforcement[5],self.concrete,self.b,self.stemBottomWidth)

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
    outputFile.write("\\tablefirsthead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{Armatures mur "+self.name+"}}\\\\\\hline\n}\n")
    outputFile.write("\\tablehead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{"+self.name+" (suite)}}\\\\\\hline\n}\n")
    outputFile.write("\\tabletail{\\hline \\multicolumn{1}{|r|}{../..}\\\\\\hline}\n")
    outputFile.write("\\tablelasttail{\\hline}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{supertabular}[H]{|l|}\n")
    outputFile.write("\\hline\n")

    #Coupe 1. Béton armé. Encastrement.
    C1= self.getSection1()
    VdMaxEncastrement= self.internalForcesULS.VdMaxEncastrement(self.stemBottomWidth)
    MdMaxEncastrement= self.internalForcesULS.MdMaxEncastrement(self.footingThickness)
    outputFile.write("\\textbf{Armature 1 (armature extérieure en attente) :} \\\\\n")
    NdEncastrement= 0.0 #we neglect axial force
    C1.writeResultFlexion(outputFile,NdEncastrement, MdMaxEncastrement,VdMaxEncastrement)
    C1.writeResultStress(outputFile,MdMaxEncastrement)

    #Coupe 2. Béton armé. Stem
    yCoupe2= self.internalForcesULS.getYStem(self.getBasicAnchorageLength(1))
    C2= self.getSection2(yCoupe2)

    #Coupe 3. Béton armé. Semelle
    C3= self.getSection3()
    Nd3= 0.0 #we neglect axial force
    Vd3= max(abs(self.internalForcesULS.VdSemelle.positive[0]),abs(self.internalForcesULS.VdSemelle.negative[0]))
    Md3= max(abs(self.internalForcesULS.MdSemelle.positive[0]),abs(self.internalForcesULS.MdSemelle.negative[0]))
    outputFile.write("\\textbf{Armature 3 (armature supérieure semelle):}\\\\\n")
    C3.writeResultFlexion(outputFile,Nd3,Md3,Vd3)
    C3.writeResultStress(outputFile,Md3)

    C4= self.getSection4()
    
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
    C5= self.getSection5()
    VdMinMidStem= self.internalForcesULS.VdMinMidStem(self.stemBottomWidth)
    MdMinMidStem= self.internalForcesULS.MdMinMidStem(self.footingThickness)
    NdMidStem= 0.0 #we neglect axial force
    C5.writeResultFlexion(outputFile,NdMidStem, MdMinMidStem,VdMinMidStem)
    C5.writeResultStress(outputFile,MdMinMidStem)

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
    #self.reinforcement[10].writeRebars(outputFile,self.concrete,1e-5)

    #Coupe 11. armature long. extérieure voile.
    outputFile.write("\\textbf{Armature 11 (armature long. extérieure voile):}\\\\\n")
    C11.writeResultTraction(outputFile,0.0)

    #Coupe 12. armature long. intérieure voile.
    outputFile.write("\\textbf{Armature 12 (armature long. intérieure voile):}\\\\\n")
    C12.writeResultTraction(outputFile,0.0)

    #Armature 13. armature long. couronnement.
    outputFile.write("\\textbf{Armature 13 (armature long. couronnement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #self.reinforcement[13].writeRebars(outputFile,self.concrete,1e-5)
    outputFile.write("\\hline\n")
    outputFile.write("\\end{supertabular}\n")
    outputFile.write("\\end{center}\n")
    outputFile.close()
