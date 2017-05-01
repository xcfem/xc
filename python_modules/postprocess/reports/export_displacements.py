# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AOO)"
__copyright__= "Copyright 2016,LCPT, AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com"


import math



# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportDisplacements(combNm, nodSet, fDesc):
  '''Prints the specified file the resuls of displacements in a combination and set of nodes given as parameters 

  :param combNM: name of the combination
  :param  nodSet: set of nodes
  :param  fDesc:  name of the file to save the displacements
  '''
  fDesc.write(" Comb. , Node , Ux , Uy , Uz , ROTx , ROTy , ROTz \n")
  for n in nodSet:
    strDisp= str(n.getDisp).rstrip().replace(' ',', ') #displacement vector [ux,uy,uz,rotx,roty,rotz]
    fDesc.write(combNm+", "+str(n.tag)+", " + strDisp+'\n')
    #fDesc.write(combNm+", "+str(n.tag)+", " + str(vDisp[0])+", "+str(vDisp[1])+", "+str(vDisp[2])+", "+str(vDisp[3])+", "+str(vDisp[4])+", "+str(vDisp[5])+'\n')
