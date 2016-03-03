# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os

def plotTimeSerie(timeSerie, outputFile):
  # Imprime la serie temporal que se pasa como parámetro.
  pthLoad = NamedTemporaryFile(delete=False)
  pthLoadName= pthLoad.name
  pthLoad= nuevoArchivoTemporal("load","/tmp/load")
  tIni= timeSerie.getStart
  tFin= timeSerie.getFinish
  incT= timeSerie.getDuration/100
  for i in range(0,1.2*tFin+1):
    pthLoad.write(i,"\t",timeSerie.getFactor(i),"\n")
  pthLoad.close()

  pthTmp= NamedTemporaryFile(delete=False)
  pthTmpName= pthTmp.name
  pthTmp.write("set size 1.0, 0.6\n")
  pthTmp.write("set title \"Serie temporal: " + nmbTimeSerie +"\"\n")
  pthTmp.write("set terminal postscript portrait enhanced color dashed lw 1 \"Helvetica\" 14\n") 
  pthTmp.write("set output \"",outputFile,"\"\n")
  pthTmp.write("plot  \"",pthLoad,"\" using 1:2 title '"+nmbTimeSerie + "' with lines\n")
  pthTmp.close()
  os.system("gnuplot "+ pthTmpNmae)
  os.system("rm "+ pthTmpName)
  os.system("rm "+ pthLoadName)

# Imprime los datos del archivo que se pasa como parámetro.
def plotDataFile(nmbFile, outputFile, label):
  pthTmp= NamedTemporaryFile(delete=False)
  pthTmpName= pthTmp.name
  pthTmp.write("set size 1.0, 0.6\n")
  pthTmp.write("set title \"Serie temporal: " + nmbTimeSerie +"\"\n")
  pthTmp.write("set terminal postscript portrait enhanced color dashed lw 1 \"Helvetica\" 14\n") 
  pthTmp.write("set output \"",outputFile,"\"\n")
  pthTmp.write("set grid\n")
  pthTmp.write("plot  \"",nmbFile,"\" using 1:2 title '"+ label + "' with lines\n")
  pthTmp.close()
  os.system("gnuplot "+ pthTmpName)
  os.system("rm "+ pthTmp)
