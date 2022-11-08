# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import tempfile

def plotTimeSerie(timeSeries, outputFile):
    ''' Print the time series argument.'''
    pthLoad = tempfile.NamedTemporaryFile(delete=False)
    pthLoadName= pthLoad.name
    #tIni= timeSeries.getStart
    tFin= timeSeries.getFinish
    #incT= timeSeries.getDuration/100
    for i in range(0,1.2*tFin+1):
        pthLoad.write(i,"\t",timeSeries.getFactor(i),"\n")
    pthLoad.close()

    pthTmp= tempfile.NamedTemporaryFile(delete=False)
    pthTmpName= pthTmp.name
    pthTmp.write("set size 1.0, 0.6\n")
    pthTmp.write("set title \"Serie temporal: " + timeSeries.name +"\"\n")
    pthTmp.write("set terminal postscript portrait enhanced color dashed lw 1 \"Helvetica\" 14\n") 
    pthTmp.write("set output \"",outputFile,"\"\n")
    pthTmp.write("plot  \"",pthLoad,"\" using 1:2 title '"+timeSeries.name + "' with lines\n")
    pthTmp.close()
    os.system("gnuplot "+ pthTmpName)
    os.system("rm "+ pthTmpName)
    os.system("rm "+ pthLoadName)

def plotDataFile(fileName, outputFile, label):
    ''' Plots the data read from the file argument.

    :param fileName: name of the input file.
    :param outputFile: name of the output file.
    :param label: label for the plot.
    '''
    pthTmp= tempfile.NamedTemporaryFile(delete=False)
    pthTmpName= pthTmp.name
    pthTmp.write("set size 1.0, 0.6\n")
    #pthTmp.write("set title \"Serie temporal: " + timeSeries.name +"\"\n")
    pthTmp.write("set terminal postscript portrait enhanced color dashed lw 1 \"Helvetica\" 14\n") 
    pthTmp.write("set output \"",outputFile,"\"\n")
    pthTmp.write("set grid\n")
    pthTmp.write("plot  \"",fileName,"\" using 1:2 title '"+ label + "' with lines\n")
    pthTmp.close()
    os.system("gnuplot "+ pthTmpName)
    os.system("rm "+ pthTmp)
