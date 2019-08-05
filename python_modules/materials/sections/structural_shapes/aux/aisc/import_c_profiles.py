# -*- coding: utf-8 -*-
import csv

fNameIn= 'aisc_metric_c_profiles.csv'
fNameOut= 'dict_aisc_c_shapes.py'
dictName= 'CShapes'

def writeCShapeRecord(out,row):
  name= row[70]
  if(name.find('C')!=-1):
    out.write(dictName+"['" + row[70] + "']= ")
    out.write("{")
    out.write("\'nmb\':\'" + row[70] +"\', ")
    out.write("\'P\':" + row[72]+", ")
    out.write("\'A\':" + row[73]+"e-6, ")
    out.write("\'h\':" + row[74]+"e-3, ")
    out.write("\'b\':" + row[78]+"e-3, ")
    out.write("\'tw\':" + row[82]+"e-3, ")
    out.write("\'tf\':" + row[85]+"e-3, ")
    out.write("\'kDes\':" + row[90]+"e-3, ")
    out.write("\'x\':" + row[93]+"e-3, ")
    out.write("\'e0\':" + row[95]+"e-3, ")
    out.write("\'xp\':" + row[96]+"e-3, ")
    out.write("\'Iz\':" + row[103]+"e-6, ")
    out.write("\'Wzpl\':" + row[104]+"e-6, ")
    out.write("\'Wzel\':" + row[105]+"e-6, ")
    out.write("\'iz\':" + row[106]+"e-3, ")
    out.write("\'Iy\':" + row[107]+"e-6, ")
    out.write("\'Wypl\':" + row[108]+"e-6, ")
    out.write("\'Wyel\':" + row[109]+"e-6, ")
    out.write("\'iy\':" + row[110]+"e-3, ")
    out.write("\'It\':" + row[114]+"e-9, ")
    out.write("\'Cw\':" + row[115]+"e-9, ")
    out.write("\'E\': 210000e6, ")
    out.write("\'nu\': 0.3")
    out.write(" }\n")

outfile= open(fNameOut,mode='w')
with open(fNameIn, mode='r') as infile:
  reader = csv.reader(infile)
  for rows in reader:
    if(len(rows)>0):
      writeCShapeRecord(outfile,rows)
