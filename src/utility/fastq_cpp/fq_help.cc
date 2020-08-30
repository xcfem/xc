/* fq_help.f -- translated by f2c (version 20160102).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "fastq_cpp.h"

/*    Copyright(C) 1999-2020 National Technology & Engineering Solutions */
/*    of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with */
/*    NTESS, the U.S. Government retains certain rights in this software. */

/*    See packages/seacas/LICENSE for details */
/* Subroutine */ int help_fq__(integer *i__)
{
    extern /* Subroutine */ int mesage_(char *, ftnlen);

/* ************************************************************************ */
/*  SUBROUTINE HELP = WRITES HELP MESSAGES ONTO THE SCREEN */
/* ************************************************************************ */
/*  SUBROUTINE CALLED BY ANY ROUTINE NEEDED HELP MESSAGES */
/* ************************************************************************ */
/*  VARIABLES USED: */
/*     I = THE POINTER TO DESIGNATE WHICH MESSAGE IS NEEDED */
/* *********************************************************************** */
    if (*i__ == 1) {
	mesage_("        ", (ftnlen)8);
	mesage_("THE FOLLOWING MAIN OPTIONS ARE AVAILABLE:", (ftnlen)41);
	mesage_("     R*EAD       = READS FASTQ DATA FILE", (ftnlen)40);
	mesage_("     W*RITE      = WRITES FASTQ DATA FILE", (ftnlen)41);
	mesage_("     RW*RITE     = WRITES SPECIFIED REGIONS TO FASTQ DATA F\
ILE", (ftnlen)62);
	mesage_("     BW*RITE     = WRITES SPECIFIED BARSETS TO FASTQ DATA F\
ILE", (ftnlen)62);
	mesage_("     T*ABLET     = GEOMETRY INPUT FROM A DIGITIZING TABLET", 
		(ftnlen)58);
	mesage_("     S*TRAIGHTEN = STRAIGHTENS LINES IN X OR Y DIRECTION", (
		ftnlen)56);
	mesage_("     K*EY-IN     = INPUTS GEOMETRY FROM KEYBOARD", (ftnlen)
		48);
	mesage_("     G*RAPHICS   = DRAWS CURRENT FASTQ DATA", (ftnlen)43);
	mesage_("     L*IST       = LISTS FASTQ DATA", (ftnlen)35);
	mesage_("     M*ESH       = GENERATES THE MESH", (ftnlen)37);
	mesage_("     D*ELETE     = DELETES PORTIONS OF CURRENT FASTQ DATA", (
		ftnlen)57);
	mesage_("     F*LUSH      = CLEARS ALL FASTQ DATA", (ftnlen)40);
	mesage_("     EX*IT       = EXITS FASTQ", (ftnlen)30);
	mesage_("     SP*AWN      = SPAWNS A SUBPROCESS", (ftnlen)38);
    } else if (*i__ == 2) {
	mesage_(" ", (ftnlen)1);
	mesage_("|-----------------+-----------------+-----------------+----\
-------------+", (ftnlen)73);
	mesage_("| 0    CLOSEST    | 1     POINT     | 2 STRAIGHT LINE | 3  \
 CURSOR      |", (ftnlen)73);
	mesage_("|      (PREFIX)   |0- CLOSEST POINT |0- CLOSEST LINE  |    \
             |", (ftnlen)73);
	mesage_("|                 |C- POINT AT POINT|C- LINE ON LINE  |    \
             |", (ftnlen)73);
	mesage_("|                 |D- DELETE POINT  |D- DELETE LINE   |    \
             |", (ftnlen)73);
	mesage_("+-----------------+-----------------+-----------------+----\
-------------+", (ftnlen)73);
	mesage_("| 4  BISECT LINE  | 5    CCW ARC    | 6    CW ARC     | 7  \
 REGION      |", (ftnlen)73);
	mesage_("|                 |0- CLOSEST CCW   |0- CLOSEST CCW   |    \
             |", (ftnlen)73);
	mesage_("|                 |C- CCW ARC ON ARC|C- CW ARC ON ARC |    \
             |", (ftnlen)73);
	mesage_("|                 |                 |                 |    \
             |", (ftnlen)73);
	mesage_("+-----------------+-----------------+-----------------+----\
-------------+", (ftnlen)73);
	mesage_("| 8  MOVE POINT   | 9 REPAINT SCREEN| A TOGGLE GRID   | B Z\
OOM (BOX)    |", (ftnlen)73);
	mesage_("|                 |                 |                 |0- P\
REVIOUS ZOOM |", (ftnlen)73);
	mesage_("|                 |                 |D- DELETE GRID   |D- R\
ESET ZOOM    |", (ftnlen)73);
	mesage_("|                 |                 |                 |    \
             |", (ftnlen)73);
	mesage_("+-----------------+-----------------+-----------------+----\
-------------+", (ftnlen)73);
	mesage_("| C               | D               | E               | F  \
             |", (ftnlen)73);
	mesage_("|  SLIDE-LINE     |  DELETE         |  EXIT           |  (N\
OT USED)     |", (ftnlen)73);
	mesage_("|   (PREFIX)      |   (PREFIX)      |                 |    \
             |", (ftnlen)73);
	mesage_("|                 |                 |                 |    \
             |", (ftnlen)73);
	mesage_("+-----------------+-----------------+-----------------+----\
-------------+", (ftnlen)73);
    } else if (*i__ == 3) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING SCHEME AND STEP PROCESSING CONTROL OPTIONS AR\
E AVAILABLE:", (ftnlen)71);
	mesage_("    +A OR A = INCREASES SMOOTHING PARAMETER TO FORCE", (
		ftnlen)52);
	mesage_("              MORE EQUAL ELEMENT AREAS (DEFAULT = 0.7)", (
		ftnlen)54);
	mesage_("    -A      = DECREASES ABOVE SMOOTHING PARAMETER", (ftnlen)
		49);
	mesage_("     D      = DELETES THE ELEMENT WITH THE SMALLEST", (
		ftnlen)51);
	mesage_("              INTERIOR ANGLE (MUST BE < CURRENT MINIMUM ANG\
LE", (ftnlen)61);
	mesage_("              TO BE DELETED - SEE \"V\"", (ftnlen)37);
	mesage_("     E      = EXIT STEP PROCESSING SAVING REGION", (ftnlen)
		48);
	mesage_("    +F OR F = FREES (INCREASES) SMOOTHING RELAXATION PARAME\
TER", (ftnlen)62);
	mesage_("              (DEFAULT IS 1.)", (ftnlen)29);
	mesage_("    -F      = DECREASES SMOOTHING RELAXATION PARAMETER", (
		ftnlen)54);
	mesage_("    +I OR I = INCREASES MAXIMUM SMOOTHING ITERATIONS BY 50%",
		 (ftnlen)59);
	mesage_("              (DEFAULT IS 5 * NO. OF ELEMENTS)", (ftnlen)46);
	mesage_("    -I      = DECREASES MAXIMUM SMOOTHING ITERATIONS BY 33%",
		 (ftnlen)59);
	mesage_("    +J OR J = INCREASES SMOOTHING NODE MOVEMENT TOLERANCE", (
		ftnlen)57);
	mesage_("              BY 2**.333 (DEFAULT IS 3% OF ELEMENT SIDE LEN\
GTH)", (ftnlen)63);
	mesage_("    -J      = DECREASES SMOOTHING NODE MOVEMENT TOLERANCE", (
		ftnlen)57);
	mesage_("              BY 2**.333", (ftnlen)24);
	mesage_("     L      = INSERT AN INNER NECKLACES OF ELEMENTS AROUND \
A HOLE", (ftnlen)65);
	mesage_("     N      = NECKLACES THE REGION WITH A NEW ROW OF ELEMEN\
TS", (ftnlen)61);
	mesage_("     O      = RETURNS PROCESSING TO ORIGINAL MESH", (ftnlen)
		49);
	mesage_("     P      = PLOTS THE CURRENT MESH", (ftnlen)36);
	mesage_("     Q      = ENDS PROCESSING WITHOUT SAVING MESH (QUIT)", (
		ftnlen)56);
	mesage_("     R      = TRIES A RESTRUCTURE OF THE MESH", (ftnlen)45);
	mesage_("     S      = SMOOTHS THE MESH IF SOME ACTIVITY HAS CHANGED\
 IT", (ftnlen)62);
	mesage_("    +V OR V = INCREASES MAXIMUM ANGLE FOR DELETING ELEMENTS",
		 (ftnlen)59);
	mesage_("              (DEFAULT IS SET TO 45 DEGREES)", (ftnlen)44);
	mesage_("    -V      = DECREASES MAXIMUM ANGLE FOR DELETING ELEMENTS",
		 (ftnlen)59);
	mesage_("     W      = ATTEMPT RESTRUCTURE OF WORST ELEMENT ONLY", (
		ftnlen)55);
	mesage_("    +Y OR Y = INCREASES WEIGHT FOR ISOPARAMETRIC SMOOTHING", 
		(ftnlen)58);
	mesage_("              (DEFAULT IS SET TO 0.7)", (ftnlen)37);
	mesage_("    -Y      = DECREASES WEIGHT FOR ISOPARAMETRIC SMOOTHING", 
		(ftnlen)58);
	mesage_("     1      = SETS SMOOTHING TO EQUIPOTENTIAL IF MESH IS", (
		ftnlen)56);
	mesage_("              STRUCTURALLY THE SAME AS THE ORIGINAL", (
		ftnlen)51);
	mesage_("              (OTHERWISE #2 IS USED)", (ftnlen)36);
	mesage_("     2      = SETS SMOOTHING TO AREA PULL AND LAPLACIAN", (
		ftnlen)55);
	mesage_("     3      = SETS SMOOTHING TO CENTROID INVERSE AREA PUSH", 
		(ftnlen)58);
	mesage_("              AND LAPLACIAN", (ftnlen)27);
	mesage_("     4      = SETS SMOOTHING TO CENTROID AREA PULL", (ftnlen)
		50);
	mesage_("     5      = SETS SMOOTHING TO LAPLACIAN", (ftnlen)41);
	mesage_("     6      = SETS SMOOTHING TO LENGTH-WEIGHTED LAPLACIAN", (
		ftnlen)57);
	mesage_("     7      = SETS SMOOTHING TO WEIGHTED ISOPARAMETRIC AND \
LAPLACIAN", (ftnlen)68);
	mesage_("     (      = MARKS THE BEGINNING OF A LOOP", (ftnlen)43);
	mesage_("     )      = MARKS THE ENDING OF A LOOP", (ftnlen)40);
	mesage_("              (LOOP IS DONE WHEN NO ACTIVITY HAS OCCURRED", (
		ftnlen)57);
	mesage_("              SUCH AS A SMOOTH, DELETION, RESTRUCTURE, ETC.)"
		, (ftnlen)60);
    } else if (*i__ == 4) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING LIST OPTIONS ARE AVAILABLE:", (ftnlen)41);
	mesage_("    P*OINT      = LISTS POINT DATA", (ftnlen)34);
	mesage_("    L*INE       = LISTS LINE DATA", (ftnlen)33);
	mesage_("    SI*DE       = LISTS SIDE DATA", (ftnlen)33);
	mesage_("    BA*R SETS   = LISTS BAR SET DATA", (ftnlen)36);
	mesage_("    R*EGION     = LISTS REGION DATA", (ftnlen)35);
	mesage_("    HO*LE       = LISTS REGION'S HOLE DATA", (ftnlen)42);
	mesage_("    S*CHEME     = LISTS SCHEME DATA", (ftnlen)35);
	mesage_("    BOD*Y       = LISTS REGIONS IN THE BODY", (ftnlen)43);
	mesage_("    B*OUNDARY   = LISTS BOUNDARY FLAGS", (ftnlen)38);
	mesage_("    REN*UM      = LISTS RENUM CARDS", (ftnlen)35);
	mesage_("    T*HREE      = LISTS 3 NODE GENERATION TOGGLE", (ftnlen)
		48);
	mesage_("    EI*GHT      = LISTS 8 NODE GENERATION TOGGLE", (ftnlen)
		48);
	mesage_("    N*INE       = LISTS 9 NODE GENERATION TOGGLE", (ftnlen)
		48);
	mesage_("    SP*AWN      = SPAWNS A SUBPROCESS", (ftnlen)37);
	mesage_("    EX*IT       = EXITS FASTQ", (ftnlen)29);
	mesage_("                  (CARRIAGE RETURN TO EXIT LISTING)", (
		ftnlen)51);
    } else if (*i__ == 5) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING GRAPHICS OPTIONS ARE AVAILABLE:", (ftnlen)45);
	mesage_("    P*LOT       = PLOTS THE CURRENT DATA", (ftnlen)40);
	mesage_("    R*PLOT      = PLOTS THE DATA BY REGION", (ftnlen)42);
	mesage_("    B*PLOT      = PLOTS THE DATA BY BARSET", (ftnlen)42);
	mesage_("    SP*LOT      = PLOTS THE DATA BY SIDE", (ftnlen)40);
	mesage_("    LP*LOT      = PLOTS THE DATA BY LINE", (ftnlen)40);
	mesage_("    A*XIS       = TOGGLES DRAWING OF X-Y AXIS", (ftnlen)45);
	mesage_("    PO*INT      = TOGGLES DISPLAY OF POINT NO.S", (ftnlen)47)
		;
	mesage_("    L*INE       = TOGGLES DISPLAY OF LINE NO.S", (ftnlen)46);
	mesage_("    RE*GION     = TOGGLES DISPLAY OF REGION NO.S", (ftnlen)
		48);
	mesage_("    F*ULL       = TOGGLES FULL PROPERTY DISPLAY", (ftnlen)47)
		;
	mesage_("    PB*OUNDARY  = TOGGLES DISPLAY OF POINBC FLAGS", (ftnlen)
		49);
	mesage_("    N*BOUNDARY  = TOGGLES DISPLAY OF NODEBC FLAGS", (ftnlen)
		49);
	mesage_("    EB*OUNDARY  = TOGGLES DISPLAY OF ELEMBC FLAGS", (ftnlen)
		49);
	mesage_("    I*NTERVAL   = TOGGLES DISPLAY OF INTERVALS ON LINES", (
		ftnlen)55);
	mesage_("    FA*CTOR     = TOGGLES DISPLAY OF FACTORS ON LINES", (
		ftnlen)53);
	mesage_("    M*ATERIAL   = TOGGLES DISPLAY OF REGION BLOCK ID NO.S", (
		ftnlen)57);
	mesage_("    SC*HEME     = TOGGLES DISPLAY OF REGION SCHEMES", (
		ftnlen)51);
	mesage_("    S*TATUS     = DISPLAYS STATUS OF ALL TOGGLES", (ftnlen)
		48);
	mesage_("    Z*OOM       = SETS UP NEW PLOT LIMITS", (ftnlen)41);
	mesage_("    H*ARDCOPY   = HARDCOPY PLOT FILE OUTPUT", (ftnlen)43);
	mesage_("    II*NTERVAL  = INPUTS LINE INTERVALS", (ftnlen)39);
	mesage_("    IF*ACTOR    = INPUTS LINE FACTORS", (ftnlen)37);
	mesage_("    SPA*WN      = SPAWNS A SUBPROCESS", (ftnlen)37);
	mesage_("    EX*IT       = EXITS FASTQ", (ftnlen)29);
	mesage_("                  (CARRIAGE RETURN TO EXIT GRAPHICS)", (
		ftnlen)52);
    } else if (*i__ == 6) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING DELETE OPTIONS ARE AVAILABLE:", (ftnlen)43);
	mesage_("    P*OINT    = DELETES POINT DATA", (ftnlen)34);
	mesage_("    L*INE     = DELETES LINE DATA", (ftnlen)33);
	mesage_("    S*IDE     = DELETES SIDE DATA", (ftnlen)33);
	mesage_("    R*EGION   = DELETES REGION DATA", (ftnlen)35);
	mesage_("    BA*RSET   = DELETES BARSET DATA", (ftnlen)35);
	mesage_("    SC*HEME   = DELETES SCHEME DATA", (ftnlen)35);
	mesage_("    B*OUNDARY = DELETES BOUNDARY DATA", (ftnlen)37);
	mesage_("    REN*UM    = DELETES RENUMBERING CARDS", (ftnlen)41);
	mesage_("    SP*AWN    = SPAWNS A SUBPROCESS", (ftnlen)35);
	mesage_("    EX*IT     = EXITS FASTQ", (ftnlen)27);
	mesage_("                (CARRIAGE RETURN TO EXIT DELETE)", (ftnlen)
		48);
    } else if (*i__ == 7) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING DEVICES ARE AVAILABLE:", (ftnlen)36);
	mesage_("    AED = AED 512", (ftnlen)17);
	mesage_("    ALP = ALPHA NUMERIC TERMINAL", (ftnlen)32);
	mesage_("    LS5 = LEAR SIEGLER 220/230 (ENVISION)", (ftnlen)41);
	mesage_("    MET = METAFILE", (ftnlen)18);
	mesage_("    TEK = TEKTRONICS 4010", (ftnlen)25);
	mesage_("    TK4 = TEKTRONICS 4014", (ftnlen)25);
	mesage_("    T05 = TEKTRONICS 4105", (ftnlen)25);
	mesage_("    T07 = TEKTRONICS 4107, 4109, 4207, 4208", (ftnlen)43);
	mesage_("    T13 = TEKTRONICS 4113", (ftnlen)25);
	mesage_("    T15 = TEKTRONICS 4115", (ftnlen)25);
	mesage_("    V25 = VT 125", (ftnlen)16);
	mesage_("    V40 = VT 240", (ftnlen)16);
	mesage_("    R25 = RASTER TECH ONE-25", (ftnlen)28);
	mesage_("    RET = RETROGRAPHICS", (ftnlen)23);
    } else if (*i__ == 8) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING CORRECTION OPTIONS ARE AVAILABLE", (ftnlen)46);
	mesage_("    X     = CONSTANT X VALUES ALONG LINE(S)", (ftnlen)43);
	mesage_("    Y     = CONSTANT Y VALUES ALONG LINE(S)", (ftnlen)43);
	mesage_("    Z*ERO = ZERO X VALUES (CENTERLINES)", (ftnlen)39);
    } else if (*i__ == 9) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING KEYIN OPTIONS ARE AVAILABLE:", (ftnlen)42);
	mesage_("    T*ITLE    = ENTERS TITLE", (ftnlen)28);
	mesage_("    P*OINT    = ENTERS POINT DATA", (ftnlen)33);
	mesage_("    L*INE     = ENTERS LINE DATA", (ftnlen)32);
	mesage_("    SI*DE     = ENTERS SIDE DATA", (ftnlen)32);
	mesage_("    R*EGION   = ENTERS REGION DATA", (ftnlen)34);
	mesage_("    HO*LE     = ENTERS REGIONS'S HOLE DATA", (ftnlen)42);
	mesage_("    BA*R SET  = ENTERS BARSET DATA", (ftnlen)34);
	mesage_("    S*CHEME   = ENTERS SCHEME DATA", (ftnlen)34);
	mesage_("    BOD*Y     = ENTERS THE BODY LIST", (ftnlen)36);
	mesage_("    I*NTERVAL = ENTERS LINE INTERVALS", (ftnlen)37);
	mesage_("    F*ACTOR   = ENTERS LINE FACTORS", (ftnlen)35);
	mesage_("    B*OUNDARY = ENTERS LINE/POINT BOUNDARY DATA", (ftnlen)47)
		;
	mesage_("    W*EIGHT   = ENTERS A BOUNDARY FLAG WEIGHTING", (ftnlen)
		48);
	mesage_("    M*ATERIAL = ENTERS REGION MATERIAL NUMBERS", (ftnlen)46);
	mesage_("    REN*UM    = ENTERS RENUMBERING CARDS", (ftnlen)40);
	mesage_("    O*PTIMIZE = TOGGLES NUMBERING OPTIMIZATION", (ftnlen)46);
	mesage_("    TH*REE    = TOGGLES 3 NODE BAR GENERATION", (ftnlen)45);
	mesage_("    EI*GHT    = TOGGLES 8 NODE QUAD GENERATION", (ftnlen)46);
	mesage_("    N*INE     = TOGGLES 9 NODE QUAD GENERATION", (ftnlen)46);
	mesage_("    SP*AWN    = SPAWNS A SUBPROCESS", (ftnlen)35);
	mesage_("    EX*IT     = EXITS FASTQ", (ftnlen)27);
	mesage_("                (CARRIAGE RETURN TO EXIT KEYIN)", (ftnlen)47)
		;
    } else if (*i__ == 10) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING NUMBERING OPTIONS ARE AVAILABLE:", (ftnlen)46);
	mesage_("    P*-L-P      = ENTERS POINT-LINE-POINT SEQUENCE", (ftnlen)
		50);
	mesage_("    X*-Y        = ENTERS X-Y LOCATION TO START FROM", (
		ftnlen)51);
	mesage_("    N*ODE       = ENTERS NODE NUID'S LOCATION TO START FROM",
		 (ftnlen)59);
    } else if (*i__ == 11) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING MESH GRAPHICS OPTIONS ARE AVAILABLE:", (ftnlen)
		50);
	mesage_("    A*XIS       = TOGGLES DRAWING OF X-Y AXIS", (ftnlen)45);
	mesage_("    N*NUMBERING = TOGGLES DISPLAY OF NODE NUMBERS", (ftnlen)
		49);
	mesage_("    EN*UMBERING = TOGGLES DISPLAY OF ELEMENT NUMBERS", (
		ftnlen)52);
	mesage_("    MN*UMBERING = TOGGLES DISPLAY OF BLOCK ID NUMBERS", (
		ftnlen)53);
	mesage_("    O*ORDER     = TOGGLES DISPLAY OF OPTIMIZED ELEMENT ORDER"
		, (ftnlen)60);
	mesage_("    NB*OUNDARY  = TOGGLES DISPLAY OF NODAL BOUNDARIES", (
		ftnlen)53);
	mesage_("    EB*OUNDARY  = TOGGLES DISPLAY OF ELEMENT BOUNDARIES", (
		ftnlen)55);
	mesage_("    W*EIGHT     = TOGGLES DISPLAY OF WEIGHTING FACTORS", (
		ftnlen)54);
	mesage_("    S*TATUS     = DISPLAYS STATUS OF ALL TOGGLES", (ftnlen)
		48);
	mesage_("    P*LOT       = PLOTS THE MESH USING CURRENT ZOOM", (
		ftnlen)51);
	mesage_("    EP*LOT      = PLOTS THE MESH BASED ON ELEMENT NUMBERS", (
		ftnlen)57);
	mesage_("    R*PLOT      = PLOTS THE MESH BASED ON REGION NUMBERS", (
		ftnlen)56);
	mesage_("    B*PLOT      = PLOTS THE MESH BASED ON BARSET NUMBERS", (
		ftnlen)56);
	mesage_("    M*PLOT      = PLOTS THE MESH BASED ON MATERIAL NUMBERS", 
		(ftnlen)58);
	mesage_("    Z*OOM       = SETS UP NEW PLOT LIMITS", (ftnlen)41);
	mesage_("    H*ARDCOPY   = HARDCOPY PLOT FILE OUTPUT", (ftnlen)43);
	mesage_("    SP*AWN      = SPAWNS A SUBPROCESS", (ftnlen)37);
	mesage_("    EX*IT       = EXITS FASTQ", (ftnlen)29);
	mesage_("                  (CARRIAGE RETURN TO EXIT MESH GRAPHICS)", (
		ftnlen)57);
    } else if (*i__ == 12) {
	mesage_(" ", (ftnlen)1);
	mesage_("THE FOLLOWING MESH OPTIONS ARE AVAILABLE:", (ftnlen)41);
	mesage_("    P*ROCESS    = PROCESSES THE MESH", (ftnlen)36);
	mesage_("    S*TEP       = STEPS THROUGH PROCESSING INTERACTIVELY", (
		ftnlen)56);
	mesage_("    G*RAPHICS   = GRAPHICALLY DISPLAYS MESH", (ftnlen)43);
	mesage_("    I*NTERVALS  = ENTERS LINE INTERVALS", (ftnlen)39);
	mesage_("    F*ACTOR     = ENTERS LINE FACTORS", (ftnlen)37);
	mesage_("    SI*ZE       = ENTERS SIZES FOR REGIONS", (ftnlen)42);
	mesage_("    O*PTIMIZE   = TOGGLE FOR BANDWIDTH OPTIMIZATION", (
		ftnlen)51);
	mesage_("    T*HREE      = TOGGLE FOR 3 NODE BAR GENERATION", (ftnlen)
		50);
	mesage_("    EI*GHT      = TOGGLE FOR 8 NODE QUAD GENERATION", (
		ftnlen)51);
	mesage_("    NI*NE       = TOGGLE FOR 9 NODE QUAD GENERATION", (
		ftnlen)51);
	mesage_("    R*EAD       = READS MESH DATA FROM A FILE", (ftnlen)45);
	mesage_("    R*MESH      = REMESHES BASED ON AN ERROR ESTIMATE", (
		ftnlen)53);
	mesage_("    AD*JUST     = ADJUSTS A GENERATED MESH", (ftnlen)42);
	mesage_("    W*RITE      = WRITES A GENESIS MESH FILE", (ftnlen)44);
	mesage_("    A*BAQUS     = WRITES AN ABAQUS MESH FILE", (ftnlen)44);
	mesage_("    N*ASTRAN    = WRITES A NASTRAN MESH FILE", (ftnlen)44);
	mesage_("    SP*AWN      = SPAWNS A SUBPROCESS", (ftnlen)37);
	mesage_("    EX*IT       = EXITS FASTQ", (ftnlen)29);
	mesage_("    D*ISTORTION = CALCULATES A DISTORTION INDEX", (ftnlen)47)
		;
	mesage_("                  (CARRIAGE RETURN TO EXIT MESH)", (ftnlen)
		48);
    } else if (*i__ == 13) {
	mesage_("THE FOLLOWING INITIAL MESH GENERATION SCHEMES ARE AVAILABLE:"
		, (ftnlen)60);
	mesage_("    B      = TRANSITION REGION GENERATION", (ftnlen)41);
	mesage_("    C      = SEMICIRCULAR REGION GENERATION", (ftnlen)43);
	mesage_("    M      = AUTOMATED RECTANGULAR MESH GENERATION", (ftnlen)
		50);
	mesage_("    T      = TRIANGULAR REGION GENERATION", (ftnlen)41);
	mesage_("    U      = PENATGON REGION GENERATION", (ftnlen)39);
	mesage_("    X      = PAVING MESH GENERATION", (ftnlen)35);
	mesage_("    Z      = HOLE REGION GENERATION", (ftnlen)35);
	mesage_("             NO SCHEME (CARRIAGE RETURN) DEFAULTS TO A", (
		ftnlen)54);
	mesage_("             FORCED RECTANGULAR SCHEME", (ftnlen)38);
    } else if (*i__ == 14) {
	mesage_("THE FOLLOWING TABLET OPTIONS ARE AVAILABLE: ", (ftnlen)44);
	mesage_("    A*XIS       = TOGGLES DRAWING OF X-Y AXIS", (ftnlen)45);
	mesage_("    B*UTTONS    = SHOWS THE MOUSE BUTTON DEFINITIONS", (
		ftnlen)52);
	mesage_("    C*LEAR GRID = ERASES ALL GRID LINES", (ftnlen)39);
	mesage_("    XC*LEAR     = ERASES ALL X GRID LINES", (ftnlen)41);
	mesage_("    YC*LEAR     = ERASES ALL Y GRID LINES", (ftnlen)41);
	mesage_("    D*IGITIZE   = DIGITZES GEOMETRY WITH MOUSE/TABLET", (
		ftnlen)53);
	mesage_("    DE*FAULT    = USES THE ZOOM LIMITS TO INITIALIZE TABLET",
		 (ftnlen)59);
	mesage_("    I*NITIALIZE = INITIALIZES A DRAWING TO THE TABLET", (
		ftnlen)53);
	mesage_("    S*NAP       = TOGGLES THE SNAP MODE FOR GRID INTERSECTI\
ONS", (ftnlen)62);
	mesage_("    U*NIFORM    = ADDS UNIFORMLY SPACED SQUARE GRID LINES", (
		ftnlen)57);
	mesage_("    UX          = ADDS UNIFORMLY X SPACED GRID LINES", (
		ftnlen)52);
	mesage_("    UY          = ADDS UNIFORMLY Y SPACED GRID LINES", (
		ftnlen)52);
	mesage_("    X*GRID      = ADDS ARBITRARY X GRID LINES", (ftnlen)45);
	mesage_("    Y*GRID      = ADDS ARBITRARY Y GRID LINES", (ftnlen)45);
	mesage_("    P*OINT GRID = ADDS X AND Y GRIDS THROUGH ALL EXISTING P\
OINTS", (ftnlen)64);
	mesage_("    Z*OOM       = SETS PLOTTING (AND  TABLET) LIMITS", (
		ftnlen)52);
	mesage_("    SP*AWN      = SPAWNS A SUBPROCESS", (ftnlen)37);
	mesage_("    EX*IT       = EXITS FASTQ", (ftnlen)29);
	mesage_("                  (CARRIAGE RETURN TO EXIT TABLET)", (ftnlen)
		50);
    }
    return 0;
} /* help_fq__ */

#ifdef __cplusplus
	}
#endif
