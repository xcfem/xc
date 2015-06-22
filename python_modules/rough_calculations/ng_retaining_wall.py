# -*- coding: utf-8 -*-
from __future__ import division
import sys
from postprocess.reports import common_formats as fmt
from postprocess.reports import draw_schema_armature_mur as draw_schema
import math
import scipy.interpolate
import matplotlib
matplotlib.use('PS')
import matplotlib.pyplot as plt
from materials.sia262 import concreteSIA262
from materials.sia262 import steelSIA262
from rough_calculations import ng_rebar_def
from rough_calculations import ng_rc_section
import os

class EffortsMur(object):
  y= [0,1]
  mdMax= [0,1]
  vdMax= [0,1]
  mdMaxVoile= scipy.interpolate.interp1d(y,mdMax)
  vdMaxVoile= scipy.interpolate.interp1d(y,vdMax)
  hauteurVoile= y[-1]
  MdSemelle= 0.0
  VdSemelle= 0.0
  def __init__(self,y,mdMax,vdMax,MdSemelle,VdSemelle):
    self.y= y
    self.mdMax= mdMax
    self.vdMax= vdMax
    self.mdMaxVoile= scipy.interpolate.interp1d(self.y,self.mdMax)
    self.vdMaxVoile= scipy.interpolate.interp1d(self.y,self.vdMax)
    self.hauteurVoile= self.y[-1]
    self.MdSemelle= MdSemelle
    self.VdSemelle= VdSemelle
  def MdEncastrement(self):
    return self.mdMax[-1]
  def VdEncastrement(self,epaisseurEncastrement):
    yV= self.hauteurVoile-epaisseurEncastrement
    return abs(self.vdMaxVoile(yV))
  def Vd(self, yCoupe):
    return abs(self.vdMaxVoile(yCoupe))
  def Md(self, yCoupe):
    return abs(self.mdMaxVoile(yCoupe))
  def getYVoile(self,hCoupe):
    return self.hauteurVoile-hCoupe
  def writeGraphic(self,fileName):
    z= []
    for yi in self.y:
      z.append(self.hauteurVoile-yi)
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


class RetainingWall(object):
  name= "prb"
  #Matériels
  beton= concreteSIA262.c25_30
  acier= steelSIA262.B500B
  exigeanceFisuration= "B"

  #Géométrie
  enrobage= 40e-3
  b= 1.0
  hEncastrement= 0.25
  hCouronnement= 0.25
  hSemelle= 0.25
  armatures= {}

  def __init__(self,name,enrobage,hEncastrement,hCouronnement,hSemelle):
    self.name= name
    self.enrobage= enrobage
    self.hEncastrement= hEncastrement
    self.hCouronnement= hCouronnement
    self.hSemelle= hSemelle
    #Default reinforcement
    AdefA= ng_rebar_def.RebarFamily(self.acier,8e-3,0.15,enrobage)
    #AdefB= RebarFamily(self.acier,10e-3,0.30,enrobage)
    Adef= AdefA # DoubleRebarFamily(AdefA,AdefB)
    for i in range(1,15):
      self.armatures[i]= Adef
    # #Armature de peau semelle
    # R= self.hSemelle-2*self.enrobage-8e-3
    # n= math.ceil(R/0.15)+1
    # ecart= R/(n-1)
    # self.armatures[10]= FamNBars(self.acier,n,8e-3,ecart,enrobage)
    # #Armature couronnement.
    # R= self.hCouronnement-2*self.enrobage-8e-3
    # n= math.ceil(R/0.15)+1
    # ecart= R/(n-1)
    # self.armatures[13]= FamNBars(self.acier,n,8e-3,ecart,enrobage)
    
  def setArmature(self,index,armature):
    self.armatures[index]= armature
  def getBasicAnchorageLength(self,index):
    return self.armatures[index].getBasicAnchorageLength(self.beton)

  def getDepth(self,hauteurVoile,y):
    return (self.hEncastrement-self.hCouronnement)/hauteurVoile*y+self.hCouronnement
  def getSection1(self):
    return ng_rc_section.RCSection(self.armatures[1],self.beton,self.exigeanceFisuration,self.b,self.hEncastrement)
  def getSection2(self,hauteurVoile,y):
    c= self.getDepth(hauteurVoile,y)
    return ng_rc_section.RCSection(self.armatures[2],self.beton,self.exigeanceFisuration,self.b,c)
  def getSection3(self):
    return ng_rc_section.RCSection(self.armatures[3],self.beton,self.exigeanceFisuration,self.b,self.hSemelle)
  def getSection4(self):
    return ng_rc_section.RCSection(self.armatures[4],self.beton,self.exigeanceFisuration,self.b,self.hEncastrement)
  def getSection6(self):
    return ng_rc_section.RCSection(self.armatures[6],self.beton,self.exigeanceFisuration,self.b,self.hCouronnement)
  def getSection7(self):
    return ng_rc_section.RCSection(self.armatures[7],self.beton,self.exigeanceFisuration,self.b,self.hSemelle)
  def getSection8(self):
    return ng_rc_section.RCSection(self.armatures[8],self.beton,self.exigeanceFisuration,self.b,self.hSemelle)
  def getSection11(self):
    return ng_rc_section.RCSection(self.armatures[11],self.beton,self.exigeanceFisuration,self.b,self.hSemelle)

  def writeDef(self,pth,outputFile,effortsMur):
    pathFiguraEPS= pth+self.name+".eps"
    effortsMur.writeGraphic(pathFiguraEPS)
    pathFiguraPDF= pth+self.name+".pdf"
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
    outputFile.write("\\begin{tabular}{l}\n")
    outputFile.write("\\textsc{Géométrie mur}\\\\\n")
    outputFile.write("Épaisseur couronnement: \\\\\n")
    outputFile.write("$b_{couronn}= "+fmt.Longs.format(self.hCouronnement)+"\\ m$\\\\\n")
    outputFile.write("Hauteur voile: \\\\\n")
    outputFile.write("$h_{voile}= "+fmt.Longs.format(effortsMur.hauteurVoile)+"\\ m$\\\\\n")
    outputFile.write("Épaisseur encastrement: \\\\\n")
    outputFile.write("$b_{encast.}= "+fmt.Longs.format(self.hEncastrement)+"\\ m$\\\\\n")
    outputFile.write("Épaisseur semelle: \\\\\n")
    outputFile.write("$b_{semelle.}= "+fmt.Longs.format(self.hSemelle)+"\\ m$\\\\\n")
    outputFile.write("\\end{tabular} \\\\\n")
    outputFile.write("\\end{tabular} \\\\\n")

    outputFile.write("\\hline\n")
    outputFile.write("\\begin{tabular}{llll}\n")
    outputFile.write("\\multicolumn{4}{c}{\\textsc{Matériels}}\\\\\n")
    outputFile.write("  Béton: " + self.beton.name +" & ")
    outputFile.write("  Acier: " + self.acier.name +" & ")
    outputFile.write("  Exigeance fisuration: " + self.exigeanceFisuration +" & ")
    outputFile.write("  Enrobage: "+ fmt.Diam.format(self.enrobage*1e3)+ " mm\\\\\n")
    outputFile.write("\\end{tabular} \\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\end{tabular}\n")
    outputFile.write("\\caption{Matériels et dimensions mur "+ self.name +"} \\label{tb_def_"+self.name+"}\n")
    outputFile.write("\\end{center}\n")
    outputFile.write("\\end{table}\n")


  def writeResult(self,pth,effortsMur):
    outputFile= open(pth+self.name+".tex","w")
    self.writeDef(pth,outputFile,effortsMur)
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
    VdEncastrement= effortsMur.VdEncastrement(self.hEncastrement)
    MdEncastrement= effortsMur.MdEncastrement()
    outputFile.write("\\textbf{Armature 1 (armature extérieure en attente) :} \\\\\n")
    NdEncastrement= 0.0 #we neglect axial force
    C1.writeResultFlexion(outputFile,NdEncastrement, MdEncastrement,VdEncastrement)

    #Coupe 2. Béton armé. Voile
    yCoupe2= effortsMur.getYVoile(self.getBasicAnchorageLength(1))
    C2= self.getSection2(effortsMur.hauteurVoile,yCoupe2)
    Nd2= 0.0 #we neglect axial force
    Vd2= effortsMur.Vd(yCoupe2)
    Md2= effortsMur.Md(yCoupe2)
    outputFile.write("\\textbf{Armature 2 (armature extériéure voile):}\\\\\n")
    C2.writeResultFlexion(outputFile,Nd2,Md2,Vd2)

    #Coupe 3. Béton armé. Semelle
    C3= self.getSection3()
    Nd3= 0.0 #we neglect axial force
    Vd3= effortsMur.VdSemelle
    Md3= effortsMur.MdSemelle
    outputFile.write("\\textbf{Armature 3 (armature supérieure semelle):}\\\\\n")
    C3.writeResultFlexion(outputFile,Nd3,Md3,Vd3)

    #Coupe 4. armature intérieure en attente. Encastrement voile 
    C4= self.getSection4()
    outputFile.write("\\textbf{Armature 4 (armature intérieure en attente):}\\\\\n")
    C4.writeResultFlexion(outputFile,0.0,0.0,0.0)

    #Coupe 5. armature intérieure en voile.
    C5= C4
    outputFile.write("\\textbf{Armature 5 (armature intérieure en voile):}\\\\\n")
    C5.writeResultFlexion(outputFile,0.0,0.0,0.0)

    #Coupe 6. armature couronnement.
    C6= self.getSection6()
    outputFile.write("\\textbf{Armature 6 (armature couronnement):}\\\\\n")
    C6.writeResultFlexion(outputFile,0.0,0.0,0.0)

    #Coupe 7. armature inférieure semelle.
    C7= self.getSection7()
    outputFile.write("\\textbf{Armature 7 (armature trsv. inférieure semelle):}\\\\\n")
    C7.writeResultFlexion(outputFile,0.0,0.0,0.0)

    #Coupe 8. armature long. inférieure semelle.
    C8= self.getSection8()
    outputFile.write("\\textbf{Armature 8 (armature long. inférieure semelle):}\\\\\n")
    C8.writeResultTraction(outputFile,0.0)

    #Coupe 9. armature long. supérieure semelle.
    C9= C8
    outputFile.write("\\textbf{Armature 9 (armature long. supérieure semelle):}\\\\\n")
    C9.writeResultTraction(outputFile,0.0)

    #Armature 10. armature de peau semelle.
    outputFile.write("\\textbf{Armature 10 (armature de peau semelle):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #writeRebars(outputFile,self.beton,self.armatures[10],1e-5)

    #Coupe 11. armature long. extérieure voile.
    C11= self.getSection11()
    outputFile.write("\\textbf{Armature 11 (armature long. extérieure voile):}\\\\\n")
    C11.writeResultTraction(outputFile,0.0)

    #Coupe 12. armature long. intérieure voile.
    C12= C11
    outputFile.write("\\textbf{Armature 12 (armature long. intérieure voile):}\\\\\n")
    C12.writeResultTraction(outputFile,0.0)

    #Armature 13. armature long. couronnement.
    outputFile.write("\\textbf{Armature 13 (armature long. couronnement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #writeRebars(outputFile,self.beton,self.armatures[13],1e-5)
    outputFile.write("\\hline\n")
    outputFile.write("\\end{supertabular}\n")
    outputFile.write("\\end{center}\n")
    outputFile.close()

  def drawSchema(self,pth,effortsMur):
    outputFile= open(pth+'schema_'+self.name+".tex","w")
    outputFile.write("\\begin{figure}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write(draw_schema.hdr)
    for l in draw_schema.lines:
      outputFile.write(l)

    defStrings= {}
    defStrings[1]= self.getSection1().tractionRebars.getDefStrings()
    yCoupe2= effortsMur.getYVoile(self.getBasicAnchorageLength(1))
    defStrings[2]= self.getSection2(effortsMur.hauteurVoile,yCoupe2).tractionRebars.getDefStrings()
    defStrings[3]= self.getSection3().tractionRebars.getDefStrings()
    defStrings[4]= self.getSection4().tractionRebars.getDefStrings()
    defStrings[5]= self.getSection4().tractionRebars.getDefStrings() #C5==C4
    defStrings[6]= self.getSection6().tractionRebars.getDefStrings()
    defStrings[7]= self.getSection7().tractionRebars.getDefStrings()
    defStrings[8]= self.getSection8().tractionRebars.getDefStrings()
    defStrings[9]= self.getSection8().tractionRebars.getDefStrings() #C9==C8
    #defStrings[10]= self.getSection10().tractionRebars.getDefStrings()
    defStrings[11]= self.getSection11().tractionRebars.getDefStrings()
    defStrings[12]= self.getSection11().tractionRebars.getDefStrings() #C12==C11

    rebarAnno= draw_schema.getRebarAnnotationLines(defStrings)
    for l in rebarAnno:
      outputFile.write(l)
    outputFile.write(draw_schema.tail)
    outputFile.write("\\end{center}\n")
    outputFile.write("\\caption{Schéma armatures mur "+ self.name +"} \\label{fg_"+self.name+"}\n")
    outputFile.write("\\end{figure}\n")

