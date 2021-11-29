# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

from misc.latex import latex_utils

def printCabeceraListadoCapacityFactor(archivo, nmbSecc):
    ''' Print header for capacity factors listing.'''
    archivo.write("\\begin{center}\n")
    archivo.write("\\begin{footnotesize}\n")
    archivo.write("\\tablefirsthead{%\n")
    archivo.write("\\hline\n")
    # archivo.write("\\multicolumn{6}{|c|}{\\textbf{",nmbArch,"}}\\\\\n")
    archivo.write("\\multicolumn{6}{|c|}{Comprobación de la sección ", latex_utils.toLaTeX(nmbSecc),"}\\\\\n")
    archivo.write("\\hline\n")
    archivo.write("Elem. & Hip. & $N$  & $M_x$  & $M_y$  & FC \\\\\n")
    archivo.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
    archivo.write("\\hline}\n")
    archivo.write("\\tablehead{%\n")
    archivo.write("\\hline\n")
    archivo.write("\\multicolumn{6}{|l|}{\small\sl \ldots continúa}\\\\\n")
    archivo.write("\\hline\n")
    # archivo.write("\\multicolumn{6}{|c|}{\\textbf{",nmbArch,"}}\\\\\n")
    archivo.write("\\multicolumn{6}{|c|}{Comprobación de la sección ", latex_utils.toLaTeX(nmbSecc),"}\\\\\n")
    archivo.write("\\hline\n")
    archivo.write("Elem. & Hip. & $N$  & $M_x$  & $M_y$  & FC \\\\\n")
    archivo.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
    archivo.write("\\hline}\n")
    archivo.write("\\tabletail{%\n")
    archivo.write("\\hline\n")
    archivo.write("\\multicolumn{6}{|r|}{\small\sl continúa\ldots}\\\\\n")
    archivo.write("\\hline}\n")
    archivo.write("\\tablelasttail{\hline}\n")
    archivo.write("\\begin{supertabular}{|l|r|r|r|r|r|}\n")

# Imprime el cierre del listado de capacity factors.
def printCierreListadoCapacityFactor(archivo):
  archivo.write("\\end{supertabular}\n")
  archivo.write("\\end{footnotesize}\n")
  archivo.write("\\end{center}\n")


def listaCapacityFactor(preprocessor,setName, fName, titulo):
  '''
  Print results of normal stresses in elastic range checking for an
  elment set whose material is a steel shape.
  '''
  printCabeceraListadoCapacityFactor(fName,titulo)
  st= preprocessor.getSets.getSet(setName)
  elems= st.elements
  tagFCCPCP= 0
  FCCPCP= 0
  HIPCPCP= "" 
  for e in elems:
    fName.write(tag," & ",e.getProp("HIPCP")," & ",'{:5.2f}'.format(e.getProp("NCP")/1e3)," & ",'{:5.2f}'.format(e.getProp("MyCP")/1e3)," & ",'{:5.2f}'.format(e.getProp("MzCP")/1e3)," & ",'{:3.2f}'.format(e.getProp("FCCP")),"\\\\\n")
    if(e.getProp("FCCP")>FCCPCP):
      FCCPCP= e.getProp("FCCP")
      HIPCPCP= e.getProp("HIPCP")
      tagFCCPCP= e.tag
  fName.write("\\hline\n")
  fName.write(tagFCCPCP," & ",HIPCPCP," &  &  &  & ",'{:3.2f}'.format(FCCPCP),"\\\\\n")
  printCierreListadoCapacityFactor(fName) 

