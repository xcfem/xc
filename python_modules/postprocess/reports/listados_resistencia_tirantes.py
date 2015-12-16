# -*- coding: utf-8 -*-
# Imprime el encabezamiento para el listado de tensiones normales.
def printCabeceraListadoTirante(archivo, tit):
  archivo.write("\\begin{center}\n")
  archivo.write("\\begin{footnotesize}\n")
  archivo.write("\\tablefirsthead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{4}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & FC \\\\\n")
  archivo.write("      &      & (kN) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablehead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{4}{|l|}{\small\sl \ldots continúa}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{4}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & FC \\\\\n")
  archivo.write("      &      & (kN) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tabletail{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{4}{|r|}{\small\sl continúa\ldots}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablelasttail{\hline}\n")
  archivo.write("\\begin{supertabular}{|l|r|r|r|r|r|}\n")

# Imprime el cierre del listado de de tensiones normales.
def printCierreListadoTirante(archivo):
  archivo.write("\\end{supertabular}\n")
  archivo.write("\\end{footnotesize}\n")
  archivo.write("\\end{center}\n")

def listaResistenciaTirantes(preprocessor,setName, fName, titulo):
  '''
  Imprime los resultados de la comprobación de tensiones normales en régimen
  elástico de un conjunto de elementos que representa un tirante.
  '''
  printCabeceraListadoTirante(fName,titulo)
  st= preprocessor.getSetLoader.getSet(setName)
  elems= st.getElements()
  tagFCTNCPCP= 0
  FCTNCPCP= 0
  HIPCPCPTN= ""
  for e in elems:
    fName.write(e.tag," & ",HIPCPTN," & ",'{:5.2f}'.format(e.getProp("NCP")/1e3)," & ",'{:3.2f}'.format(e.getProp("FCTNCP")),"\\\\\n")
    if(e.getProp("FCTNCP")>FCTNCPCP):
      FCTNCPCP= e.getProp("FCTNCP")
      HIPCPCPTN= e.getProp("HIPCPTN")
      tagFCTNCPCP= e.tag
  fName.write("\\hline\n")
  fName.write(tagFCTNCPCP," & ",HIPCPCPTN," &  & ",'{:3.2f}'.format(FCTNCPCP),"\\\\\n")    
  printCierreListadoTirante(fName) 


