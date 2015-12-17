# -*- coding: utf-8 -*-
# Imprime el encabezamiento para el listado de tensiones normales.
def printCabeceraListadoTNPerfilMetalico(archivo, tit):
  archivo.write("\\begin{center}\n")
  archivo.write("\\begin{footnotesize}\n")
  archivo.write("\\tablefirsthead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & $M_y$  & $M_z$  & FC \\\\\n")
  archivo.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablehead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|l|}{\small\sl \ldots continúa}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & $M_y$  & $M_z$  & FC \\\\\n")
  archivo.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tabletail{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|r|}{\small\sl continúa\ldots}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablelasttail{\hline}\n")
  archivo.write("\\begin{supertabular}{|l|r|r|r|r|r|}\n")


# Imprime el cierre del listado de de tensiones normales.
def printCierreListadoTNPerfilMetalico(archivo):
  archivo.write("\\end{supertabular}\n")
  archivo.write("\\end{footnotesize}\n")
  archivo.write("\\end{center}\n")


def listaResistenciaPerfilesMetalicosTNRegElastico(setName, fName, titulo):
  '''
  Imprime los resultados de la comprobación de tensiones normales en régimen
  elástico de un conjunto de elementos cuyo material es un perfil
  metálico.'''
  printCabeceraListadoTNPerfilMetalico(fName,titulo) 
  st= preprocessor.getSetLoader.getSet(setName)
  elems= st.getElements()
  tagFCTNCPCP= 0
  FCTNCPCP= 0
  HIPCPCPTN= ""
  for e in elems:
    fName.write(tag," & ",HIPCPTN," & ",'{:5.2f}',format(e.getProp("NCP")/1e3)," & ",'{:5.2f}'.format(e.getProp("MyCP")/1e3)," & ",'{:5.2f}'.format(e.getProp("MzCP")/1e3)," & ",'{:3.2f}'.format(e.getProp("FCTNCP")),"\\\\\n")
    if(e.getProp("FCTNCP")>FCTNCPCP):
      FCTNCPCP= e.getProp("FCTNCP")
      HIPCPCPTN= e.getProp("HIPCPTN")
      tagFCTNCPCP= e.tag
  fName.write("\\hline\n")
  fName.write("\\multicolumn{6}{|c|}{Caso pésimo}\\\\\n")
  fName.write("\\hline\n")
  fName.write(tagFCTNCPCP," & ",HIPCPCPTN," &  &  &  & ",'{:3.2f}'.format(FCTNCPCP),"\\\\\n")

  printCierreListadoTNPerfilMetalico(fName) 


# Imprime el encabezamiento para el listado de cortantes.
def printCabeceraListadoVPerfilMetalico(archivo, tit):
  archivo.write("\\begin{center}\n")
  archivo.write("\\begin{footnotesize}\n")
  archivo.write("\\tablefirsthead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{5}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $V_y$  & $V_z$  & FC \\\\\n")
  archivo.write("      &      & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablehead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{5}{|l|}{\small\sl \ldots continúa}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{5}{|c|}{",tit,"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $V_y$  & $V_z$  & FC \\\\\n")
  archivo.write("      &      & (kN m) & (kN m) &    \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tabletail{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{5}{|r|}{\small\sl continúa\ldots}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablelasttail{\hline}\n")
  archivo.write("\\begin{supertabular}{|l|r|r|r|r|r|}\n")

'''
Imprime los resultados de la comprobación de tensiones cortantes
de un conjunto de elementos cuyo material es un perfil metálico.
'''
def listaResistenciaPerfilesMetalicosV(setName, fName, titulo):
  printCabeceraListadoVPerfilMetalico(fName,titulo) 
  st= preprocessor.getSetLoader.getSet(setName)
  elems= st.getElements()
  tagFCTNCPCP= 0
  FCTNCPCP= 0
  HIPCPCPTN= ""
  for e in elems:
    fName.write(tag," & ",HIPCPV," & ",'{:5.2f}'.format(e.getProp("VyCP")/1e3)," & ",'{:5.2f}'.format(e.getProp("VzCP")/1e3)," & ",'{:3.2f}'.format(e.getProp("FCVCP")),"\\\\\n")
    if(e.getProp("FCTNCP")>FCTNCPCP):
      FCTNCPCP= e.getProp("FCTNCP")
      HIPCPCPTN= e.getProp("HIPCPTN")
      tagFCTNCPCP= e.tag
  fName.write("\\hline\n")
  fName.write("\\multicolumn{5}{|c|}{Caso pésimo}\\\\\n")
  fName.write("\\hline\n")
  fName.write(tagFCVCPCP," & ",HIPCPCPV," &  &  & ",format(e.getProp("FCVCPCP"),'{:3.2f}'),"\\\\\n")
  printCierreListadoTNPerfilMetalico(fName) 

