# -*- coding: utf-8 -*-
# Imprime las reacciones of the nodes de la lista que se pasa como parámetro.
def listNodeReacciones(preprocessor,nmbComb,nodeListArranque, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Fx & Fy & Fz & Mx & My & Mz \\\\\n - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)
  nodes= preprocessor.getNodeLoader
  nodes.calculateNodalReactions()

  for iNode in nodeListArranque:
    fName.write(nmbComb," & ",iNode," & ")
    if(iNode>=0):
      nod= nodes.getNode(iNode)
      reac= nod.getReaction()
      fName.write(fmt.format(reac[0]/1e3)," & ",fmt.format(reac[1]/1e3)," & ",fmt.format(reac[2]/1e3)," & ")
      fName.write(fmt.format(reac[3]/1e3)," & ",fmt.format(reac[4]/1e3)," & ",fmt.format(reac[5]/1e3),"\\\\\n")
    else:
      fName.write("\\multicolumn{6}{|l|}{undefined node.}\\\\\n")
  cierraSupertabular(fName)

'''
Imprime las reacciones of the nodes asociados a la lista de puntos
   que se pasa como parámetro.
'''
def listaReaccionesPuntos(nmbComb,lstPuntosArranque, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdP & IdN & Fx & Fy & Fz & Mx & My & Mz \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,9,defCampos,idsCampos,caption)
  nodes= preprocessor.getNodeLoader
  nodes.calculateNodalReactions()

  for iArranque in lstPuntosArranque:
    pto= preprocessor.getCad.getPunto(iArranque)
    iNode= pto.getNodeTag()
    fName.write(nmbComb," & ",lstPuntosArranque[i]," & ",iNode," & ")
    if(iNode>=0):
      nod= nodes.getNode(iNode)
      reac= nod.getReaction()
      fName.write(fmt.format(reac[0]/1e3)," & ",fmt.format(reac[1]/1e3)," & ",fmt.format(reac[2]/1e3)," & ")
      fName.write(fmt.format(reac[3]/1e3)," & ",fmt.format(reac[4]/1e3)," & ",fmt.format(reac[5]/1e3),"\\\\\n")
    else:
      fName.write("\\multicolumn{6}{|l|}{undefined node.}\\\\\n")
  cierraSupertabular(fName)

'''
Imprime en formato CSV (comma separated values) las reacciones of the nodes asociados a la lista de puntos
   que se pasa como parámetro .
'''
def listaReaccionesPuntosCSV(preprocessor,nmbComb,lstPuntosArranque, fmt, fName):
  fName.write("Caso ; IdP ; IdN ; Fx ; Fy ; Fz ; Mx ; My ; Mz\n")
  fName.write(" - ; - ; - ; kN ; kN ; kN ; kN m ; kN m ; kN m \n")

  nodes= preprocessor.getNodeLoader
  nodes.calculateNodalReactions()

  for iArranque in lstPuntosArranque:
    pto= preprocessor.getCad.getPunto(iArranque)
    iNode= pto.getNodeTag()
    fName.write(nmbComb," ; ",lstPuntosArranque[i]," ; ",iNode," ; ")
    if(iNode>=0):
      nod= nodes.getNode(iNode)
      reac= nod.getReaction()
      fName.write(fmt.format(reac[0]/1e3)," ; ",fmt.format(reac[1]/1e3)," ; ",fmt.format(reac[2]/1e3)," ; ")
      fName.write(fmt.format(reac[3]/1e3)," ; ",fmt.format(reac[4]/1e3)," ; ",fmt.format(reac[5]/1e3),"\n")
    else:
      fName.write("undefined node.\n")

class RecordListadoReacciones(object):
  formato= '{:7.2f}'
  sectionHeadingA= "subsection"
  titulo= "Reacciones"
  sectionHeadingB= "subsubsection"
  nodeLists= []
  listaCabeceras= []

  def generaListadoReacciones(self,nmbComb, fName):
    fName.write("\\",self.sectionHeadingA,"{",titulo,"}\n")
    j= 0
    for l in self.nodeLists:
      listNodeReacciones(nmbComb,l,'{:7.2f}',fName,sectionHeadingB,self.listaCabeceras[j])
      j+= 1

