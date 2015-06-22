#!/usr/bin/perl -w
# conv.pl
# Conversión de un archivo de acciones en elementos de Ansys.
open(INFILE,$ARGV[0]);

$accion_actual= "";

while($line= <INFILE>)
  {
    if($line =~ m/^ Accion .*/)
      {
        @acc=split(/:/,$line);
        @tmp=split(/\s+/,$acc[0]);
        $accion_actual=$tmp[2];
        $accion_actual=~ s/([A-Z])-/$1/g;
        $accion_actual=~ s/([0-9])-([0-9])/$1#$2/g;
      }
    else
      {
        if($line =~ m/^ +[0-9]+ .*/)
          {
            $line=~ s/([0-9])-/$1 -/g;
            $line=~ s/\s+$//g;
            $line=~ s/\s+/,/g;
            print "$accion_actual$line\n";
          }
      }
  }
