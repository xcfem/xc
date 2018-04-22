# -*- coding: utf-8 -*-

'''Generation of reactions repports.'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2016,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

def listNodeReactions(preprocessor,nmbComb,nodeListArranque, fmt, fName, encab, tit):
  '''Prints reactions for each of the nodes from the list.'''
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdN & Fx & Fy & Fz & Mx & My & Mz \\\\\n - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,8,defCampos,idsCampos,caption)
  nodes= preprocessor.getNodeHandler
  nodes.calculateNodalReactions(True,1e-7)

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

def listPointReactions(nmbComb,pointList, fmt, fName, encab, tit):
  '''
     Print the reactions associated with the nodes associated to the
     points from the list.
  '''
  fName.write("\\",encab,"{",tit,"}\n")
  caption= tit
  defCampos= "|l|r|r|r|r|r|r|r|r|"
  idsCampos= "Caso & IdP & IdN & Fx & Fy & Fz & Mx & My & Mz \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m "
  cabeceraSupertabular(fName,9,defCampos,idsCampos,caption)
  nodes= preprocessor.getNodeHandler
  nodes.calculateNodalReactions(True,1e-7)

  for iArranque in pointList:
    pto= preprocessor.getMultiBlockTopology.getPoint(iArranque)
    iNode= pto.getNodeTag()
    fName.write(nmbComb," & ",pointList[i]," & ",iNode," & ")
    if(iNode>=0):
      nod= nodes.getNode(iNode)
      reac= nod.getReaction()
      fName.write(fmt.format(reac[0]/1e3)," & ",fmt.format(reac[1]/1e3)," & ",fmt.format(reac[2]/1e3)," & ")
      fName.write(fmt.format(reac[3]/1e3)," & ",fmt.format(reac[4]/1e3)," & ",fmt.format(reac[5]/1e3),"\\\\\n")
    else:
      fName.write("\\multicolumn{6}{|l|}{undefined node.}\\\\\n")
  cierraSupertabular(fName)

def listPointReactionsCSV(preprocessor,nmbComb,pointList, fmt, fName):
  '''
     Prints the reactions in CSV (comma separated values) format.
  '''
  fName.write("Caso ; IdP ; IdN ; Fx ; Fy ; Fz ; Mx ; My ; Mz\n")
  fName.write(" - ; - ; - ; kN ; kN ; kN ; kN m ; kN m ; kN m \n")

  nodes= preprocessor.getNodeHandler
  nodes.calculateNodalReactions(True,1e-7)

  for iArranque in pointList:
    pto= preprocessor.getMultiBlockTopology.getPoint(iArranque)
    iNode= pto.getNodeTag()
    fName.write(nmbComb," ; ",pointList[i]," ; ",iNode," ; ")
    if(iNode>=0):
      nod= nodes.getNode(iNode)
      reac= nod.getReaction()
      fName.write(fmt.format(reac[0]/1e3)," ; ",fmt.format(reac[1]/1e3)," ; ",fmt.format(reac[2]/1e3)," ; ")
      fName.write(fmt.format(reac[3]/1e3)," ; ",fmt.format(reac[4]/1e3)," ; ",fmt.format(reac[5]/1e3),"\n")
    else:
      fName.write("undefined node.\n")

class ReactionsListRecord(object):
  formato= '{:7.2f}'
  sectionHeadingA= "subsection"
  titulo= "Reactions"
  sectionHeadingB= "subsubsection"
  nodeLists= []
  listaCabeceras= []

  def write(self,nmbComb, fName):
    fName.write("\\",self.sectionHeadingA,"{",titulo,"}\n")
    j= 0
    for l in self.nodeLists:
      listNodeReactions(nmbComb,l,'{:7.2f}',fName,sectionHeadingB,self.listaCabeceras[j])
      j+= 1

