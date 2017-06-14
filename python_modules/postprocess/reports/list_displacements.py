# -*- coding: utf-8 -*-
# Imprime los desplazamientos de los nodos de la lista se pasa como parámetro.
def listaDesplazamientosNodos(preprocessor,nmbComb,lstNodos, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)

  nodos= preprocessor.getNodeLoader
  for iNodo in lstNodos:
    fName.write(nmbComb," & ",iNodo," & ")
    nod= nodos.getNode(iNodo)
    disp= nod.getDisp()
    fName.write(fmt.format(disp[0]*1e3)," & ",fmt.format(disp[1]*1e3)," & ",fmt.format(disp[2]*1e3)," & ")
    fName.write(fmt.format(disp[3])," & ",fmt.format(disp[4])," & ",fmt.format(disp[5]),"\\\\\n")
  cierraSupertabular(fName)

'''
Imprime los desplazamientos de los nodos asociados a la lista de puntos
   que se pasa como parámetro.
'''
def listaDesplazamientosPuntos(preprocessor,nmbComb,lstPuntos, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdP & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & - & mm & mm & mm & rad & rad & rad "
  cabeceraSupertabular(fName,9,defCampos,idsCampos,caption)

  # Formamos la lista de nodos de arranque.
  for iPunto in lstPuntos:
    pnt= preprocessor.getCad.getPoint(iPunto)
    iNodo= pnt.getTagNodo()
    fName.write(nmbComb," & ",iPunto," & ",iNodo," & ")
    nod= nodos.getNode(iNodo)
    disp= nod.getDisp()
    fName.write(fmt.format(disp[0]*1e3)," & ",fmt.format(disp[1]*1e3)," & ",fmt.format(disp[2]*1e3)," & ")
    fName.write(fmt.format(disp[3])," & ",fmt.format(disp[4])," & ",fmt.format(disp[5]),"\\\\\n")
  cierraSupertabular(fName)

# Imprime los desplazamientos de los nodos contenidos en el conjunto que se pasa como parámetro.
def listaDesplazamientosNodosSet(preprocessor,nmbComb, setName, fmt, fName):
  s= preprocessor.getSets.getSet(setName)
  nodos= s.getNodos()
  for n in nodos:
    fName.write(nmbComb," & ",n.tag," & ")
    disp= nod.getDisp()
    fName.write(fmt.format(disp[0]*1e3)," & ",fmt.format(disp[1]*1e3)," & ",fmt.format(disp[2]*1e3)," & ")
    fName.write(fmt.format(disp[3])," & ",fmt.format(disp[4])," & ",fmt.format(disp[5]),"\\\\\n")

# Imprime los desplazamientos de los nodos contenidos en el conjunto que se pasa como parámetro.
def listaDesplazamientosNodosSET(nmbComb, setName, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Nodos del conjunto: "+setName
  defCampos= "|l|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)

  listaDesplazamientosNodosSet(nmbComb,setName,fmt,fName)

  cierraSupertabular(fName)

#execfile("modelo/model_inquiry.xcm")

# Imprime los desplazamientos de los nodos contenidos en las lineas del conjunto que se pasa como parámetro.
def listaDesplazamientosNodosLineas(preprocessor, nmbComb, setNameLineas, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Nodos del conjunto: "+setNameLineas
  defCampos= "|l|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)

  s= preprocessor.getSets.getSet(setNameLineas)
  lineas= s.getLineas
  for l in lineas:
    nmb= l.getName()
    fName.write("\hline\n")
    fName.write("\\multicolumn{8}{|l|}{Desplazamiento de los nodos de la linea: ",nmb,"}\\\\\n")
    fName.write("\hline\n")
    listaDesplazamientosNodosSet(nmbComb,nmb,fmt,fName)
    cierraSupertabular(fName)

class RecordListadoDesplazamientos(object):
  formato= '{:7.2f}'
  sectionHeadingA= "subsection"
  titulo= "Desplazamientos"
  sectionHeadingB= "subsubsection"
  listasPuntos= [] 
  listaCabeceras= [] 

  def generaListadoDesplazamientos(self, nmbComb, fName):
    fName.write("\\",sectionHeadingA,"{",titulo,"}\n")
    j= 0
    for l in listasPuntos:
      listaDesplazamientosPuntos(nmbComb,l,'{:7.3f}',fName,sectionHeadingB,listaCabeceras[j])
      j+= 1

