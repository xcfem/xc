# -*- coding: utf-8 -*-

def snap(fScript,value):
  fScript.write("_snap ")
  fScript.write('{:.8f}'.format(value))
  fScript.write("\n")

def coo2D(fScript,x,y):
  fScript.write('{}'.format(x))
  fScript.write(",")
  fScript.write('{}'.format(y))
  fScript.write("\n")

def coo3D(fScript,x,y,z):
  fScript.write('{}'.format(x))
  fScript.write(",")
  fScript.write('{}'.format(y))
  fScript.write(",")
  fScript.write('{}'.format(z))
  fScript.write("\n")

def point(fScript,x,y,z):
  fScript.write("_point\n")
  coo3D(fScript,x,y,z)
  

def text(fScript,x,y,just,size,angle,msg):
  fScript.write(";\n")
  fScript.write("_text\n")
  fScript.write("_j\n")
  fScript.write(just+"\n")
  coo2D(fScript,x,y)
  fScript.write('{}'.format(size)+'\n')
  fScript.write('{}'.format(angle)+'\n')
  fScript.write(msg+'\n')
  fScript.write(";\n")

def comment(fScript,text):
  fScript.write("; "+text+"\n")  

def pline(fScript,pointsPline):
  fScript.write("_pline\n")
  for p in pointsPline:
    coo2D(fScript,p[0],p[1])
  fScript.write("\n")

def line2D(fScript,pA,pB):
  fScript.write("_line\n")
  coo2D(fScript,pA[0],pA[1])
  coo2D(fScript,pB[0],pB[1])
  fScript.write("\n")

def pline3D(fScript,pointsPline):
  fScript.write("_3DPOLY\n")
  for p in pointsPline:
    coo3D(fScript,p[0],p[1],p[2])
  fScript.write("\n")

def zoomExtents(fScript):
  fScript.write("_zoom _e\n")

def save(fScript,fName):
  fScript.write("_save "+'"'+fName+'"\n')

def makeLayer(fScript, layerName, colorName):
  fScript.write('_layer _make "'+ layerName +'" _color ' + colorName + ' "' + layerName + '"\n\n')

def currentLayer(fScript, layerName):
  fScript.write('_clayer "'+ layerName + '"\n\n')

  
