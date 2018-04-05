# -*- coding: utf-8 -*-
# Imprime los desplazamientos of the nodes de la lista se pasa como parámetro.
def listNodeDisplacements(preprocessor,nmbComb,nodeList, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)

  nodes= preprocessor.getNodeHandler
  for iNode in nodeList:
    fName.write(nmbComb," & ",iNode," & ")
    nod= nodes.getNode(iNode)
    disp= nod.getDisp()
    fName.write(fmt.format(disp[0]*1e3)," & ",fmt.format(disp[1]*1e3)," & ",fmt.format(disp[2]*1e3)," & ")
    fName.write(fmt.format(disp[3])," & ",fmt.format(disp[4])," & ",fmt.format(disp[5]),"\\\\\n")
  cierraSupertabular(fName)

def listPointsDisplacements(preprocessor,nmbComb,pointList, fmt, fName, encab, tit):
  ''' Writes a list of the displacements of the nodes associated to the points.

      :param nmbComb: name of the combination for which the displacements
                      were obtained.
      :param pointList: list of points.
      :param fmt: format for numbers.
      :param fName: output file name.
      :param encab: header.
      :param tit: title.
  '''
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdP & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & - & mm & mm & mm & rad & rad & rad "
  cabeceraSupertabular(fName,9,defCampos,idsCampos,caption)

  # Formamos la lista de nodes de arranque.
  for iPoint in pointList:
    pnt= preprocessor.getMultiBlockTopology.getPoint(iPoint)
    iNode= pnt.getNodeTag()
    fName.write(nmbComb," & ",iPoint," & ",iNode," & ")
    nod= nodes.getNode(iNode)
    disp= nod.getDisp()
    fName.write(fmt.format(disp[0]*1e3)," & ",fmt.format(disp[1]*1e3)," & ",fmt.format(disp[2]*1e3)," & ")
    fName.write(fmt.format(disp[3])," & ",fmt.format(disp[4])," & ",fmt.format(disp[5]),"\\\\\n")
  cierraSupertabular(fName)

# Imprime los desplazamientos de los nodes contenidos en el conjunto que se pasa como parámetro.
def listNodeDisplacementsSet(preprocessor,nmbComb, setName, fmt, fName):
  s= preprocessor.getSets.getSet(setName)
  nodes= s.getNodes()
  for n in nodes:
    fName.write(nmbComb," & ",n.tag," & ")
    disp= nod.getDisp()
    fName.write(fmt.format(disp[0]*1e3)," & ",fmt.format(disp[1]*1e3)," & ",fmt.format(disp[2]*1e3)," & ")
    fName.write(fmt.format(disp[3])," & ",fmt.format(disp[4])," & ",fmt.format(disp[5]),"\\\\\n")

# Imprime los desplazamientos of the nodes contenidos en el conjunto que se pasa como parámetro.
def listNodeDisplacementsSET(nmbComb, setName, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Nodes from set: "+setName
  defCampos= "|l|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)

  listNodeDisplacementsSet(nmbComb,setName,fmt,fName)

  cierraSupertabular(fName)

#execfile("modelo/model_inquiry.xcm")

# Imprime los desplazamientos of the nodes contenidos en las lineas del conjunto que se pasa como parámetro.
def listNodeDisplacementsLineas(preprocessor, nmbComb, setNameLineas, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= "Nodes from set: "+setNameLineas
  defCampos= "|l|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)

  s= preprocessor.getSets.getSet(setNameLineas)
  lineas= s.getLineas
  for l in lineas:
    nmb= l.getName()
    fName.write("\hline\n")
    fName.write("\\multicolumn{8}{|l|}{Desplazamiento of the nodes de la linea: ",nmb,"}\\\\\n")
    fName.write("\hline\n")
    listNodeDisplacementsSet(nmbComb,nmb,fmt,fName)
    cierraSupertabular(fName)

class RecordListadoDesplazamientos(object):
  formato= '{:7.2f}'
  sectionHeadingA= "subsection"
  titulo= "Desplazamientos"
  sectionHeadingB= "subsubsection"
  pointsLists= [] 
  listaCabeceras= [] 

  def generaListadoDesplazamientos(self, nmbComb, fName):
    fName.write("\\",sectionHeadingA,"{",titulo,"}\n")
    j= 0
    for l in pointsLists:
      listPointsDisplacements(nmbComb,l,'{:7.3f}',fName,sectionHeadingB,listaCabeceras[j])
      j+= 1

