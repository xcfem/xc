# -*- coding: utf-8 -*-

# Imprime el encabezamiento para el listado de factores de capacidad.
def printCabeceraListadoFactorCapacidad(archivo, nmbSecc):
  archivo.write("\\begin{center}\n")
  archivo.write("\\begin{footnotesize}\n")
  archivo.write("\\tablefirsthead{%\n")
  archivo.write("\\hline\n")
  # archivo.write("\\multicolumn{6}{|c|}{\\textbf{",nmbArch,"}}\\\\\n")
  archivo.write("\\multicolumn{6}{|c|}{Comprobación de la sección ",toLaTex(nmbSecc),"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & $M_x$  & $M_y$  & FC \\\\\n")
  archivo.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablehead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|l|}{\small\sl \ldots continúa}\\\\\n")
  archivo.write("\\hline\n")
  # archivo.write("\\multicolumn{6}{|c|}{\\textbf{",nmbArch,"}}\\\\\n")
  archivo.write("\\multicolumn{6}{|c|}{Comprobación de la sección ",toLaTex(nmbSecc),"}\\\\\n")
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

# Imprime el cierre del listado de factores de capacidad.
def printCierreListadoFactorCapacidad(archivo):
  archivo.write("\\end{supertabular}\n")
  archivo.write("\\end{footnotesize}\n")
  archivo.write("\\end{center}\n")


'''
Imprime los resultados de la comprobación de tensiones normales en régimen
elástico de un conjunto de elementos cuyo material es un perfil
metálico.
'''
def listaFactorCapacidad(preprocessor,nmbSet, fName, titulo):
  printCabeceraListadoFactorCapacidad(fName,titulo)
  st= preprocessor.getSetLoader.getSet(nmbSet)
  elems= st.getElements()
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
  printCierreListadoFactorCapacidad(fName) 

