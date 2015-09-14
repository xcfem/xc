# -*- coding: utf-8 -*-

import plotGeomSeccion as pg
import sys

class RecordFamArmaduraPrincipal(object):
  # Parámetros que se obtienen de cada familia de armaduras
  nmb=  None # Código que identifica a la familia de armaduras.
  nBarras=  0 # Número de las barras.
  diamBarras=  0.0 # Diámetro de las barras.
  areaBarras=  0.0 # Area total de la familia de armaduras
  recubMec=  0.0 # Valor mínimo del recubrimiento mecánico.
  cdgBarras=  None # Posición del centro de gravedad.

  def __init__(self,reinfLayer):
    self.nmb= reinfLayer.nombre
    self.nBarras= reinfLayer.nReinfBars # Número de las barras.
    self.diamBarras= reinfLayer.getBarDiam # Diámetro de las barras.
    self.areaBarras= reinfLayer.getArea # Area total de la familia de armaduras
    self.recubMec= reinfLayer.getRecubrimiento # Valor mínimo del recubrimiento mecánico.
    self.cdgBarras= reinfLayer.getCdg # Posición del centro de gravedad.

  def texWrite(self,archTex):
    archTex.write(self.nmb," & ",self.nBarras)
    archTex.write(" & ",format("%u",round(self.diamBarras*1e3)))
    archTex.write(" & ",format("%5.2f",self.areaBarras*1e4))
    archTex.write(" & ",format("%4.2f",self.areaBarras/areaHorm*1e3))
    archTex.write(" & ",format("%4.1f",self.recubMec*1e2))
    archTex.write(" & ",format("%5.3f",self.cdgBarras.x) ," & ",format("%5.3f",self.cdgBarras.y) ,"\\\\\n")

def writeMainReinforcement(listaFamArmaduraPrincipal, areaHorm, archTex):
  archTex.write("\\begin{tabular}{ll}\n")
  archTex.write("Área total $A_s=",format("%5.2f",areaArmaduraPrincipal*1e4) ,"\\ cm^2$ & Cuantía geométrica $\\rho= ",format("%4.2f",areaArmaduraPrincipal/AB*1e3) ,"\\permil$\\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("Familias de armadura principal:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{cccccccc}\n")
  archTex.write("Id & n. barras & $\\phi$ & área & c. geom. & recub. mec. & $y_{cdg}$ & $z_{cdg}$\\\\\n")
  archTex.write(" &  & $(mm)$ & $(cm^2)$ & $(\\permil)$ & $(cm)$ & $(m)$ & $(m)$\\\\\n")
  for f in listaFamArmaduraPrincipal:
    archTex.write("\hline\n")
    f.write(archTex)
  archTex.write("\\end{tabular} \\\\\n")

def writeShearReinforcement(recordArmaduraCortante, archTex):
  archTex.write("\\hline\n")
  archTex.write(nmbFamilia," & ",recordArmaduraCortante.nRamas)
  diamRamas= sqrt(4*areaRamas/PI)
  archTex.write(" & ",format("%u",round(recordArmaduraCortante.diamRamas*1e3)))
  archTex.write(" & ",format("%5.2f",recordArmaduraCortante.areaRamas*recordArmaduraCortante.nRamas*1e4))
  archTex.write(" & ",format("%4.1f",recordArmaduraCortante.espaciamientoRamas*1e2))
  archTex.write(" & ",format("%5.2f",recordArmaduraCortante.areaRamas*nRamas/ancho/recordArmaduraCortante.espaciamientoRamas*1e4))
  archTex.write(" & ",format("%3.1f",rad2deg(recordArmaduraCortante.angAlphaRamas)))
  archTex.write(" & ",format("%3.1f",rad2deg(recordArmaduraCortante.angThetaBielas)),"\\\\\n")

def getSectionInfo(mdlr,seccion):
# Elabora un informe con los datos de la sección.
  EHorm= seccion.tipoHormigon.Ecm
  EAcero= seccion.tipoArmadura.Es
  tangHorm= seccion.getConcreteDiagram(mdlr).getTangent
  geomSection= mdlr.getMaterialLoader.getSectionGeometry(seccion.nmbGeomSeccion())
  regions= geomSection.getRegions
  GB= regions.getCdgSeccBruta # Centro de gravedad de la sección bruta.
  AB= regions.getAreaSeccBruta # Área de la sección bruta.
  IyB= regions.getIySeccBruta # Tensor de inercia de la sección bruta.
  IzB= regions.getIzSeccBruta
  PyzB= regions.getPyzSeccBruta

  armaduras= geomSection.getReinfLayers
  areaArmaduraPrincipal= armaduras.getAreaSeccBruta
  recubrimiento= armaduras.getRecubrimiento
  lista_fams_armadura= []
  datosFam = RecordFamArmaduraPrincipal()
  for f in armaduras:
    populateRecordFamArmaduraPrincipal(datosFam,f)
    lista_fams_armadura.append(datosFam)
 
  GH= regions.getCdgSeccHomogeneizada(tangHorm) # Centro de gravedad de la sección homogeneizada.
  AH= regions.getAreaSeccHomogeneizada(tangHorm) # Área de la sección homogeneizada.
  IyH= regions.getIySeccHomogeneizada(tangHorm) # Tensor de inercia de la sección homogeneizada.
  IzH=  regions.getIzSeccHomogeneizada(tangHorm)
  PyzH= regions.getPyzSeccHomogeneizada(tangHorm)


# Elabora un informe con los datos de la sección.
def writeSectionReport(mdlr,scc, archTex, pathFigura):
  sectionGeometryName= scc.nmbGeomSeccion()
  geomSection= mdlr.getMaterialLoader.getSectionGeometry(sectionGeometryName)
  if(geomSection):
    pg.plotGeomSeccion(geomSection,pathFigura)
  else:
    sys.stderr.write('error: geometry of section: '+scc.nmbGeomSeccion()+' not defined\n')
  getSectionInfo(mdlr,scc)
  archTex.write("\\begin{table}\n")
  archTex.write("\\begin{center}\n")
  archTex.write("\\begin{tabular}{|c|}\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{large} ",toLaTex(nmbSeccion)," \end{large}\\\\\n")
  archTex.write("\\hline\n")
  archTex.write(descSeccion,"\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{c|l}\n")
  archTex.write("\\begin{minipage}{85mm}\n")
  archTex.write("\\vspace{2mm}\n")
  archTex.write("\\begin{center}\n")
  archTex.write("\\includegraphics[width=80mm]{",pathFigura,"}\n")
  archTex.write("\\end{center}\n")
  archTex.write("\\vspace{1pt}\n")
  archTex.write("\\end{minipage} & \n")
  archTex.write("\\begin{tabular}{l}\n")
  archTex.write("ancho: \\\\\n")
  archTex.write("$b= ",format("%4.2f",ancho),"\\ m$\\\\\n")
  archTex.write("canto: \\\\\n")
  archTex.write("$h= ",format("%4.2f",canto),"\\ m$\\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\textbf{Materiales}:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{ll}\n")
  archTex.write("Hormigón: ",tipoHormigon," & Módulo de deformación longitudinal: $E_c= ",format("%4.2f",EHorm/1e9),"\\ GPa$\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("Acero: ",tipoArmadura," & Módulo elástico: $E_s= ",format("%4.2f",EAcero/1e9),"\\ GPa$\\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\textbf{Valores estáticos}:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("Sección bruta:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{ll}\n")
  archTex.write("$A_{bruta}=",format("%6.3f",AB) ,"\\ m^2$ & \\multirow{3}{*}{Tensor de inercia ($cm^4$): $ \\left( \\begin{array}{ccc}", format("%5.2f",JTorsion*1e4) ," & 0.00 & 0.00 \\\\ 0.00 & ", format("%5.2f",IyB*1e4) ," & ",format("%5.2f",PyzB) ," \\\\ 0.00 & ",format("%5.2f",PyzB) ," & ",format("%5.2f",IzB*1e4) ," \\end{array} \\right)$} \\\\\n")
  archTex.write("& \\\\\n")
  archTex.write("C.D.G.: $(",format("%5.2f",GB[0]) ,",",format("%5.2f",GB[1]) ,")\\ m$  & \\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("Sección homogeneizada:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{ll}\n")
  archTex.write("$A_{homog.}=",format("%6.3f",AH) ,"\\ m^2$ & \\multirow{3}{*}{Tensor de inercia ($cm^4$): $ \\left( \\begin{array}{ccc}", format("%5.2f",JTorsion*1e4) ," & 0.00 & 0.00 \\\\ 0.00 & ", format("%5.2f",IyH*1e4) ," & ",format("%5.2f",PyzH) ," \\\\ 0.00 & ",format("%5.2f",PyzH) ," & ",format("%5.2f",IzH*1e4)," \\end{array} \\right)$} \\\\\n")
  archTex.write("& \\\\\n")
  archTex.write("C.D.G.: $(",format("%5.2f",GH[0]),",",format("%5.2f",GH[1]),")\\ m$  & \\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\textbf{Armadura pasiva}:\\\\\n")
  archTex.write("\\hline\n")
  imprimeArmaduraPrincipal("lista_fams_armadura",AB,archTex)
  archTex.write("\\hline\n")
  archTex.write("Familias de armadura de cortante:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{cccccccc}\n")
  archTex.write("Id & n. ramas & $\\phi$ & área & sep. & area/m & $\\alpha$ & $\\beta$\\\\\n")
  archTex.write(" &  & $(mm)$ & $(cm^2)$ & $(cm)$ & $(cm^2/m)$ & $( \\degree)$ & $( \\degree)$\\\\\n")
  imprimeArmaduraCortante("armCortanteZ",archTex)
  imprimeArmaduraCortante("armCortanteY",archTex)
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\end{tabular}\n")
  archTex.write("\\end{center}\n")
  archTex.write("\\caption{",descSeccion," ("+ toLaTex(nmbSeccion) +").","} \\label{tb_",nmbSeccion,"}\n")
  archTex.write("\\end{table}\n")

