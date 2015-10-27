# -*- coding: utf-8 -*-
# Imprime las reacciones de los nodos de la lista que se pasa como parámetro.
def listaReaccionesNodos(preprocessor,nmbComb,lstNodosArranque, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Fx & Fy & Fz & Mx & My & Mz \\\\\n - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)
  nodos= preprocessor.getNodeLoader
  nodos.calculateNodalReactions()

  for iNodo in lstNodosArranque:
    fName.write(nmbComb," & ",iNodo," & ")
    if(iNodo>=0):
      nod= nodos.getNode(iNodo)
      reac= nod.getReaction()
      fName.write(fmt.format(reac[0]/1e3)," & ",fmt.format(reac[1]/1e3)," & ",fmt.format(reac[2]/1e3)," & ")
      fName.write(fmt.format(reac[3]/1e3)," & ",fmt.format(reac[4]/1e3)," & ",fmt.format(reac[5]/1e3),"\\\\\n")
    else:
      fName.write("\\multicolumn{6}{|l|}{nodo no definido.}\\\\\n")
  cierraSupertabular(fName)

'''
Imprime las reacciones de los nodos asociados a la lista de puntos
   que se pasa como parámetro.
'''
def listaReaccionesPuntos(nmbComb,lstPuntosArranque, fmt, fName, encab, tit):
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdP & IdN & Fx & Fy & Fz & Mx & My & Mz \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,9,defCampos,idsCampos,caption)
  nodos= preprocessor.getNodeLoader
  nodos.calculateNodalReactions()

  for iArranque in lstPuntosArranque:
    pto= preprocessor.getCad.getPunto(iArranque)
    iNodo= pto.getTagNodo()
    fName.write(nmbComb," & ",lstPuntosArranque[i]," & ",iNodo," & ")
    if(iNodo>=0):
      nod= nodos.getNode(iNodo)
      reac= nod.getReaction()
      fName.write(fmt.format(reac[0]/1e3)," & ",fmt.format(reac[1]/1e3)," & ",fmt.format(reac[2]/1e3)," & ")
      fName.write(fmt.format(reac[3]/1e3)," & ",fmt.format(reac[4]/1e3)," & ",fmt.format(reac[5]/1e3),"\\\\\n")
    else:
      fName.write("\\multicolumn{6}{|l|}{nodo no definido.}\\\\\n")
  cierraSupertabular(fName)

'''
Imprime en formato CSV (comma separated values) las reacciones de los nodos asociados a la lista de puntos
   que se pasa como parámetro .
'''
def listaReaccionesPuntosCSV(preprocessor,nmbComb,lstPuntosArranque, fmt, fName):
  fName.write("Caso ; IdP ; IdN ; Fx ; Fy ; Fz ; Mx ; My ; Mz\n")
  fName.write(" - ; - ; - ; kN ; kN ; kN ; kN m ; kN m ; kN m \n")

  nodos= preprocessor.getNodeLoader
  nodos.calculateNodalReactions()

  for iArranque in lstPuntosArranque:
    pto= preprocessor.getCad.getPunto(iArranque)
    iNodo= pto.getTagNodo()
    fName.write(nmbComb," ; ",lstPuntosArranque[i]," ; ",iNodo," ; ")
    if(iNodo>=0):
      nod= nodos.getNode(iNodo)
      reac= nod.getReaction()
      fName.write(fmt.format(reac[0]/1e3)," ; ",fmt.format(reac[1]/1e3)," ; ",fmt.format(reac[2]/1e3)," ; ")
      fName.write(fmt.format(reac[3]/1e3)," ; ",fmt.format(reac[4]/1e3)," ; ",fmt.format(reac[5]/1e3),"\n")
    else:
      fName.write("nodo no definido.\n")

class RecordListadoReacciones(object):
  formato= '{:7.2f}'
  sectionHeadingA= "subsection"
  titulo= "Reacciones"
  sectionHeadingB= "subsubsection"
  listasNodos= []
  listaCabeceras= []

  def generaListadoReacciones(self,nmbComb, fName):
    fName.write("\\",self.sectionHeadingA,"{",titulo,"}\n")
    j= 0
    for l in self.listasNodos:
      listaReaccionesNodos(nmbComb,l,'{:7.2f}',fName,sectionHeadingB,self.listaCabeceras[j])
      j+= 1

