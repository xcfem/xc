# -*- coding: utf-8 -*-

'''sectionReport.py: report describing RC cross sections mechanical properties.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"


import sys
import math
import os
from latex import latexUtils
from postprocess.reports import graph_material as mg
import plot_fiber_section as pfs
from materials.sections import section_properties as prmRect
from materials.sections.fiber_section import  defSimpleRCSection as sHAs

fmt3_1f= '{:3.1f}'
fmt4_1f= '{:4.1f}'
fmt4_2f= '{:4.2f}'
fmt5_2f= '{:5.2f}'
fmt5_3f= '{:5.3f}'
fmt6_3f= '{:6.3f}'

class RecordFamMainReinforcement(object):
  '''Parameters for each layer of main reinforcement
  '''
  def __init__(self,reinfLayer):
    self.name= reinfLayer.nombre
    self.nRebars= reinfLayer.numReinfBars # number of rebars
    self.rebarsDiam= reinfLayer.barDiameter # Rebars diameter
    self.areaRebar= reinfLayer.barArea # total area of reinforcement in the layer
    self.minEffCover= reinfLayer.getCover() # Minimum value of effective cover
    self.barsCOG= reinfLayer.getCenterOfMass() # center of gravity of the bars

  def texWrite(self,archTex,areaHorm):
    archTex.write(self.name+' & '+str(self.nRebars))
    archTex.write(' & '+str(round(self.rebarsDiam*1e3)))
    archTex.write(' & '+fmt5_2f.format(self.areaRebar*1e4))
    archTex.write(' & '+fmt4_2f.format(self.areaRebar/areaHorm*1e3))
    archTex.write(' & '+fmt4_1f.format(self.minEffCover*1e2))
    archTex.write(' & '+fmt5_3f.format(self.barsCOG[0]) +' & '+fmt5_3f.format(self.barsCOG[1]) +"\\\\\n")

def writeMainReinforcement(listaFamMainReinforcement, areaHorm, archTex):
  archTex.write("\\begin{tabular}{ll}\n")
  areaMainReinforcement= listaFamMainReinforcement.getAreaGrossSection()
  archTex.write("Total area $A_s="+fmt5_2f.format(areaMainReinforcement*1e4) +"\\ cm^2$ & Geometric quantity $\\rho= "+fmt4_2f.format(areaMainReinforcement/areaHorm*1e3) +"\\permil$\\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("Layers of main reinforcement:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{cccccccc}\n")
  archTex.write("Id & N$^o$ bars & $\\phi$ & area & c. geom. & eff. cover & $y_{COG}$ & $z_{COG}$\\\\\n")
  archTex.write(" &  & $(mm)$ & $(cm^2)$ & $(\\permil)$ & $(cm)$ & $(m)$ & $(m)$\\\\\n")
  for f in listaFamMainReinforcement:
    archTex.write("\hline\n")
    RecordFamMainReinforcement(f).texWrite(archTex,areaHorm)
  archTex.write("\\end{tabular} \\\\\n")

def writeShearReinforcement(recordShearReinf, archTex, width):
  archTex.write("\\hline\n")
  archTex.write(recordShearReinf.familyName+' & '+str(recordShearReinf.nShReinfBranches))
  areaShReinfBranchs= recordShearReinf.getAs()
  diamRamas= math.sqrt(4*areaShReinfBranchs/math.pi)
  archTex.write(' & '+str(round(diamRamas*1e3)))
  archTex.write(' & '+fmt5_2f.format(areaShReinfBranchs*recordShearReinf.nShReinfBranches*1e4))
  archTex.write(' & '+fmt4_1f.format(recordShearReinf.shReinfSpacing*1e2))
  archTex.write(' & '+fmt5_2f.format(areaShReinfBranchs*recordShearReinf.nShReinfBranches/width/recordShearReinf.shReinfSpacing*1e4))
  archTex.write(' & '+fmt3_1f.format(math.degrees(recordShearReinf.angAlphaShReinf)))
  archTex.write(' & '+fmt3_1f.format(math.degrees(recordShearReinf.angThetaConcrStruts))+"\\\\\n")

class SectionInfo(object):
  ''' Obtains the fiber section parameters for writing its report page
  :ivar preprocessor: preprocessor
  :ivar sectName:     section name (used as title for the table and as caption, also)
  :ivar sectDescr:    section description (used as subtitle for the table}
  :ivar concrete:     concrete (its name and elastic modulus are reported on the table)
  :ivar rfSteel:      reinforcing steeĺ (its name and elastic modulus are reported on the table)
  :ivar concrDiag:    concrete diagram 
  :ivar rfStDiag:     reinforcing steel diagram
  :ivar geomSection:  material fiber section
  :ivar width:        width of the rectangular section (reported on the table and used to calculate 
                      the torsional inertia, also)
  :ivar depth:        depth of the rectangular section (reported on the table and used to calculate
                      the torsional inertia, also)
  :ivar G,A,I: match center of gravity, area and inertia tensor, respectively
  :ivar B,H: match gross and homogenized sections, respectively
  :ivar cover: refers to effective cover
  '''
  def __init__(self,preprocessor,sectName,sectDescr,concrete,rfSteel,concrDiag,rfStDiag,geomSection,width,depth):
    self.concrete=concrete
    self.rfSteel=rfSteel
    self.geomSection= geomSection
    self.sectName=sectName
    self.sectDescr=sectDescr
    self.concrete=concrete
    self.rfSteel=rfSteel
    self.concrDiag=concrDiag
    self.rfStDiag=rfStDiag
    self.width=width
    self.depth=depth
    self.JTorsion=self.scc.getJTorsion()
    self.shReinfY=sHAs.RecordShearReinforcement()
    self.shReinfY.familyName= "Vy"
    self.shReinfZ= sHAs.RecordShearReinforcement()
    self.shReinfZ.familyName= "Vz"
    self.Ec= self.concrete.Ecm()
    self.Es= self.rfSteel.Es
    self.regions= self.geomSection.getRegions
    self.tangConcr= self.concrDiag.getTangent()
    self.GB= self.geomSection.getCenterOfMassGrossSection() 
    self.AB= self.geomSection.getAreaGrossSection() 
    self.IyB= self.geomSection.getIyGrossSection() 
    self.IzB= self.geomSection.getIzGrossSection()
    self.PyzB= self.geomSection.getPyzGrossSection()
    self.reinforcement= self.geomSection.getReinfLayers  #ListReinfLayer
    self.tangSteel= self.rfStDiag.getTangent()
    self.areaMainReinforcement= self.reinforcement.getAreaGrossSection()
    self.cover= self.reinforcement.getCover
    self.lista_fams_reinforcement= []
    for f in self.reinforcement:
      datosFam= RecordFamMainReinforcement(f)
      self.lista_fams_reinforcement.append(datosFam)
    self.GH= self.geomSection.getCenterOfMassHomogenizedSection(self.tangConcr) # Center of gravity of the homogenized section
    self.AH= self.geomSection.getAreaHomogenizedSection(self.tangConcr) # Area of the homogenized section
    self.IyH= self.geomSection.getIyHomogenizedSection(self.tangConcr) # Inertia tensor of homogenized section.
    self.IzH=  self.geomSection.getIzHomogenizedSection(self.tangConcr)
    self.PyzH= self.geomSection.getPyzHomogenizedSection(self.tangConcr)

  def writeReport(self,archTex, pathFigura):
    if(self.geomSection):
      pfs.plotGeomSeccion(self.geomSection,pathFigura)
    else:
      sys.stderr.write('error: geometry of section: '+self.sectName+' not defined\n')
    fileHandler= open(archTex,'w')
    fileHandler.write('%% ****** Packages needed for LaTeX document: ****** \n')
    fileHandler.write('%%\usepackage{graphicx} %%\postscript includes\n')
    fileHandler.write('%%\usepackage{multirow} %%\multirow command\n')
    fileHandler.write('%%\usepackage{wasysym} %%\permil command\n')
    fileHandler.write('%%\usepackage{gensymb} %%\degree command\n')

    fileHandler.write('\\begin{table}\n')
    fileHandler.write('\\begin{center}\n')
    fileHandler.write('\\begin{tabular}{|c|}\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{large} '+latexUtils.toLaTex(self.sectName)+' \end{large}\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write(self.sectDescr+'\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{c|l}\n')
    fileHandler.write('\\begin{minipage}{85mm}\n')
    fileHandler.write('\\vspace{2mm}\n')
    fileHandler.write('\\begin{center}\n')
    #  name without extension to allow pdfLatex process the file
    nameWOExt= os.path.splitext(pathFigura)[0]
    fileHandler.write('\\includegraphics[width=80mm]{'+nameWOExt+'}\n')
    # name without extension and without path
    # nameWOExt= os.path.splitext(pathFigura)[0]
    # fileHandler.write('\\includegraphics[width=80mm]{'+self.sectName[4:]+'}\n')
    fileHandler.write('\\end{center}\n')
    fileHandler.write('\\vspace{1pt}\n')
    fileHandler.write('\\end{minipage} & \n')
    fileHandler.write('\\begin{tabular}{l}\n')
    fileHandler.write('width: \\\\\n')
    fileHandler.write('$b= '+'{0:.2f}'.format(self.width)+'\\ m$\\\\\n')
    fileHandler.write('depth: \\\\\n')
    fileHandler.write('$h= '+'{0:.2f}'.format(self.depth)+'\\ m$\\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Materials - mechanical properties}:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('Concrete: '+self.concrete.materialName+' & Modulus of elasticity: $E_c= '+'{0:.2f}'.format(self.Ec/1e9)+'\\ GPa$\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Steel: '+self.rfSteel.materialName+' & Modulus of elasticity: $E_s= '+'{0:.2f}'.format(self.Es/1e9)+'\\ GPa$\\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Sections - geometric and mechanical characteristics}:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Gross section:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('$A_{gross}='+fmt6_3f.format(self.AB) +'\\ m^2$ & \\multirow{3}{*}{Inertia tensor ($cm^4$): $ \\left( \\begin{array}{ccc}'+ fmt5_2f.format(self.JTorsion*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ fmt5_2f.format(self.IyB*1e4) +' & '+fmt5_2f.format(self.PyzB) +' \\\\ 0.00 & '+fmt5_2f.format(self.PyzB) +' & '+fmt5_2f.format(self.IzB*1e4) +' \\end{array} \\right)$} \\\\\n')
    fileHandler.write('& \\\\\n')
    fileHandler.write('C.O.G.: $('+fmt5_3f.format(self.GB[0])+','+fmt5_3f.format(self.GB[1])+')\\ m$  & \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Homogenized section:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('$A_{homog.}='+fmt6_3f.format(self.AH) +'\\ m^2$ & \\multirow{3}{*}{Inertia tensor ($cm^4$): $ \\left( \\begin{array}{ccc}'+ fmt5_2f.format(self.JTorsion*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ fmt5_2f.format(self.IyH*1e4) +' & '+fmt5_2f.format(self.PyzH) +' \\\\ 0.00 & '+fmt5_2f.format(self.PyzH) +' & '+fmt5_2f.format(self.IzH*1e4)+' \\end{array} \\right)$} \\\\\n')
    fileHandler.write('& \\\\\n')
    fileHandler.write('C.O.G.: $('+fmt5_3f.format(self.GH[0])+','+fmt5_3f.format(self.GH[1])+')\\ m$  & \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Passive reinforcement}:\\\\\n')
    fileHandler.write('\\hline\n')
    writeMainReinforcement(self.reinforcement,self.AB,fileHandler)
    fileHandler.write('\\hline\n')
    fileHandler.write('Layers of shear reinforcement:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{cccccccc}\n')
    fileHandler.write('Id & N$^o$ branch & $\\phi$ & area & spac. & area/m & $\\alpha$ & $\\beta$\\\\\n')
    fileHandler.write(' &  & $(mm)$ & $(cm^2)$ & $(cm)$ & $(cm^2/m)$ & $( \\degree)$ & $( \\degree)$\\\\\n')
    writeShearReinforcement(self.shReinfZ,fileHandler,self.width)
    writeShearReinforcement(self.shReinfY,fileHandler,self.depth)
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\end{tabular}\n')
    fileHandler.write('\\end{center}\n')
    fileHandler.write('\\caption{'+self.sectDescr+' ('+ latexUtils.toLaTex(self.sectName) +').'+'} \\label{tb_'+self.sectName+'}\n')
    fileHandler.write('\\end{table}\n')
    fileHandler.close()


class SectionInfoHASimple(SectionInfo):
  '''Obtains the parameters of a HASimple fiber section for writing its report page

  :ivar preprocessor: preprocessor
  :ivar sectHASimple: fiber section defined as a HASimple
  '''
  def __init__(self,preprocessor,sectHASimple):
    self.scc= sectHASimple
    sectName=sectHASimple.gmSectionName()
    sectDescr=sectHASimple.sectionDescr
    concrete=sectHASimple.concrType
    rfSteel=sectHASimple.reinfSteelType
    concrDiag=sectHASimple.getConcreteDiagram(preprocessor)
    rfStDiag=sectHASimple.getSteelDiagram(preprocessor)
    geomSection= preprocessor.getMaterialHandler.getSectionGeometry(sectName)
    width=sectHASimple.b
    depth=sectHASimple.h
    super(SectionInfoHASimple,self).__init__(preprocessor,sectName,sectDescr,concrete,rfSteel,concrDiag,rfStDiag,geomSection,width,depth)
    self.shReinfZ=sectHASimple.shReinfZ
    self.shReinfY=sectHASimple.shReinfY
