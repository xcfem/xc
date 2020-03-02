# -*- coding: utf-8 -*-
''' Tata CHS profiles'''

import csv

fNameIn= 'tata_steel_chs_shapes.csv'
fNameOut= 'dict_tata_steel_chs_shapes.py'
dictName= 'CHSshapes'


def writeCHSShapeRecord(out,row):
  name= row[0]
  if(name.find('CHS')!=-1):
    out.write(dictName+"['" + row[0] + "']= ")
    out.write("{")
    out.write("\'nmb\':\'" + row[0] +"\', ")
    out.write("\'D\':" + row[1]+"e-3, ")
    out.write("\'t\':" + row[2]+"e-3, ")
    out.write("\'P\':" + row[3]+", ")
    out.write("\'A\':" + row[4]+"e-4, ")
    out.write("\'Iz\':" + row[6]+"e-8, ")
    out.write("\'iz\':" + row[7]+"e-2, ")
    out.write("\'Wzel\':" + row[8]+"e-6, ")
    out.write("\'Wzpl\':" + row[9]+"e-6, ")
    out.write("\'Iy\':" + row[6]+"e-8, ")
    out.write("\'iy\':" + row[7]+"e-2, ")
    out.write("\'Wyel\':" + row[8]+"e-6, ")
    out.write("\'Wypl\':" + row[9]+"e-6, ")
    out.write("\'It\':" + row[10]+"e-8, ")
    out.write("\'C\':" + row[11]+"e-6, ")
    out.write("\'AL\':" + row[12]+", ")
    out.write("\'AG\':" + row[13]+", ")

    out.write("\'E\': 210000e6, ")
    out.write("\'nu\': 0.3")
    out.write(" }\n")

outfile= open(fNameOut,mode='w')
with open(fNameIn, mode='r') as infile:
  reader = csv.reader(infile)
  for rows in reader:
    if(len(rows)>0):
      writeCHSShapeRecord(outfile,rows)
