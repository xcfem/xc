      PROGRAM ITPTST (OUTPUT, TAPE6=OUTPUT)     
C       
C     CHANGES TO BE MADE FOR USE ON DIFFERENT COMPUTERS:  
C     1. REMOVE OR CHANGE PROGRAM LINE ABOVE OR OPEN LINE BELOW     
C     2. CHANGE THE VALUE OF SRELPR BELOW AND IN ITPACK ROUTINE DFAULT
C     3. CHANGES IN THE ITPACK TIMING ROUTINE TIMER       
C       
C     OPEN(UNIT=6,DEVICE='DSK',ACCESS='SEQOUT',FILE='OUT.LPT')      
C       
C       
C     MACHINE PRECISION  SRELPR       
C       
C     SRELPR = 7.1E-15   FOR CDC CYBER 170/750  (APPROX.) 2**-47    
C          = 1.49E-8   FOR DEC 10  (APPROX.) 2**-26       
C          = 1.192E-7  FOR VAX 11/780 (APPROX.) 2**-23    
C          = 9.534E-7  FOR IBM 370 (APPROX.)    
C       
      SRELPR = 7.1E-15      
      CALL TEST1(SRELPR)    
      CALL TEST2(SRELPR)    
      CALL TEST3(SRELPR)    
      CALL TEST4(SRELPR)    
      STOP
      END 
      SUBROUTINE TEST1(EPSI)
C       
C ... TEST1 IS A PROGRAM DESIGNED TO TEST ITPACK 2C METHODS ON      
C ... MATRICES ARISING FROM THE SYMMETRIC FIVE POINT DISCRETIZATION 
C ... OF TWO DIMENSIONAL ELLIPTIC PARTIAL DIFFERENTIAL EQUATIONS ON 
C ... A RECTANGLE WITH A RECTANGULAR MESH.  ALL SEVEN METHODS FROM  
C ... ITPACK 2C ARE TESTED AND A SUMMARY IS PRINTED AT THE END.     
C       
C     THE EXACT SIZE OF THE TEST PROBLEMS CAN BE INCREASED OR       
C     DECREASED BY CHANGING THE ARRAY SIZE IN DIMENSION STATEMENTS  
C     AND THE VARIABLES LISTED BELOW UNDER SIZE OF TEST PROBLEM.    
C     ALSO, THE NUMBER OF TIMES THROUGH THE TEST LOOPS CAN BE REDUCED 
C     BY CHANGING  ITEST AND JTEST  AS FOLLOWS. 
C       
C          ITEST = 1 FOR SYMMETRIC STORAGE TEST 
C                = 2 FOR SYMMETRIC AND NONSYMMETRIC STORAGE TEST    
C          JTEST = 1 FOR NATURAL ORDERING TEST  
C                = 2 FOR NATURAL AND RED-BLACK ORDERING TEST
C       
C     ARRAY DIMENSIONING    
C       
      REAL    A(1729),RHS(361),U(361),WKSP(2606),RPARM(12), 
     A        GRIDX(21),GRIDY(21),DIGIT1(7),DIGIT2(7),TIM1(7),TIM2(7) 
      INTEGER IA(362),JA(1729),IWKSP(1083),IPARM(12),IWORK(1729),   
     A        BCTYPE(4),ITER(7),IWRK(7) 
C       
C ... SYM5PT COMMON BLOCKS  
C       
      COMMON /TBK11/ AX, AY, BX, BY, SRELPR, HX, HY       
      COMMON /TBK12/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,N
C       
C       
C ... INITIALIZE INTEGER CONSTANTS WHICH CONTROL OUTPUT AND DEFINE  
C ... ARRAY DIMENSIONS. THEY ARE      
C       
C        SRELPR    -  MACHINE PRECISION 
C        NOUT    -  FORTRAN OUTPUT UNIT 
C        LEVEL   -  LEVEL OF OUTPUT FROM ITPACK 2C
C        IERAN    -  ERROR ANALYSIS SWITCH      
C        ITMAX   -  MAXIMUM NUMBER OF ITERATIONS ALLOWED  
C        ZETA    -  STOPPING CRITERION
C        NW      -  SIZE OF THE REAL ARRAY WKSP.
C        ILEVEL  -  0/1 LEVEL OF OUTPUT FROM SYM7PT       
C        MXNEQ   -  MAXIMUM NUMBER OF EQUATIONS TO BE GENERATED (I.E. 
C                   THE MAXIMUM NUMBER OF INTERIOR AND NON-DIRICHLET
C                   BOUNDARY POINTS)  
C        NELMAX  -  MAXIMUM NUMBER OF NON-ZERO ENTRIES IN THE UPPER 
C                   TRIANGULAR PART OF THE RESULTING SYMMETRIC MATRIX 
C        NGRIDX  -  NUMBER OF HORIZONTAL MESH PLANES.     
C        NGRIDY  -  NUMBER OF VERTICAL MESH PLANES.       
C        NGRDXD  -  MAXIMUM NUMBER OF VERTICAL MESH PLANES INCLUDING THE
C                   VERTICAL BOUNDARY PLANES.   
C        NGRDYD  -  MAXIMUM NUMBER OF HORIZONTAL MESH PLANES INCLUDING
C                   THE HORIZONTAL BOUNDARY PLANES.       
C       
      SRELPR = EPSI 
C       
C     SET ITPACK SWITCHES   
C       
      NOUT = 6    
      LEVEL = 1   
      IERAN = 0   
      ITMAX = 110 
      ZETA = 0.5E-5 
      NW = 2606   
C       
C     SET NUMBER OF TIMES THROUGH TEST LOOPS    
C       
      ITEST = 2   
      JTEST = 2   
C       
C     SET SIZE OF TEST PROBLEM
C       
      ILEVEL = 0  
      MXNEQ = 361 
      NELMAX = 1729 
      NGRIDX = 21 
      NGRIDY = 21 
C       
      NGRDXD = NGRIDX       
      NGRDYD = NGRIDY       
      WRITE(NOUT,10)
   10 FORMAT(1H1 //15X,32HITPACK 2C  TEST PROGRAM -- TEST1
     A            /15X,23HTESTS ITERATIVE MODULES 
     B            /15X,40HUSES FIVE POINT SYMMETRIC DISCRETIZATION  ) 
C       
C ... SET UP DEFAULT VALUES FOR BCTYPE AND INITIALIZE INFORMATION ARRAYS
C       
C ... BCTYPE DEFINES THE BOUNDARY CONDITIONS ON THE EDGES OF THE    
C ... RECTANGLE.  WHERE     
C       
C        BCTYPE(I) = 0 IMPLIES THE ITH SIDE IS NEUMANN OR MIXED     
C                  = 1 IMPLIES THE ITH SIDE IS DIRICHLET  
C       
C          AND I = 1 IMPLIES THE EAST  SIDE DEFINED BY (BX, Y)      
C                = 2 IMPLIES THE SOUTH SIDE DEFINED BY ( X,AY)      
C                = 3 IMPLIES THE WEST  SIDE DEFINED BY (AX, Y)      
C                = 4 IMPLIES THE NORTH SIDE DEFINED BY ( X,BY)      
C       
      DO 20 I = 1,4 
         BCTYPE(I) = 1      
   20 CONTINUE    
C       
C ... DEFINE THE DISCRETIZATION MESH  
C       
C       AX      -  MINIMUM X VALUE ON THE RECTANGLE (WEST SIDE)     
C       BX      -  MAXIMUM X VALUE ON THE RECTANGLE (EAST SIDE)     
C       GRIDX   -  REAL ARRAY CONTAINING THE X-COORDINATE OF THE    
C                  HORIZONTAL MESH LINES FROM WEST TO EAST. 
C                  THESE ARE UNIFORM BUT THAT IS NOT REQUIRED.      
C       AY      -  MINIMUM Y VALUE ON THE RECTANGLE (SOUTH SIDE)    
C       BY      -  MAXIMUM Y VALUE ON THE RECTANGLE (NORTH SIDE)    
C       GRIDY   -  REAL ARRAY CONTAINING THE Y-COORDINATE OF THE    
C                  VERTICAL MESH LINES FROM SOUTH TO NORTH. 
C                  THESE ARE  UNIFORM BUT THAT IS NOT REQUIRED.     
C       
      AX = 0.E0   
      BX = 1.E0   
      HX = (BX-AX)/  FLOAT(NGRIDX-1)  
      DO 40 J = 1,  NGRIDX  
         GRIDX(J) = AX + FLOAT(J-1)*HX
   40 CONTINUE    
      GRIDX(NGRIDX) = BX    
C       
      AY = 0.E0   
      BY = 1.E0   
      HY = (BY-AY)/  FLOAT(NGRIDY-1)  
      DO 50 J = 1,  NGRIDY  
         GRIDY(J) = AY + FLOAT(J-1)*HY
   50 CONTINUE    
      GRIDY(NGRIDY) = BY    
C       
C ... DISCRETIZE THE ELLIPTIC PDE     
C       
      DO 60 LOOP1=1,ITEST   
      ISYM = LOOP1 - 1      
      IF(LOOP1 .EQ. 2) WRITE(NOUT,63) 
   63 FORMAT(1H1///)
      CALL SYM5PT(GRIDX,NGRDXD,GRIDY,NGRDYD,RHS,MXNEQ,IA,JA,A,NELMAX, 
     A            IWORK)    
C       
C       
C ... SOLVE THE MATRIX PROBLEM
C       
      DO 60 LOOP2=1,JTEST   
      NB = LOOP2 - 2
      IF(ISYM .EQ. 0) WRITE(NOUT,91)  
      IF(ISYM .EQ. 1) WRITE(NOUT,92)  
      IF(NB .EQ. (-1)) WRITE(NOUT,93) 
      IF(NB .EQ. 0)  WRITE(NOUT,94)   
C       
C        TEST JCG 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL JCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(1)=RPARM(9)   
         TIM2(1)=RPARM(10)  
         DIGIT1(1)=RPARM(11)
         DIGIT2(1)=RPARM(12)
         ITER(1)=IPARM(1)   
         IWRK(1)=IPARM(8)   
C       
C        TEST JSI 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL JSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(2)=RPARM(9)   
         TIM2(2)=RPARM(10)  
         DIGIT1(2)=RPARM(11)
         DIGIT2(2)=RPARM(12)
         ITER(2)=IPARM(1)   
         IWRK(2)=IPARM(8)   
C       
C        TEST SOR 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL SOR (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(3)=RPARM(9)   
         TIM2(3)=RPARM(10)  
         DIGIT1(3)=RPARM(11)
         DIGIT2(3)=RPARM(12)
         ITER(3)=IPARM(1)   
         IWRK(3)=IPARM(8)   
C       
C        TEST SSORCG
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL SSORCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)
         ITER(4)=IPARM(1)   
         IWRK(4)=IPARM(8)   
         TIM1(4)=RPARM(9)   
         TIM2(4)=RPARM(10)  
         DIGIT1(4)=RPARM(11)
         DIGIT2(4)=RPARM(12)
C       
C        TEST SSORSI
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL SSORSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)
         TIM1(5)=RPARM(9)   
         TIM2(5)=RPARM(10)  
         DIGIT1(5)=RPARM(11)
         DIGIT2(5)=RPARM(12)
         ITER(5)=IPARM(1)   
         IWRK(5)=IPARM(8)   
C       
C        TEST RSCG
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL RSCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  
         TIM1(6)=RPARM(9)   
         TIM2(6)=RPARM(10)  
         DIGIT1(6)=RPARM(11)
         DIGIT2(6)=RPARM(12)
         ITER(6)=IPARM(1)   
         IWRK(6)=IPARM(8)   
C       
C        TEST RSSI
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL RSSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  
         TIM1(7)=RPARM(9)   
         TIM2(7)=RPARM(10)  
         DIGIT1(7)=RPARM(11)
         DIGIT2(7)=RPARM(12)
         ITER(7)=IPARM(1)   
         IWRK(7)=IPARM(8)   
C       
C     TIMING ANALYSIS       
C       
         CALL TIME1 (N,IA,JA,A,WKSP,WKSP(N+1),ITER,TIM1,TIM2,       
     A                DIGIT1,DIGIT2,IWRK,NOUT)  
   60 CONTINUE    
C       
   91 FORMAT(//15X,29HSYMMETRIC SPARSE STORAGE USED )     
   92 FORMAT(//15X,32HNONSYMMETRIC SPARSE STORAGE USED )  
   93 FORMAT(15X,21HNATURAL ORDERING USED )     
   94 FORMAT(15X,23HRED-BLACK ORDERING USED )   
      RETURN      
      END 
      SUBROUTINE TIME1 (N,IA,JA,A,V,W,ITER,TIM1,TIM2,DIGIT1,DIGIT2, 
     A                   IWORK,NOUT)  
      INTEGER ITER(7),IA(1),JA(1),IWORK(7)      
      REAL V(N),W(N),REL(7),A(1),DIGIT1(7),DIGIT2(7),     
     A     TIM1(7),TIM2(7)  
      DATA MAXLP/20/
C       
      CALL VFILL (N,V,1.E0) 
      TIMI1 = TIMER(0.0)    
      DO 10 I=1,MAXLP       
         CALL PMULT (N,IA,JA,A,V,W)   
   10 CONTINUE    
      TIMI2 = TIMER(0.0)    
      TIMMAT = (TIMI2 - TIMI1)/FLOAT(MAXLP)     
C       
      WRITE (NOUT,30)       
      DO 20 I=1,7 
          REL(I) = 0.E0     
          TEMP = FLOAT(ITER(I))       
          IF ((TIMI2 .EQ. TIMI1) .OR. (ITER(I) .EQ. 0))  GO TO 20   
          REL(I) = TIM1(I)/(TEMP*TIMMAT)
   20 CONTINUE    
C       
      WRITE (NOUT,40) (TIM1(I),TIM2(I),ITER(I),REL(I),DIGIT1(I),    
     A                   DIGIT2(I),IWORK(I),I=1,7)
C       
      RETURN      
C       
   30 FORMAT(1H0,3(/),15X,15HTIMING ANALYSIS/15X,15(1H-)//
     A 35X,9HITERATION,3X,5HTOTAL,9X,6HNO. OF,4X,9HAVG ITER/,       
     B 2X,6HDIGITS,4X,6HDIGITS,5X,4HWKSP/15X,   
     C 6HMETHOD,14X,10HTIME (SEC),2X,10HTIME (SEC),1X,    
     D 10HITERATIONS,1X,12HMTX-VCTR MLT,1X,7HSTP TST,     
     E 3X,7HRES/RHS,4X,4HUSED/15X,103(1H-))     
   40 FORMAT(15X,17HJACOBI CG        ,2F10.3,I14,F10.1,1X,2F10.1,I10/ 
     2       15X,17HJACOBI SI        ,2F10.3,I14,F10.1,1X,2F10.1,I10/ 
     3       15X,17HSOR              ,2F10.3,I14,F10.1,1X,2F10.1,I10/ 
     4       15X,17HSYMMETRIC SOR CG ,2F10.3,I14,F10.1,1X,2F10.1,I10/ 
     5       15X,17HSYMMETRIC SOR SI ,2F10.3,I14,F10.1,1X,2F10.1,I10/ 
     6       15X,17HREDUCED SYSTEM CG,2F10.3,I14,F10.1,1X,2F10.1,I10/ 
     7       15X,17HREDUCED SYSTEM SI,2F10.3,I14,F10.1,1X,2F10.1,I10/)
C       
C       
      END 
      SUBROUTINE PDE1(X,Y,CVALUS)     
C       
C ... THIS SUBROUTINE IS A USER SUPPLIED SUBROUTINE TO SPECIFY THE  
C ... SELF-ADJOINT ELLIPTIC PDE FOR SYM5PT IN THE FOLLOWING FORM    
C       
C        (CVALUS(1)*UX)X + (CVALUS(3)*UY)Y + CVALUS(6)*U = CVALUS(7)
C       
C     NOTE:  CVALUS(I), FOR I = 2, 4, AND 5 ARE NOT USED. 
C       
         REAL CVALUS(7)     
C       
         CVALUS( 1) = 1.E0  
         CVALUS( 2) = 0.E0  
         CVALUS( 3) = 2.E0  
         CVALUS( 4) = 0.E0  
         CVALUS( 5) = 0.E0  
         CVALUS( 6) = 0.E0  
         CVALUS( 7) = 0.E0  
C       
         RETURN   
      END 
      REAL FUNCTION BCOND1(ISIDE,X,Y,BVALUS)    
C       
C ... THIS REAL FUNCTION IS A USER SUPPLIED FUNCTION TO SPECIFY THE 
C ... BOUNDARY CONDITIONS OF THE ELLIPTIC PDE DEPENDING ON ISIDE, X,
C ... AND Y.      
C       
C        IF ISIDE = 1, THEN X = BX (EAST SIDE)  
C                 = 2, THEN Y = AY (SOUTH SIDE) 
C                 = 3, THEN X = AX (WEST SIDE)  
C                 = 4, THEN Y = BY (NORTH SIDE) 
C       
C ... THE BVALUS ARRAY IS DEFINED AS FOLLOWS    
C       
C        BVALUS(1)*U + BVALUS(2)*UX + BVALUS(3)*UY = BVALUS(4)      
C       
C        NOTE:  BCOND1 IS SET TO BVALUS(4) BEFORE RETURNING.
C       
         REAL BVALUS(4)     
C       
         GOTO(10,20,30,40),ISIDE      
C       
   10    BVALUS(1) = 1.E0   
         BVALUS(4) = 1.E0+ X * Y      
         BVALUS(2) = 0.E0   
         BVALUS(3) = 0.E0   
         GO TO 999
C       
   20    BVALUS(1) = 1.E0   
         BVALUS(4) = 1.E0+ X * Y      
         BVALUS(2) = 0.E0   
         BVALUS(3) = 0.E0   
         GO TO 999
C       
   30    BVALUS(1) = 1.E0   
         BVALUS(4) = 1.E0+ X * Y      
         BVALUS(2) = 0.E0   
         BVALUS(3) = 0.E0   
         GO TO 999
C       
   40    BVALUS(1) = 1.E0   
         BVALUS(4) = 1.E0+ X * Y      
         BVALUS(2) = 0.E0   
         BVALUS(3) = 0.E0   
C       
  999    BCOND1 = BVALUS(4) 
         RETURN   
      END 
      SUBROUTINE SYM5PT (GRIDX,NGRDXD,GRIDY,NGRDYD,RHS,   
     A                   MXNEQ,IA,JA,A,NELS,IWKSP)
C       
      INTEGER IA(1),JA(NELS),BCTYPE(4),IWKSP(NELS)
      REAL    GRIDX(NGRDXD),GRIDY(NGRDYD),RHS(MXNEQ),A(NELS)
C       
C       
C ... SYM5PT / SYM7PT COMMON BLOCKS   
C       
      COMMON /TBK11/ AX, AY, BX, BY, SRELPR, HX, HY       
      COMMON /TBK12/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,N
C       
C       
         IF (NGRIDX .LT. 3 .OR. NGRIDY .LT. 3)    GO TO 120 
C       
C     DETERMINE RANGE OF UNKNOWN GRID POINTS    
C       
      IX1 = 1     
      IX2 = NGRIDX
      JY1 = 1     
      JY2 = NGRIDY
         IF (BCTYPE(1) .EQ. 1) IX2 = NGRIDX - 1 
         IF (BCTYPE(2) .EQ. 1) JY1 = 2
         IF (BCTYPE(3) .EQ. 1) IX1 = 2
         IF (BCTYPE(4) .EQ. 1) JY2 = NGRIDY - 1 
      LNGTHX = IX2 - IX1 + 1
      N = (JY2 - JY1 + 1) * LNGTHX    
         IF (N .GT. MXNEQ) GO TO 260  
C       
C     OUTPUT INITIAL GRID INFORMATION 
C       
         IF (ILEVEL .EQ. 0) GO TO 70  
      WRITE(NOUT,20) AX,BX,AY,BY      
   20 FORMAT(/10X,24HFINITE DIFFERENCE MODULE  ,6H ---- , 
     A       20HSYMMETRIC FIVE POINT /10X,21HDOMAIN = RECTANGLE  (, 
     B       E11.4,1H,,E11.4,5H) X (,E11.4,1H,,E11.4,1H)  ) 
C       
      WRITE(NOUT,30)
   30 FORMAT(/10X,35HCOEFFICIENTS OF VERTICAL MESH LINES    )       
      WRITE(NOUT,40) (GRIDX(I),I=1,NGRIDX)      
   40 FORMAT(/8X,8(2X,E11.4) )
      WRITE(NOUT,50)
   50 FORMAT(/10X,37HCOEFFICIENTS OF HORIZONTAL MESH LINES  )       
      WRITE(NOUT,40) (GRIDY(I),I=1,NGRIDY)      
      WRITE(NOUT,60) (BCTYPE(I), I=1,4) 
   60 FORMAT(/10X,42HBOUNDARY CONDITIONS ON PIECES 1,2,3,4 ARE   ,  
     A       3(1X,I1,1H,),1X,I1,1H.  )
C       
C     GENERATE EQUATIONS ONE MESH POINT AT A TIME 
C       
   70 CALL SBINI(N,NELS,IA,JA,A,IWKSP)
         IXADD = 0
         JYADD = 0
            IF (BCTYPE(3) .EQ. 1) IXADD = 1     
            IF (BCTYPE(2) .EQ. 1) JYADD = 1     
C       
      DO 100 IJ = 1,N       
         IXX = MOD(IJ-1,LNGTHX) + 1   
         JYY = (IJ - IXX) / LNGTHX + 1
            IXX = IXX + IXADD 
            JYY = JYY + JYADD 
         HN = 0.E0
         HS = 0.E0
         HE = 0.E0
         HW = 0.E0
         PX = GRIDX(IXX)    
         PY = GRIDY(JYY)    
            IF (IXX .NE. 1) HW = PX - GRIDX(IXX - 1)      
            IF (IXX .NE. NGRIDX) HE = GRIDX(IXX+1) - PX   
            IF (JYY .NE. 1) HS = PY - GRIDY(JYY - 1)      
            IF (JYY .NE. NGRIDY) HN = GRIDY(JYY+1) - PY   
C       
               INIJ=IJ      
               CALL PNT2D(PX,PY,INIJ,IXX,JYY,LNGTHX,HN,HS,HW,HE,    
     A                    B,NELS,IA,JA,A,IWKSP) 
C       
         RHS(IJ) = B
  100    CONTINUE 
C       
      CALL SBEND(N,NELS,IA,JA,A,IWKSP)
C     NORMAL EXIT 
C       
         IF (ILEVEL .EQ. 0) RETURN    
C       
      WRITE(NOUT,110)       
  110 FORMAT(/10X,30HSYM5PT COMPLETED SUCCESSFULLY.  )    
      NU = IA(N+1) - 1      
      NUU = N + 1 + 2*NU    
      WRITE(NOUT,115) N,NU,NUU
  115 FORMAT( 10X,38HSPARSE MATRIX REPRESENTATION FINISHED. 
     A       /15X,25HNO. OF EQUATIONS        =,I8 
     B       /15X,25HNO. OF NON-ZEROES       =,I8 
     C       /15X,25HTOTAL MATRIX STORAGE    =,I8  /)     
C       
      RETURN      
C       
C     ERROR EXITS 
C       
  120 CONTINUE    
         IF (NGRIDX .LT. 3)  WRITE(NOUT,160)    
  160 FORMAT(/10X,30HSYM5PT ERROR -- NGRIDX .LT. 3     )  
         IF (NGRIDY .LT. 3)  WRITE(NOUT,170)    
  170 FORMAT(/10X,30HSYM5PT ERROR -- NGRIDY .LT. 3     )  
C       
      STOP
C       
  260 WRITE(NOUT,270) N,MXNEQ 
  270 FORMAT(/10X,17HN .GT. MXNEQ, N =,I10,8H MXNEQ =,I10)
      STOP
C       
      END 
      SUBROUTINE PNT2D(PCX,PCY,IJ,IX,JY,LNGTHX,HN,HS,HW,HE, 
     A                 B,NELS,IA,JA,A,IWKSP)    
C       
      REAL    CVALUS(7),BVALUS(4),A(NELS)       
      INTEGER IA(1),JA(NELS),BCTYPE(4),IWKSP(NELS)
C       
C ... SYM5PT / SYM7PT COMMON BLOCKS   
C       
      COMMON /TBK11/ AX, AY, BX, BY, SRELPR, HX, HY       
      COMMON /TBK12/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,N
C       
C       
C ... INITIALIZE COEFFICIENTS 
C       
      CEAST = 0.E0
      CWEST = 0.E0
      CNORTH = 0.E0 
      CSOUTH = 0.E0 
      CALL PDE1(PCX,PCY,CVALUS)       
      CENTER = -CVALUS(6)*(HN+HS)*(HE+HW)/4.E0  
      CRHS  = -CVALUS(7)*(HN+HS)*(HE+HW)/4.E0   
C       
C     SET EAST COEFFICIENT  
C       
      CALL PDE1 (PCX+.5E0*HE,PCY,CVALUS)
         IF (IX .GE. NGRIDX-1) TEMP = BCOND1(1,BX,PCY,BVALUS)       
         IF (IX .EQ. NGRIDX) GO TO 20 
         IF ((IX+1 .EQ. NGRIDX) .AND. (BCTYPE(1) .EQ. 1)) GO TO 10  
C       
C     NORMAL EAST POINT     
C       
      CEAST = -CVALUS(1)*(HN+HS)/(2.E0*HE)      
      CENTER = CENTER - CEAST 
            GO TO 30
C       
C     EAST POINT IS A DIRICHLET POINT 
C       
   10 TEMP = CVALUS(1)*(HN+HS)/(2.E0*HE)
      CRHS = CRHS + BVALUS(4)*TEMP/BVALUS(1)    
      CENTER = CENTER + TEMP
            GO TO 30
C       
C     CENTER POINT LIES ON THE EAST BOUNDARY WHICH IS MIXED 
C       
   20 TEMP = CVALUS(1)*(HN+HS)/(2.E0*BVALUS(2)) 
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(4)*TEMP    
C       
C     SET WEST COEFFICIENT  
C       
   30 CALL PDE1(PCX - .5E0*HW,PCY,CVALUS)       
         IF (IX .LE. 2) TEMP = BCOND1(3,AX,PCY,BVALUS)    
         IF (IX .EQ. 1) GO TO 50      
         IF (IX .EQ. 2 .AND. BCTYPE(3).EQ.1) GO TO 40     
C       
C     NORMAL WEST POINT     
C       
      CWEST = -CVALUS(1)*(HN+HS)/(2.E0*HW)      
      CENTER = CENTER - CWEST 
            GO TO 60
C       
C     WEST POINT IS DIRICHLET 
C       
   40 TEMP = CVALUS(1)*(HN+HS)/(2.E0*HW)
      CRHS = CRHS + BVALUS(4)*TEMP/BVALUS(1)    
      CENTER = CENTER + TEMP
            GO TO 60
C       
C     CENTER POINT LIES ON WEST BOUNDARY WHICH IS MIXED.  
C       
   50 TEMP = CVALUS(1)*(HN + HS)/(2.E0*BVALUS(2)) 
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(4)*TEMP    
C       
C     SET NORTH COEFFICIENTS
C       
   60 CALL PDE1(PCX,PCY+.5E0*HN,CVALUS) 
         IF (JY .GE. NGRIDY-1) TEMP = BCOND1(4,PCX,BY,BVALUS)       
         IF (JY .EQ. NGRIDY) GO TO 80 
         IF ((JY.EQ.NGRIDY-1) .AND. BCTYPE(4).EQ.1) GO TO 70
C       
C     NORMAL NORTH POINT    
C       
      CNORTH = -CVALUS(3) * (HE + HW)/(2.E0*HN) 
      CENTER = CENTER - CNORTH
            GO TO 90
C       
C     NORTH POINT IS DIRICHLET
C       
   70 TEMP = CVALUS(3) * (HE + HW) / (2.E0*HN)  
      CRHS = CRHS + BVALUS(4)*TEMP / BVALUS(1)  
      CENTER = CENTER + TEMP
            GO TO 90
C       
C     CENTER POINT LIES ON NORTHERN MIXED BOUNDARY
C       
   80 TEMP = CVALUS(3)*(HE + HW)/(2.E0*BVALUS(3)) 
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(4) * TEMP  
C       
C     SET SOUTH COEFFICIENTS
C       
   90 CALL PDE1(PCX,PCY-.5E0*HS,CVALUS) 
         IF (JY .LE. 2) TEMP = BCOND1(2,PCX,AY,BVALUS)    
         IF (JY .EQ. 1) GO TO 110     
         IF (JY .EQ. 2 .AND. BCTYPE(2) .EQ. 1) GO TO 100  
C       
C     NORMAL SOUTH POINT    
C       
      CSOUTH = - CVALUS(3)*(HE+HW)/(2.E0*HS)    
      CENTER = CENTER - CSOUTH
            GO TO 120       
C       
C     SOUTH POINT IS DIRICHLET
C       
  100 TEMP = CVALUS(3) * (HE + HW) / (2.E0 * HS)
      CRHS = CRHS + BVALUS(4)*TEMP/BVALUS(1)    
      CENTER = CENTER + TEMP
            GO TO 120       
C       
C     CENTER POINT LIES ON SOUTHERN MIXED BOUNDARY
C       
  110 TEMP = CVALUS(3)*(HE+HW)/(2.E0*BVALUS(3)) 
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(4)*TEMP    
C       
C     COEFFICIENT GENERATION IS COMPLETED.      
C     NOW SET B (RHS) AND IA,JA, AND A (MATRIX REPRESENTATION)      
C       
  120 B = CRHS    
C       
      IF (ILEVEL .EQ. 1) WRITE(NOUT,130) IJ,CRHS
  130 FORMAT(/10X,18HSYM5PT -- EQUATION,I8 /20X,
     A       24HRIGHT HAND SIDE =       ,E15.7) 
C       
      CALL STVAL1(NELS,IA,JA,A,IJ,IJ,CENTER,IWKSP)
      CALL STVAL1(NELS,IA,JA,A,IJ,IJ+1,CEAST,IWKSP)       
      CALL STVAL1(NELS,IA,JA,A,IJ,IJ-LNGTHX,CSOUTH,IWKSP) 
      CALL STVAL1(NELS,IA,JA,A,IJ,IJ-1,CWEST,IWKSP)       
      CALL STVAL1(NELS,IA,JA,A,IJ,IJ+LNGTHX,CNORTH,IWKSP) 
C       
      RETURN      
      END 
      SUBROUTINE STVAL1(NELS,IA,JA,A,I,J,VAL,IWKSP)       
         REAL A(NELS)       
         INTEGER IA(1),JA(NELS),IWKSP(NELS),BCTYPE(4)     
C       
      COMMON /TBK11/ AX, AY, BX, BY, SRELPR, HX, HY       
      COMMON /TBK12/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,N
C       
      IF (J .GT. N .OR. VAL .EQ. 0.E0) RETURN   
      IF(ISYM .EQ. 1) GO TO 10
      IF(J .LT. I) RETURN   
C       
   10 IF (ILEVEL .EQ. 1) WRITE(NOUT,20) J, VAL  
   20 FORMAT(20X,6HCOLUMN,I8,10H   VALUE =,E15.7) 
C       
      IER = 0     
      CALL SBSIJ(N,NELS,IA,JA,A,IWKSP,I,J,VAL,0,ILEVEL,NOUT,IER)    
      IF(IER .GT. 700) STOP 
C       
      RETURN      
      END 
      SUBROUTINE TEST2(EPSI)
C       
C ... TEST2 IS A PROGRAM DESIGNED TO TEST ITPACK 2C  METHODS ON     
C ... MATRICES ARISING FROM THE SYMMETRIC SEVEN POINT DISCRETIZATION
C ... OF THREE DIMENSIONAL ELLIPTIC PARTIAL DIFFERENTIAL EQUATIONS ON 
C ... A 3D RECTANGLE WITH A RECTANGULAR MESH.  ALL SEVEN METHODS FROM 
C ... ITPACK 2C ARE TESTED AND A SUMMARY IS PRINTED AT THE END.     
C       
C     THE EXACT SIZE OF THE TEST PROBLEMS CAN BE INCREASED OR       
C     DECREASED BY CHANGING ARRAY SIZE IN THE DIMENSION STATEMENTS  
C     AND THE VARIABLES LISTED BELOW UNDER SIZE OF TEST PROBLEM.    
C     ALSO, THE NUMBER OF TIMES THROUGH THE TEST LOOPS CAN BE REDUCED 
C     BY CHANGING  ITEST AND JTEST  AS FOLLOWS. 
C       
C          ITEST = 1 FOR SYMMETRIC STORAGE TEST 
C                = 2 FOR SYMMETRIC AND NONSYMMETRIC STORAGE TEST    
C          JTEST = 1 FOR NATURAL ORDERING TEST  
C                = 2 FOR NATURAL AND RED-BLACK ORDERING TEST
C       
C ... SYM7PT COMMON BLOCKS  
C       
      COMMON /TBK21/ AX, AY, AZ, BX, BY, BZ, SRELPR, HX, HY, HZ     
      COMMON /TBK22/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,NGRIDZ,N 
C       
C     ARRAY DIMENSIONING    
C       
      REAL    GRIDX(7), GRIDY(7), GRIDZ(7), RHS(216), U(216), A(1296),
     A        WKSP(1600), RPARM(12), DIGIT1(7), DIGIT2(7),
     B        TIM1(7), TIM2(7)
      INTEGER BCTYPE(6), IPARM(12), ITER(7), IWRK(7), IWORK(1296),  
     A        IA(217), JA(1296), IWKSP(648)     
C       
C       
C       
C ... INITIALIZE INTEGER CONSTANTS WHICH CONTROL OUTPUT AND DEFINE  
C ... ARRAY DIMENSION.  THEY ARE      
C       
C        SRELPR    -  MACHINE PRECISION 
C        NOUT    -  FORTRAN OUTPUT UNIT 
C        LEVEL   -  LEVEL OF OUTPUT FROM ITPACK 2C
C        IERAN    -  ERROR ANALYSIS SWITCH      
C        ITMAX   -  MAXIMUM NUMBER OF ITERATIONS ALLOWED  
C        ZETA    -  STOPPING CRITERION
C        NW      -  SIZE OF THE REAL ARRAY WKSP.
C       
C        ILEVEL  -  0/1 LEVEL OF OUTPUT FROM SYM7PT       
C        MXNEQ   -  MAXIMUM NUMBER OF EQUATIONS TO BE GENERATED (I.E. 
C                   THE MAXIMUM NUMBER OF INTERIOR AND NON-DIRICHLET
C                   BOUNDARY POINTS)  
C        NELMAX  -  MAXIMUM NUMBER OF NON-ZERO ENTRIES IN THE UPPER 
C                   TRIANGULAR PART OF THE RESULTING SYMMETRIC MATRIX 
C        NGRIDX  -  NUMBER OF HORIZONTAL MESH PLANES.     
C        NGRIDY  -  NUMBER OF VERTICAL MESH PLANES.       
C        NGRIDZ  -  NUMBER OF PERPENDICULAR MESH PLANES.  
C        NGRDXD  -  MAXIMUM NUMBER OF VERTICAL MESH PLANES INCLUDING THE
C                   VERTICAL BOUNDARY PLANES.   
C        NGRDYD  -  MAXIMUM NUMBER OF HORIZONTAL MESH PLANES INCLUDING
C                   THE HORIZONTAL BOUNDARY PLANES.       
C        NGRDZD  -  MAXIMUM NUMBER OF PERPENDICULAR MESH PLANES INCLUDIN
C                   THE PERPENDICULAR BOUNDARY PLANES.    
C       
      SRELPR = EPSI 
C       
C     SET ITPACK SWITCHES   
C       
      NOUT = 6    
      LEVEL = 1   
      IERAN = 0   
      ITMAX = 75  
      ZETA = 0.5E-5 
      NW = 1600   
C       
C       
C     SET NUMBER OF TIMES THROUGH LOOPS 
C       
      ITEST = 2   
      JTEST = 2   
C     SET SIZE OF TEST PROBLEM
C       
      ILEVEL = 0  
      MXNEQ = 216 
      NELMAX = 1296 
      NGRIDX = 7  
      NGRIDY = 7  
      NGRIDZ = 7  
C       
      NGRDXD = NGRIDX       
      NGRDYD = NGRIDY       
      NGRDZD = NGRIDZ       
      WRITE(NOUT,10)
   10 FORMAT(1H1 //15X,32HITPACK 2C  TEST PROGRAM -- TEST2
     A            /15X,23HTESTS ITERATIVE MODULES 
     B            /15X,41HUSES SEVEN POINT SYMMETRIC DISCRETIZATION  )
C       
C ... SET UP DEFAULT VALUES FOR BCTYPE AND INITIALIZE INFORMATION ARRAYS
C       
C ... BCTYPE DEFINES THE BOUNDARY CONDITIONS ON THE EDGES OF THE    
C ... RECTANGLE.  WHERE     
C        BCTYPE(I) = 0 IMPLIES THE ITH SIDE IS NEUMANN OR MIXED     
C                  = 1 IMPLIES THE ITH SIDE IS DIRICHLET  
C       
C          AND I = 1 IMPLIES THE EAST   SIDE DEFINED BY (BX, Y, Z)  
C                = 2 IMPLIES THE SOUTH  SIDE DEFINED BY ( X,AY, Z)  
C                = 3 IMPLIES THE WEST   SIDE DEFINED BY (AX, Y, Z)  
C                = 4 IMPLIES THE NORTH  SIDE DEFINED BY ( X,BY, Z)  
C                = 5 IMPLIES THE TOP    SIDE DEFINED BY ( X, Y,BZ)  
C                = 6 IMPLIES THE BOTTOM SIDE DEFINED BY ( X, Y,AZ)  
C       
      DO 20 I = 1,6 
         BCTYPE(I) = 1      
   20 CONTINUE    
C       
C ... DEFINE THE DISCRETIZATION MESH  
C       
C       AX      -  MINIMUM X VALUE ON THE RECTANGLE (WEST SIDE)     
C       BX      -  MAXIMUM X VALUE ON THE RECTANGLE (EAST SIDE)     
C       GRIDX   -  REAL ARRAY CONTAINING THE X-COORDINATE OF THE    
C                  HORIZONTAL MESH PLANES FROM WEST TO EAST.
C                  THESE ARE UNIFORM BUT THAT IS NOT REQUIRED.      
C       AY      -  MINIMUM Y VALUE ON THE RECTANGLE (SOUTH SIDE)    
C       BY      -  MAXIMUM Y VALUE ON THE RECTANGLE (NORTH SIDE)    
C       GRIDY   -  REAL ARRAY CONTAINING THE Y-COORDINATE OF THE    
C                  VERTICAL MESH PLANES FROM SOUTH TO NORTH.
C                  THESE ARE  UNIFORM BUT THAT IS NOT REQUIRED.     
C       AZ      -  MINIMUM Z VALUE ON THE RECTANGLE (BOTTOM SIDE)   
C       BZ      -  MAXIMUM Z VALUE ON THE RECTANGLE (TOP SIDE)      
C       GRIDZ   -  REAL ARRAY CONTAINING THE Z-COORDINATE OF THE    
C                  PERPENDICULAR MESH PLANES FROM BOTTOM TO TOP.    
C                  THESE ARE UNIFORM BUT THAT IS NOT REQUIRED.      
C       
      AX = 0.E0   
      BX = 1.E0   
      HX = (BX-AX)/  FLOAT(NGRIDX-1)  
      DO 40 J = 1,  NGRIDX  
         GRIDX(J) = AX + FLOAT(J-1)*HX
   40 CONTINUE    
      GRIDX(NGRIDX) = BX    
      BCTYPE(1) = 2 
C       
      AY = 0.E0   
      BY = 1.E0   
      HY = (BY-AY)/  FLOAT(NGRIDY-1)  
      DO 50 J = 1,  NGRIDY  
         GRIDY(J) = AY + FLOAT(J-1)*HY
   50 CONTINUE    
      GRIDY(NGRIDY) = BY    
      BCTYPE(4) = 2 
C       
      AZ = 0.E0   
      BZ = 1.E0   
      HZ = (BZ-AZ)/  FLOAT(NGRIDZ-1)  
      DO 60 J = 1,  NGRIDZ  
         GRIDZ(J) = AZ + FLOAT(J-1)*HZ
   60 CONTINUE    
      GRIDZ(NGRIDZ) = BZ    
      BCTYPE(5) = 2 
C       
C ... DISCRETIZE THE ELLIPTIC PDE     
C       
      DO 70 LOOP1=1,ITEST   
      ISYM = LOOP1 - 1      
      IF(LOOP1 .EQ. 2) WRITE(NOUT,63) 
   63 FORMAT(1H1///)
      CALL SYM7PT(GRIDX,NGRDXD,GRIDY,NGRDYD,GRIDZ,NGRDZD, 
     A            RHS,MXNEQ,IA,JA,A,NELMAX,IWORK) 
C       
C       
C ... SOLVE THE MATRIX PROBLEM
C       
C       
      DO 70 LOOP2=1,JTEST   
      NB = LOOP2 - 2
      IF(ISYM .EQ. 0) WRITE(NOUT,91)  
      IF(ISYM .EQ. 1) WRITE(NOUT,92)  
      IF(NB .EQ. (-1)) WRITE(NOUT,93) 
      IF(NB .EQ. 0)  WRITE(NOUT,94)   
C       
C        TEST JCG 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL JCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(1)=RPARM(9)   
         TIM2(1)=RPARM(10)  
         DIGIT1(1)=RPARM(11)
         DIGIT2(1)=RPARM(12)
         ITER(1)=IPARM(1)   
         IWRK(1)=IPARM(8)   
C       
C        TEST JSI 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL JSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(2)=RPARM(9)   
         TIM2(2)=RPARM(10)  
         DIGIT1(2)=RPARM(11)
         DIGIT2(2)=RPARM(12)
         ITER(2)=IPARM(1)   
         IWRK(2)=IPARM(8)   
C       
C        TEST SOR 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL SOR (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(3)=RPARM(9)   
         TIM2(3)=RPARM(10)  
         DIGIT1(3)=RPARM(11)
         DIGIT2(3)=RPARM(12)
         ITER(3)=IPARM(1)   
         IWRK(3)=IPARM(8)   
C       
C        TEST SSORCG
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL SSORCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)
         TIM1(4)=RPARM(9)   
         TIM2(4)=RPARM(10)  
         DIGIT1(4)=RPARM(11)
         DIGIT2(4)=RPARM(12)
         ITER(4)=IPARM(1)   
         IWRK(4)=IPARM(8)   
C       
C        TEST SSORSI
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(9)=NB
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL SSORSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)
         TIM1(5)=RPARM(9)   
         TIM2(5)=RPARM(10)  
         DIGIT1(5)=RPARM(11)
         DIGIT2(5)=RPARM(12)
         ITER(5)=IPARM(1)   
         IWRK(5)=IPARM(8)   
C       
C        TEST RSCG
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL RSCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  
         TIM1(6)=RPARM(9)   
         TIM2(6)=RPARM(10)  
         DIGIT1(6)=RPARM(11)
         DIGIT2(6)=RPARM(12)
         ITER(6)=IPARM(1)   
         IWRK(6)=IPARM(8)   
C       
C        TEST RSSI
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL RSSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  
         TIM1(7)=RPARM(9)   
         TIM2(7)=RPARM(10)  
         DIGIT1(7)=RPARM(11)
         DIGIT2(7)=RPARM(12)
         ITER(7)=IPARM(1)   
         IWRK(7)=IPARM(8)   
C       
C     TIMING ANALYSIS       
C       
         CALL TIME1 (N,IA,JA,A,WKSP,WKSP(N+1),ITER,TIM1,TIM2,       
     A                DIGIT1,DIGIT2,IWRK,NOUT)  
   70 CONTINUE    
C       
   91 FORMAT(//15X,29HSYMMETRIC SPARSE STORAGE USED )     
   92 FORMAT(//15X,32HNONSYMMETRIC SPARSE STORAGE USED )  
   93 FORMAT(15X,21HNATURAL ORDERING USED )     
   94 FORMAT(15X,23HRED-BLACK ORDERING USED )   
      RETURN      
      END 
      SUBROUTINE PDE2(X,Y,Z,CVALUS)   
C       
C ... THIS SUBROUTINE IS A USER SUPPLIED SUBROUTINE TO SPECIFY THE  
C ... SELF-ADJOINT ELLIPTIC PDE FOR SYM7PT IN THE FOLLOWING FORM    
C       
C        (CVALUS(1)*UX)X + (CVALUS(3)*UY)Y + (CVALUS(6)*UZ)Z
C                                          + CVALUS(10)*U = CVALUS(11)
C       
C     NOTE:  CVALUS(I), FOR I = 2, 4, 5, 7, 8, AND 9 ARE NOT USED.  
C       
         REAL CVALUS(11)    
C       
         CVALUS( 1) = 1.E0  
         CVALUS( 2) = 0.E0  
         CVALUS( 3) = 2.E0  
         CVALUS( 4) = 0.E0  
         CVALUS( 5) = 0.E0  
         CVALUS( 6) = 1.E0  
         CVALUS( 7) = 0.E0  
         CVALUS( 8) = 0.E0  
         CVALUS( 9) = 0.E0  
         CVALUS(10) = 0.E0  
         CVALUS(11) = 0.E0  
C       
         RETURN   
      END 
      REAL FUNCTION BCOND2(ISIDE,X,Y,Z,BVALUS)  
C       
C ... THIS REAL FUNCTION IS A USER SUPPLIED FUNCTION TO SPECIFY THE 
C ... BOUNDARY CONDITIONS OF THE ELLIPTIC PDE DEPENDING ON ISIDE, X,
C ... Y, AND Z.   
C       
C        IF ISIDE = 1, THEN X = BX (EAST SIDE)  
C                 = 2, THEN Y = AY (SOUTH SIDE) 
C                 = 3, THEN X = AX (WEST SIDE)  
C                 = 4, THEN Y = BY (NORTH SIDE) 
C                 = 5, THEN Z = BZ (TOP SIDE)   
C                 = 6, THEN Z = AZ (BOTTOM SIDE)
C       
C ... THE BVALUS ARRAY IS DEFINED AS FOLLOWS    
C       
C        BVALUS(1)*U + BVALUS(2)*UX + BVALUS(3)*UY + BVALUS(4)*UZ   
C                                                  = BVALUS(5)      
C       
C        NOTE:  BCOND2 IS SET TO BVALUS(5) BEFORE RETURNING.
C       
         REAL BVALUS(5)     
C       
         GOTO(10,20,30,40,50,60),ISIDE
C       
   10    BVALUS(2) = 1.E0   
         BVALUS(1) = 0.E0   
         BVALUS(5) = Y*Z*(1.E0 + Y*Z) 
         BVALUS(3) = 0.E0   
         BVALUS(4) = 0.E0   
         GO TO 999
C       
   20    BVALUS(1) = 1.E0   
         BVALUS(5) = 1.E0   
         BVALUS(2) = 0.E0   
         BVALUS(3) = 0.E0   
         BVALUS(4) = 0.E0   
         GO TO 999
C       
   30    BVALUS(1) = 1.E0   
         BVALUS(5) = 1.E0   
         BVALUS(2) = 0.E0   
         BVALUS(3) = 0.E0   
         BVALUS(4) = 0.E0   
         GO TO 999
C       
   40    BVALUS(3) = 1.E0   
         BVALUS(1) = 0.E0   
         BVALUS(2) = 0.E0   
         BVALUS(5) = X*Z * (1.E0+ X*Z)
         BVALUS(4) = 0.E0   
         GO TO 999
C       
   50    BVALUS(4) = 1.E0   
         BVALUS(1) = 0.E0   
         BVALUS(2) = 0.E0   
         BVALUS(3) = 0.E0   
         BVALUS(5) = X*Y * (1.E0+ X*Y)
         GO TO 999
C       
   60    BVALUS(1) = 1.E0   
         BVALUS(5) = 1.E0   
         BVALUS(2) = 0.E0   
         BVALUS(3) = 0.E0   
         BVALUS(4) = 0.E0   
C       
  999    CONTINUE 
         BCOND2 = BVALUS(5) 
         RETURN   
      END 
      SUBROUTINE SYM7PT (GRIDX,NGRDXD,GRIDY,NGRDYD,GRIDZ,NGRDZD,RHS,
     A                   MXNEQ,IA,JA,A,NELS,IWKSP)
C       
      INTEGER IA(1), JA(NELS), BCTYPE(6), IWKSP(NELS)     
      REAL    GRIDX(NGRDXD),GRIDY(NGRDYD),GRIDZ(NGRDZD),  
     A        RHS(MXNEQ),A(NELS)      
C       
C ...  SYM7PT COMMON BLOCKS 
C       
      COMMON /TBK21/ AX, AY, AZ, BX, BY, BZ, SRELPR, HX, HY, HZ     
      COMMON /TBK22/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,NGRIDZ,N 
C       
C       
         IF (NGRIDX.LT.3 .OR. NGRIDY.LT.3 .OR. NGRIDZ.LT.3)    GO TO 120
C       
C     DETERMINE RANGE OF UNKNOWN GRID POINTS    
C       
      IX1 = 1     
      IX2 = NGRIDX
      JY1 = 1     
      JY2 = NGRIDY
      KZ1 = 1     
      KZ2 = NGRIDZ
         IF (BCTYPE(1) .EQ. 1) IX2 = NGRIDX - 1 
         IF (BCTYPE(2) .EQ. 1) JY1 = 2
         IF (BCTYPE(3) .EQ. 1) IX1 = 2
         IF (BCTYPE(4) .EQ. 1) JY2 = NGRIDY - 1 
         IF (BCTYPE(6) .EQ. 1) KZ1 = 2
         IF (BCTYPE(5) .EQ. 1) KZ2 = NGRIDZ - 1 
      LNGTHX = IX2 - IX1 + 1
      LNGTHY = JY2 - JY1 + 1
      N = LNGTHX * LNGTHY * (KZ2 - KZ1 + 1)     
         IF (N .GT. MXNEQ) GO TO 260  
C       
C     OUTPUT INITIAL GRID INFORMATION 
C       
         IF (ILEVEL .EQ. 0) GO TO 70  
      WRITE(NOUT,20) AX,BX,AY,BY,AZ,BZ
   20 FORMAT(//10X,24HFINITE DIFFERENCE MODULE  ,6H ---- ,
     A       21HSYMMETRIC SEVEN POINT //10X,15HDOMAIN = BOX  (,     
     B       E11.4,1H,,E11.4,5H) X (,E11.4,1H,,E11.4,5H) X (,E11.4,1H,, 
     C       E11.4,1H)  )   
C       
      WRITE(NOUT,30)
   30 FORMAT(/10X,28HCOEFFICIENTS OF X-MESH LINES    )    
      WRITE(NOUT,40) (GRIDX(I),I=1,NGRIDX)      
   40 FORMAT(/8X,8(2X,E11.4) )
      WRITE(NOUT,50)
   50 FORMAT(/10X,28HCOEFFICIENTS OF Y-MESH LINES  )      
      WRITE(NOUT,40) (GRIDY(I),I=1,NGRIDY)      
      WRITE(NOUT,55)
   55 FORMAT(/10X,28HCOEFFICIENTS OF Z-MESH LINES  )      
      WRITE(NOUT,40) (GRIDZ(I),I=1,NGRIDZ)      
      WRITE(NOUT,60) (BCTYPE(I), I=1,6) 
   60 FORMAT(/10X,46HBOUNDARY CONDITIONS ON PIECES 1,2,3,4,5,6 ARE   ,
     A       5(1X,I1,1H,),1X,I1,1H. //) 
C       
C     GENERATE EQUATIONS ONE MESH POINT AT A TIME 
C       
   70 CONTINUE    
      CALL SBINI(N,NELS,IA,JA,A,IWKSP)
C       
      DO 100 IJ = 1,N       
         IXX = MOD(IJ-1,LNGTHX) + 1   
         JYY = MOD((IJ-IXX)/LNGTHX,LNGTHY) + 1  
         KZZ = (IJ-IXX-(JYY-1)*LNGTHX)/(LNGTHX*LNGTHY) + 1
            IXX = IXX + IX1 - 1       
            JYY = JYY + JY1 - 1       
            KZZ = KZZ + KZ1 - 1       
C       
         HU = 0.E0
         HD = 0.E0
         HW = 0.E0
         HE = 0.E0
         HS = 0.E0
         HN = 0.E0
         PZ = GRIDZ(KZZ)    
            IF (KZZ .NE. 1) HD = PZ - GRIDZ(KZZ-1)
            IF (KZZ .NE. NGRIDZ) HU = GRIDZ(KZZ+1) - PZ   
         PY = GRIDY(JYY)    
            IF (JYY .NE. 1) HS = PY - GRIDY(JYY-1)
            IF (JYY .NE. NGRIDY) HN = GRIDY(JYY+1) - PY   
         PX = GRIDX(IXX)    
            IF (IXX .NE. 1) HW = PX - GRIDX(IXX-1)
            IF (IXX .NE. NGRIDX) HE = GRIDX(IXX+1) - PX   
C       
         IJIN = IJ
         CALL PNT3D(PX,PY,PZ,IJIN,IXX,JYY,KZZ,LNGTHX,LNGTHY,
     A              HN,HS,HW,HE,HU,HD,B,NELS,IA,JA,A,IWKSP) 
C       
         RHS(IJ) = B
  100 CONTINUE    
      CALL SBEND(N,NELS,IA,JA,A,IWKSP)
C       
C     NORMAL EXIT 
C       
      IF (ILEVEL .EQ. 0) RETURN       
C       
      WRITE(NOUT,110)       
  110 FORMAT(/10X,30HSYM7PT COMPLETED SUCCESSFULLY.  )    
      NU = IA(N+1) - 1      
      NUU = N + 1 + 2*NU    
      WRITE(NOUT,115) N,NU,NUU
  115 FORMAT( 10X,38HSPARSE MATRIX REPRESENTATION FINISHED. 
     A       /15X,25HNO. OF EQUATIONS        =,I8 
     B       /15X,25HNO. OF NON-ZEROES       =,I8 
     C       /15X,25HTOTAL MATRIX STORAGE    =,I8  /)     
C       
      RETURN      
C       
C     ERROR EXITS 
C       
  120 CONTINUE    
         IF (NGRIDX .LT. 3)  WRITE(NOUT,160)    
  160 FORMAT(//20X,30HSYM7PT ERROR -- NGRIDX .LT. 3     ) 
         IF (NGRIDY .LT. 3)  WRITE(NOUT,170)    
  170 FORMAT(//20X,30HSYM7PT ERROR -- NGRIDY .LT. 3     ) 
         IF (NGRIDZ .LT. 3)  WRITE(NOUT,180)    
  180 FORMAT(//20X,30HSYM7PT ERROR -- NGRIDZ .LT. 3     ) 
C       
      STOP
C       
  260 WRITE(NOUT,270) N,MXNEQ 
  270 FORMAT(/10X,17HN .GT. MXNEQ, N =,I10,8H MXNEQ =,I10)
      STOP
C       
      END 
      SUBROUTINE PNT3D(PCX,PCY,PCZ,IJ,IX,JY,KZ,LNGTHX,LNGTHY,       
     A                 HN,HS,HW,HE,HU,HD,B,NELS,IA,JA,A,IWKSP)      
C       
      INTEGER IA(1), JA(NELS), BCTYPE(6), IWKSP(NELS)     
      REAL    CVALUS(11), BVALUS(5), A(NELS)    
C       
C ...  SYM7PT COMMON BLOCKS 
C       
      COMMON /TBK21/ AX, AY, AZ, BX, BY, BZ, SRELPR, HX, HY, HZ     
      COMMON /TBK22/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,NGRIDZ,N 
C       
C       
C     INITIALIZE COEFFICIENTS 
C       
      CUP = 0.E0  
      CLOWER = 0.E0 
      CEAST = 0.E0
      CWEST = 0.E0
      CNORTH = 0.E0 
      CSOUTH = 0.E0 
      CALL PDE2(PCX,PCY,PCZ,CVALUS)   
      CENTER = -CVALUS(10)*(HN+HS)*(HE+HW)*(HU+HD)/8.E0   
      CRHS  = -CVALUS(11)*(HN+HS)*(HE+HW)*(HU+HD)/8.E0    
C       
C     SET EAST COEFFICIENT  
C       
      CALL PDE2 (PCX+.5E0*HE,PCY,PCZ,CVALUS)    
         IF (IX .GE. NGRIDX-1) TEMP = BCOND2(1,BX,PCY,PCZ,BVALUS)   
         IF (IX .EQ. NGRIDX) GO TO 20 
         IF ((IX+1 .EQ. NGRIDX) .AND. (BCTYPE(1) .EQ. 1)) GO TO 10  
C       
C     NORMAL EAST POINT     
C       
      CEAST = -CVALUS(1)*(HN+HS)*(HU+HD)/(4.E0*HE)
      CENTER = CENTER - CEAST 
            GO TO 30
C       
C     EAST POINT IS A DIRICHLET POINT 
C       
   10 TEMP = CVALUS(1)*(HN+HS)*(HU+HD)/(4.E0*HE)
      CRHS = CRHS + BVALUS(5)*TEMP/BVALUS(1)    
      CENTER = CENTER + TEMP
            GO TO 30
C       
C     CENTER POINT LIES ON THE EAST BOUNDARY WHICH IS MIXED 
C       
   20 TEMP = CVALUS(1)*(HN+HS)*(HU+HD)/(4.E0*BVALUS(2))   
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(5)*TEMP    
C       
C     SET WEST COEFFICIENT  
C       
   30 CALL PDE2(PCX - .5E0*HW,PCY,PCZ,CVALUS)   
         IF (IX .LE. 2) TEMP = BCOND2(3,AX,PCY,PCZ,BVALUS)
         IF (IX .EQ. 1) GO TO 50      
         IF (IX .EQ. 2 .AND. BCTYPE(3).EQ.1) GO TO 40     
C       
C     NORMAL WEST POINT     
C       
      CWEST = -CVALUS(1)*(HN+HS)*(HU+HD)/(4.E0*HW)
      CENTER = CENTER - CWEST 
            GO TO 60
C       
C     WEST POINT IS DIRICHLET 
C       
   40 TEMP = CVALUS(1)*(HN+HS)*(HU+HD)/(4.E0*HW)
      CRHS = CRHS + BVALUS(5)*TEMP/BVALUS(1)    
      CENTER = CENTER + TEMP
            GO TO 60
C       
C     CENTER POINT LIES ON WEST BOUNDARY WHICH IS MIXED.  
C       
   50 TEMP = -CVALUS(1)*(HN+HS)*(HU+HD)/(4.E0*BVALUS(2))  
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(5)*TEMP    
C       
C     SET NORTH COEFFICIENTS
C       
   60 CALL PDE2(PCX,PCY+.5E0*HN,PCZ,CVALUS)     
         IF (JY .GE. NGRIDY-1) TEMP = BCOND2(4,PCX,BY,PCZ,BVALUS)   
         IF (JY .EQ. NGRIDY) GO TO 80 
         IF ((JY.EQ.NGRIDY-1) .AND. BCTYPE(4).EQ.1) GO TO 70
C       
C     NORMAL NORTH POINT    
C       
      CNORTH = -CVALUS(3) * (HE + HW) * (HU + HD)/(4.E0*HN) 
      CENTER = CENTER - CNORTH
            GO TO 90
C       
C     NORTH POINT IS DIRICHLET
C       
   70 TEMP = CVALUS(3) * (HE + HW) * (HU + HD) / (4.E0*HN)
      CRHS = CRHS + BVALUS(5)*TEMP / BVALUS(1)  
      CENTER = CENTER + TEMP
            GO TO 90
C       
C     CENTER POINT LIES ON NORTHERN MIXED BOUNDARY
C       
   80 TEMP = CVALUS(3)*(HE + HW) * (HU + HD)/(4.E0*BVALUS(3))       
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(5) * TEMP  
C       
C     SET SOUTH COEFFICIENTS
C       
   90 CALL PDE2(PCX,PCY-.5E0*HS,PCZ,CVALUS)     
         IF (JY .LE. 2) TEMP = BCOND2(2,PCX,AY,PCZ,BVALUS)
         IF (JY .EQ. 1) GO TO 110     
         IF (JY .EQ. 2 .AND. BCTYPE(2) .EQ. 1) GO TO 100  
C       
C     NORMAL SOUTH POINT    
C       
      CSOUTH = - CVALUS(3)*(HE+HW) * (HU + HD)/(4.E0*HS)  
      CENTER = CENTER - CSOUTH
            GO TO 120       
C       
C     SOUTH POINT IS DIRICHLET
C       
  100 TEMP = CVALUS(3) * (HE + HW) * (HU + HD) / (4.E0 * HS)
      CRHS = CRHS + BVALUS(5)*TEMP/BVALUS(1)    
      CENTER = CENTER + TEMP
            GO TO 120       
C       
C     CENTER POINT LIES ON SOUTHERN MIXED BOUNDARY
C       
  110 TEMP = -CVALUS(3)*(HE+HW) * (HU + HD)/(4.E0*BVALUS(3))
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(5)*TEMP    
C       
C     SET UPPER COEFFICIENTS
C       
  120 CALL PDE2(PCX,PCY,PCZ+.5E0*HU,CVALUS)     
         IF (KZ .GE. NGRIDZ-1) TEMP = BCOND2(5,PCX,PCY,BZ,BVALUS)   
         IF (KZ .EQ. NGRIDZ) GO TO 140
         IF ((KZ.EQ.NGRIDZ-1) .AND. BCTYPE(5).EQ.1) GO TO 130       
C       
C     NORMAL UPPER POINT    
C       
      CUP = -CVALUS(6) * (HE + HW) * (HN + HS)/(4.E0*HU)  
      CENTER = CENTER - CUP 
            GO TO 150       
C       
C     UPPER POINT IS DIRICHLET
C       
  130 TEMP = CVALUS(6) * (HE + HW) * (HN + HS) / (4.E0*HU)
      CRHS = CRHS + BVALUS(5)*TEMP / BVALUS(1)  
      CENTER = CENTER + TEMP
            GO TO 150       
C       
C     CENTER POINT LIES ON UPPER  MIXED BOUNDARY
C       
  140 TEMP = CVALUS(6)*(HE + HW) * (HN + HS)/(4.E0*BVALUS(4))       
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(5) * TEMP  
C       
C     SET LOWER COEFFICIENTS
C       
  150 CALL PDE2(PCX,PCY,PCZ-.5E0*HD,CVALUS)     
         IF (KZ .LE. 2) TEMP = BCOND2(6,PCX,PCY,AZ,BVALUS)
         IF (KZ .EQ. 1) GO TO 170     
         IF (KZ .EQ. 2 .AND. BCTYPE(6) .EQ. 1) GO TO 160  
C       
C     NORMAL LOWER POINT    
C       
      CLOWER = - CVALUS(6)*(HE+HW) * (HN + HS)/(4.E0*HD)  
      CENTER = CENTER - CLOWER
            GO TO 300       
C       
C     LOWER POINT IS DIRICHLET
C       
  160 TEMP = CVALUS(6) * (HE + HW) * (HN + HS) / (4.E0*HD)
      CRHS = CRHS + BVALUS(5)*TEMP/BVALUS(1)    
      CENTER = CENTER + TEMP
            GO TO 300       
C       
C     CENTER POINT LIES ON LOWER MIXED BOUNDARY 
C       
  170 TEMP = -CVALUS(6)*(HE+HW) * (HN + HS)/(4.E0*BVALUS(4))
      CENTER = CENTER + BVALUS(1)*TEMP
      CRHS = CRHS + BVALUS(5)*TEMP    
C       
C     COEFFICIENT GENERATION IS COMPLETED.      
C     NOW SET B (RHS), IA, JA, AND A (MATRIX REPRESENTATION)
C       
  300 B = CRHS    
C       
      IF (ILEVEL .EQ. 1) WRITE(NOUT,310) IJ, CRHS 
  310 FORMAT(/10X,18HSYM7PT -- EQUATION,I8 /20X,
     A       24HRIGHT HAND SIDE =         ,E15.7) 
C       
      LXY = LNGTHX * LNGTHY 
      CALL STVAL2(NELS,IA,JA,A,IJ,IJ       ,CENTER,IWKSP) 
      CALL STVAL2(NELS,IA,JA,A,IJ,IJ+1     ,CEAST ,IWKSP) 
      CALL STVAL2(NELS,IA,JA,A,IJ,IJ-1     ,CWEST ,IWKSP) 
      CALL STVAL2(NELS,IA,JA,A,IJ,IJ-LNGTHX,CSOUTH,IWKSP) 
      CALL STVAL2(NELS,IA,JA,A,IJ,IJ+LNGTHX,CNORTH,IWKSP) 
      CALL STVAL2(NELS,IA,JA,A,IJ,IJ-LXY   ,CLOWER,IWKSP) 
      CALL STVAL2(NELS,IA,JA,A,IJ,IJ+LXY   ,CUP   ,IWKSP) 
C       
      RETURN      
      END 
      SUBROUTINE STVAL2(NELS,IA,JA,A,I,J,VAL,IWKSP)       
         REAL A(NELS)       
         INTEGER IA(1),JA(NELS),IWKSP(NELS),BCTYPE(6)     
      COMMON /TBK21/ AX, AY, AZ, BX, BY, BZ, SRELPR, HX, HY, HZ     
      COMMON /TBK22/ BCTYPE,ILEVEL,ISYM,NOUT,NGRIDX,NGRIDY,NGRIDZ,N 
C       
      IF (J .GT. N .OR. VAL .EQ. 0.E0) RETURN   
      IF(ISYM .EQ. 1) GO TO 5 
      IF(J .LT. I) RETURN   
C       
    5 IF (ILEVEL .EQ. 1) WRITE(NOUT,20) J, VAL  
   20 FORMAT(20X,6HCOLUMN,I8,10H   VALUE =,E15.7) 
C       
      IER = 0     
      CALL SBSIJ(N,NELS,IA,JA,A,IWKSP,I,J,VAL,0,ILEVEL,NOUT,IER)    
C       
         IF (IER .GT. 700) STOP       
C       
      RETURN      
      END 
      SUBROUTINE TEST3(EPSI)
C       
C     TEST3 EXERCISES ITPACK OVER SPARSE LINEAR SYSTEMS   
C     OF ORDER AT MOST 40 WITH RANDOMLY GENERATED STRUCTURE.
C     (FOR DETAILS SEE SECTION 2 OF CENTER FOR NUMERICAL ANALYSIS   
C     REPORT CNA-171 BY DAVID R. KINCAID:       
C     ACCELERATION PARAMETERS FOR A SYMMETRIC SUCCESSIVE OVERRELAXATION 
C     CONJUGATE GRADIENT METHOD FOR NONSYMMETRIC SYSTEMS, 
C     CENTER FOR NUMERICAL ANALYSIS, UNIVERSITY OF TEXAS, AUSTIN, TX, 
C     78712 - ALSO IN ADVANCES IN COMPUTER METHODS FOR PARTIAL      
C     DIFFERENTIAL EQUATIONS, IV, IMACS, NEW BRUNSWICK, NJ, 1981.)  
C       
C*****************************************************************  
C       
C     THE EXACT SIZE OF THE TEST PROBLEMS CAN BE INCREASED OR       
C     DECREASED BY CHANGING THE ARRAY SIZE IN DIMENSION STATEMENTS  
C     AND THE VARIABLES LISTED BELOW UNDER SIZE OF TEST PROBLEM.    
C       
C     ARRAY DIMENSIONING    
C       
      INTEGER  IA(41),JA(1600),IROW(40),IWKSP(1600),P(40),IP(40)    
      REAL     A(1600),RHS(40),ROW(40),WKSP(500)
C       
C     TEST3 COMMON BLOCK    
C       
      COMMON /TBK31/ SRELPR, ZETA     
      COMMON /TBK32/ IDEBUG, IERAN, ILEVEL, IORD, IPCT, IPLT, ISEED,
     A                 ISYM, ITMAX, LARGE, LEVEL, MAXNZ, MAXN, NOUT,
     B                 NBLACK, NB, NRBN, NRED, NW, NZBLK, NZRED, N  
C       
      SRELPR = EPSI 
C       
C     SET ITPACK 2C SWITCHS 
C       
C     ZETA   -  STOPPING TEST CRITERION 
C     LEVEL  -  LEVEL OF OUTPUT FROM ITPACK 2C  
C     ITMAX  -  MAXIMUM NUMBER OF ITERATIONS ALLOWED      
C     IERAN  -  ERROR ANALYSIS SWITCH 
C     NOUT   - OUTPUT UNIT NUMBER     
C       
C       
      ZETA = 1.E-5
      LEVEL = 1   
      ITMAX = 50  
      IERAN = 0   
      NOUT = 6    
C       
C     PROBLEM SIZE
C       
C     MAXNZ  -  MAXIMUM NUMBER OF NONZEROS      
C     MAXN   -  MAXIMUM SIZE OF SYSTEM
C     NW     -  SIZE OF REAL ARRAY WKSP 
C     N      -  SIZE OF LINEAR SYSTEM 
C     IPCT   -  PERCENTAGE OF OFFDIAGONAL NONZEROS
C       
      MAXNZ = 1600
      MAXN  =  40 
      NW = 500    
      N = 40      
      IPCT = 20   
C       
C     SWITCHES FOR TESTING PROGRAM    
C       
C     ILEVEL  -  0/1 SWITCH FOR PRINT FROM TESTING PROGRAM
C     IDEBUG  -  0/1 SWITCH FOR DEBUG PRINTING FROM TESTING PROGRAM 
C     IPLT    -  0/1 SWITCH FOR PRINTER PLOTTING OF NONZERO STRUCTURE 
C     LARGE   -  0/1 SWITCH FOR TESTING ITPACK ROUTINE SBELM
C     ISEED   -  RANDUM NUMBER GENERATOR SEED   
C       
      ILEVEL = 0  
      IDEBUG = 0  
      IPLT = 1    
      LARGE = 1   
      ISEED = 256 
C       
C     OTHER VARIABLES THAT CONTROL NATURE OF TEST 
C       
C     NRED  -  NUMBER OF RED EQUATIONS
C     NBLACK - NUMBER OF BLACK EQUATIONS
C     NZRED  - NUMBER OF RED OFFDIAGONAL NONZERO ENTRIES  
C     NZBLK  - NUMBER OF BLACK OFFDIAGONAL NONZERO ENTRIES
C     NRNB   - SIZE OF RED/BLACK BLOCK (NRED*NBLACK)      
C     ISYM   - 0/1 SWITCH FOR SYMMETRIC/NONSYMMETRIC STORAGE
C     NB     - ORDER OF BLACK SUBSYSTEM 
C     IORD   - 1/2 SWITCH FOR NATURAL/RED-BLACK ORDERING  
C       
C       
      WRITE(NOUT,6000)      
 6000 FORMAT(1H1 //15X,32HITPACK 2C  TEST PROGRAM -- TEST3  // )    
C       
C     GENERATE RANDOM NUMBERS FOR N, NRED, NZRED, P       
C       
      CALL SETPER ( P, IP ) 
C       
         WRITE(NOUT,7010) N,IPCT,NRED,NZRED     
 7010    FORMAT(//2X,70(1H*)
     A           /5X,33HORDER OF SYSTEM                 =, I5       
     A           /5X,33HPERCENTAGE OFFDIAGONAL NONZEROS =, I5       
     B           /5X,33HINITIAL NUMBER OF RED POINTS    =, I5       
     C           /5X,33HNUMBER OF RED NONZERO ENTRIES   =, I5 /)    
C       
C ...... LOOP FOR SYMMETRIC AND NONSYMMETRIC SYSTEMS OF SAME SIZE   
C       
      DO 100 LOOP = 1,2     
      ISYM = LOOP -1
C       
C ...... GENERATE RANDOM SPARSE SYSTEM
C       
      CALL SETSYS(IA,JA,A,RHS,P,IP,ROW,IROW,IWKSP)
C       
      IF(ISYM .EQ. 0) WRITE(NOUT,41)  
      IF(ISYM .NE. 0) WRITE(NOUT,42)  
      WRITE(NOUT,45)
41    FORMAT(1H0,14X,22HSYMMETRIC SYSTEM USING) 
42    FORMAT(1H0,14X,25HNONSYMMETRIC SYSTEM USING)
45    FORMAT(15X,16HNATURAL ORDERING) 
46    FORMAT(15X,18HRED-BLACK ORDERING) 
C       
C ...... TEST ITPACK 2C ROUTINES: NATURAL ORDERING
C       
      IORD = 1    
      CALL TSTITP(IA,JA,A,RHS,ROW,IWKSP,WKSP)   
C       
C ...... TEST RED-BLACK SYSTEM
C       
      CALL TSTPRB(IA,JA,A,RHS,P,IP,IROW,IWKSP)  
C       
      IF(ISYM .EQ. 0) WRITE(NOUT,41)  
      IF(ISYM .NE. 0) WRITE(NOUT,42)  
      WRITE(NOUT,46)
C       
C ...... TEST ITPACK 2C ROUTINES: RED-BLACK ORDERING      
C       
      IORD = 2    
      CALL TSTITP(IA,JA,A,RHS,ROW,IWKSP,WKSP)   
C       
  100 CONTINUE    
C       
      RETURN      
      END 
      SUBROUTINE SETPER ( P, IP )     
C       
C       
C       GENERATE RANDOM NUMBERS FOR   
C           ORDER OF SYSTEM, RED POINTS, NONZERO POINTS, PERMUTATION
C       
C       
      INTEGER P(1), IP(1)   
C       
      COMMON /TBK31/ SRELPR, ZETA     
      COMMON /TBK32/ IDEBUG, IERAN, ILEVEL, IORD, IPCT, IPLT, ISEED,
     A                 ISYM, ITMAX, LARGE, LEVEL, MAXNZ, MAXN, NOUT,
     B                 NBLACK, NB, NRBN, NRED, NW, NZBLK, NZRED, N  
C       
      NRED = N/2  
         NBLACK = N - NRED  
      NRNB = NRED*NBLACK    
C       
C       COMPUTE NUMBER OF NONZEROS RED ENTRIES  
C       
      NZRED = IFIX(FLOAT(NRNB)*FLOAT(IPCT)*1.0E-2)
      NZBLK = NZRED 
C       
C       GENERATE RANDOM PERMUTATION   
C       
         DO 10 I = 1, N     
            P(I) = I
      IP(I) = IRAND ( 1, N , ISEED )  
  10     CONTINUE 
C       
      RETURN      
      END 
      SUBROUTINE SETSYS (IA,JA,A,RHS,P,IP,ROW,IROW,IWKSP) 
C       
C     GENERATE SPARSE SYSTEM WITH RANDOM STRUCTURE
C       
      INTEGER  IA(1),JA(1),P(1),IP(1),IWKSP(1),IROW(1),PI,PJ
      REAL    A(1),RHS(1),ROW(1)      
C       
      COMMON /TBK31/ SRELPR, ZETA     
      COMMON /TBK32/ IDEBUG, IERAN, ILEVEL, IORD, IPCT, IPLT, ISEED,
     A                 ISYM, ITMAX, LARGE, LEVEL, MAXNZ, MAXN, NOUT,
     B                 NBLACK, NB, NRBN, NRED, NW, NZBLK, NZRED, N  
C       
      DO 2 I=1,N  
    2 ROW(I) = FLOAT(P(I))  
      CALL QSORT(N,IP,ROW,IER)
      DO 3 I=1,N  
    3 P(I) = IFIX(ROW(I))   
      IF(IER .EQ. 0) GO TO 5
C       
      WRITE(NOUT, 20) IER   
   20 FORMAT(/2X,18HQSORT ERROR, IER =, I5 )    
      STOP
C       
C       
C       
    5 CALL SBINI ( N, MAXNZ, IA, JA, A, IWKSP ) 
C       
C       ZERO OUT THINGS , SET RHS TO 1.0
C       
      DO 15 K = 1, N
      RHS(K) = 1.E0 
         ROW(K) = 0.E0      
   15 CONTINUE    
C       
      IF ( ISYM .NE. 0 ) GO TO  51    
C       
C ******************* SYMMETRIC STORAGE CASE ********************** 
      WRITE(NOUT,6001)      
 6001 FORMAT(/2X,20(1H+),24HSYMMETRIC STORAGE FORMAT ,20(1H+) )     
C       
C       STORE SYMMETRIC SYSTEM  OFF-DIAGONAL ELEMENTS     
C       
         DO 50 K = 1, NZRED 
 501      I = IRAND ( 1, NRED, ISEED )
         J = IRAND ( NRED+1, N, ISEED ) 
            PI = MIN0 ( P(I), P(J) )  
            PJ = MAX0 ( P(I), P(J) )  
            VAL = - RANDOM(ISEED)     
      IF(IDEBUG .NE. 0) WRITE(NOUT,6002) PI,PJ,VAL
 6002 FORMAT(/2X,17H NONZERO ENTRY ( ,I10,3H , ,I10,9H ) SET = ,G15.8)
            CALL SBSIJ ( N, MAXNZ, IA, JA, A, IWKSP, PI, PJ, VAL, -1, 
     A                  ILEVEL, NOUT, IER)      
            IF ( IER .EQ. 700 ) GO TO 501       
            IF ( IER .GT. 700 ) STOP  
            ROW(PI) = ROW(PI) - VAL   
            ROW(PJ) = ROW(PJ) - VAL   
  50     CONTINUE 
         GO TO 57 
C       
C ******************* NONSYMMETRIC STORAGE CASE ******************  
C       
  51  WRITE(NOUT,6003)      
 6003 FORMAT(/2X,20(1H+),27HNONSYMMETRIC STORAGE FORMAT ,20(1H+) )  
C       
C       STORE NONSYMMETRIC SYSTEM  OFF-DIAGONAL ELEMENTS  
C       
         DO 505 K = 1, NZRED
 506     I = IRAND ( 1, NRED, ISEED)  
         J = IRAND ( NRED+1, N, ISEED ) 
            PI = P(I)       
            PJ = P(J)       
            VAL = - RANDOM(ISEED)     
      IF(IDEBUG .NE. 0) WRITE(NOUT,6002) PI,PJ,VAL
            CALL SBSIJ ( N, MAXNZ, IA, JA, A, IWKSP, PI, PJ, VAL, -1, 
     A                  ILEVEL, NOUT, IER)      
            IF ( IER .EQ. 700 ) GO TO 506       
            IF ( IER .GT. 700 ) STOP  
            ROW(PI) = ROW(PI) - VAL   
  505    CONTINUE 
            DO 56 K = 1, NZBLK
 507     I = IRAND ( NRED+1, N, ISEED ) 
         J = IRAND ( 1, NRED, ISEED ) 
            PI = P(I)       
            PJ = P(J)       
            VAL = - RANDOM(ISEED)     
      IF(IDEBUG .NE. 0) WRITE(NOUT,6002) PI,PJ,VAL
            CALL SBSIJ ( N, MAXNZ, IA, JA, A, IWKSP, PI, PJ, VAL, -1, 
     A                  ILEVEL, NOUT, IER)      
            IF ( IER .EQ. 700 ) GO TO 507       
           IF ( IER .GT. 700) STOP    
            ROW(PI) = ROW(PI) - VAL   
  56  CONTINUE    
C       
C ****************************************************************  
C       
C       SET DIAGONAL
C       
   57    CONTINUE 
         DO 40 K = 1, N     
            VAL =  ROW(K) + 1.E0      
            PI = K
            PJ = K
      IF(IDEBUG .NE. 0) WRITE(NOUT,6002) PI,PJ,VAL
            CALL SBSIJ ( N, MAXNZ, IA, JA, A, IWKSP, PI, PJ, VAL, 0,
     A                  ILEVEL, NOUT, IER)      
            IF ( IER .GT. 700 ) STOP  
  40     CONTINUE 
      CALL SBEND(N,MAXNZ,IA,JA,A,IWKSP) 
      IF(IDEBUG .EQ. 0) GO TO 4441    
      CALL SYSOUT(N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    
 4441 CONTINUE    
C       
C     SET SOME DIAGONAL ELEMENTS VERY LARGE     
C       
      IF(LARGE .EQ. 0) GO TO 5553     
      EVAL = 1.E8 
      CALL SBAGN(N,MAXNZ,IA,JA,A,IWKSP,ILEVEL,NOUT,IER)   
      IF(IER .NE. 0) STOP   
C       
      DO 43 K=1,N,5 
      NBIG = IRAND(1,N,ISEED) 
      RHS(NBIG) = EVAL + RHS(NBIG)    
      PI = NBIG   
      PJ = NBIG   
      IF(IDEBUG .NE. 0) WRITE(NOUT,6002) PI,PJ,VAL
      CALL SBSIJ(N,MAXNZ,IA,JA,A,IWKSP,PI,PJ,EVAL,1,      
     A           ILEVEL, NOUT, IER)   
      IF(IER .GT. 700)  STOP
   43 CONTINUE    
      CALL SBEND(N,MAXNZ,IA,JA,A,IWKSP) 
C       
      IF(IDEBUG .EQ. 0) GO TO 5553    
      CALL SYSOUT(N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    
C       
C ...... TEST ROUTINE TO REMOVE ROWS AND COLUMNS
C       
 5553 CONTINUE    
      TOL = 1.E-8 
      CALL SBELM(N,IA,JA,A,RHS,IROW,ROW,TOL,ISYM,ILEVEL,NOUT,IER)   
      IF(IER .NE. 0) STOP   
      IF(IDEBUG .EQ. 0) GO TO 4442    
      CALL SYSOUT(N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    
 4442 CONTINUE    
C       
C ...... SYSTEM SET-UP      
C       
      IF(IPLT .EQ. 0)  GO TO 899      
      WRITE(NOUT,6015)      
 6015 FORMAT(1H1/2X,45HPATTERN OF NONZEROS WITH NATURAL ORDERING    /)
      CALL PLTADJ ( N, IA, JA, ISYM, IROW, IWKSP, NOUT )  
  899    IF ( IDEBUG .EQ. 0 ) RETURN  
C       
C       DEBUGGING PRINT OUT 
C       
      WRITE(NOUT,900)       
  900 FORMAT(//2X,14HDEBUG PRINTING   
     A         ,2X,34HP ARRAY  BEFORE RED-BLACK INDEXING  ) 
      WRITE(NOUT,901) (P(I),I=1,N)    
  901 FORMAT(2X,10I8)       
      CALL SYSOUT(N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    
      RETURN      
      END 
      SUBROUTINE TSTPRB (IA,JA,A,RHS,P,IP,IROW,ICOL)      
C       
C     TEST RED-BLACK INDEXING FOR SYSTEMS       
C       
      INTEGER IA(1),JA(1),P(1),IP(1),IROW(1),ICOL(1)      
      REAL  A(1),RHS(1)     
C       
C       
      COMMON /TBK31/ SRELPR, ZETA     
      COMMON /TBK32/ IDEBUG, IERAN, ILEVEL, IORD, IPCT, IPLT, ISEED,
     A                 ISYM, ITMAX, LARGE, LEVEL, MAXNZ, MAXN, NOUT,
     B                 NBLACK, NB, NRBN, NRED, NW, NZBLK, NZRED, N  
C       
      TIMI1 = TIMER(0.0)    
C       
         CALL PRBNDX ( N, NB, IA, JA, P, IP, ILEVEL, NOUT, IER )    
C       
      TIMI2 = TIMER(0.0)    
      TIME = TIMI2 - TIMI1  
C       
C       PRINT RESULTS       
C       
         NR = N - NB
         WRITE(NOUT,6010) NR,IER,TIME 
 6010    FORMAT(//5X,33HCOMPUTED NUMBER OF RED POINTS   =, I5       
     C           /5X,33HIER                             =, I5       
     D           /5X,33HELAPSED TIME FOR INDEXING       =, F5.3 / ) 
C       
C       VERIFY RED-BLACK ORDERING     
C       
      CALL PERMAT (N,IA,JA,A,P,ICOL,ISYM,ILEVEL,NOUT,IER) 
      CALL PERVEC (N,RHS,P) 
      IF(IER .NE. 0) STOP   
C       
      IF(IPLT .EQ. 0)  GO TO 899      
      WRITE(NOUT,6017)      
 6017 FORMAT(1H1/2X,45HPATTERN OF NONZEROS WITH RED-BLACK ORDERING  /)
      CALL PLTADJ ( N, IA, JA, ISYM, IROW, ICOL, NOUT )   
  899    IF ( IDEBUG .EQ. 0 ) GO TO 70
      WRITE(NOUT,900)       
900   FORMAT(//2X,14HDEBUG PRINTING   
     A         ,2X,33HP ARRAY  AFTER RED-BLACK INDEXING ) 
      WRITE(NOUT,901) (P(I),I=1,N)    
901   FORMAT(2X,10I8)       
      CALL SYSOUT(N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)    
   70    CONTINUE 
      RETURN      
      END 
      SUBROUTINE TSTITP ( IA, JA, A, RHS , U, IWKSP, WKSP ) 
C       
C     PROGRAM TO EXERCISE ITPACK 2C   
C       
      INTEGER IA(1),JA(1),IWKSP(1),IPARM(12),ITER(7),IWORK(7)       
      REAL A(1),RHS(1),WKSP(1),RPARM(12),DIGIT1(7),DIGIT2(7),DIGIT3(7), 
     A        TIM1(7),TIM2(7),U(1)    
C       
      COMMON /TBK31/ SRELPR, ZETA     
      COMMON /TBK32/ IDEBUG, IERAN, ILEVEL, IORD, IPCT, IPLT, ISEED,
     A                 ISYM, ITMAX, LARGE, LEVEL, MAXNZ, MAXN, NOUT,
     B                 NBLACK, NB, NRBN, NRED, NW, NZBLK, NZRED, N  
C       
C        TEST JCG 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL JCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(1)=RPARM(9)   
         TIM2(1) = RPARM(10)
         DIGIT1(1) = RPARM(11)
         DIGIT2(1) = RPARM(12)
         CALL CHKNRM(U,WKSP,DIGIT3(1))
         ITER(1)=IPARM(1)   
         IWORK(1)=IPARM(8)  
C       
C        TEST JSI 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL JSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(2)=RPARM(9)   
         TIM2(2) = RPARM(10)
         DIGIT1(2) = RPARM(11)
         CALL CHKNRM(U,WKSP,DIGIT3(2))
         DIGIT2(2) = RPARM(12)
         ITER(2)=IPARM(1)   
         IWORK(2)=IPARM(8)  
C       
C        TEST SOR 
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL SOR (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
         TIM1(3)=RPARM(9)   
         TIM2(3) = RPARM(10)
         DIGIT1(3) = RPARM(11)
         CALL CHKNRM(U,WKSP,DIGIT3(3))
         DIGIT2(3) = RPARM(12)
         ITER(3)=IPARM(1)   
         IWORK(3)=IPARM(8)  
C       
C        TEST SSORCG
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         RPARM(7)=.26E0     
         CALL VFILL (N,U,0.E0)
         CALL SSORCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)
         TIM1(4)=RPARM(9)   
         TIM2(4) = RPARM(10)
         DIGIT1(4) = RPARM(11)
         DIGIT2(4) = RPARM(12)
         CALL CHKNRM(U,WKSP,DIGIT3(4))
         ITER(4)=IPARM(1)   
         IWORK(4)=IPARM(8)  
C       
C        TEST SSORSI
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL SSORSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)
         TIM1(5)=RPARM(9)   
         TIM2(5) = RPARM(10)
         DIGIT1(5) = RPARM(11)
         DIGIT2(5) = RPARM(12)
         CALL CHKNRM(U,WKSP,DIGIT3(5))
         ITER(5)=IPARM(1)   
         IWORK(5)=IPARM(8)  
C       
C        TEST RSCG
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IF(IORD .EQ. 2)  IPARM(9) = NB 
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL RSCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  
         TIM1(6)=RPARM(9)   
         TIM2(6) = RPARM(10)
         DIGIT1(6) = RPARM(11)
         DIGIT2(6) = RPARM(12)
         CALL CHKNRM(U,WKSP,DIGIT3(6))
         ITER(6)=IPARM(1)   
         IWORK(6)=IPARM(8)  
C       
C        TEST RSSI
C       
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=ITMAX     
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IF(IORD .EQ. 2)  IPARM(9) = NB 
         IPARM(12)=IERAN    
         RPARM(1)=ZETA      
         CALL VFILL (N,U,0.E0)
         CALL RSSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  
         TIM1(7)=RPARM(9)   
         TIM2(7) = RPARM(10)
         DIGIT1(7) = RPARM(11)
         DIGIT2(7) = RPARM(12)
         CALL CHKNRM(U,WKSP,DIGIT3(7))
         ITER(7)=IPARM(1)   
         IWORK(7)=IPARM(8)  
C       
C     TIMING ANALYSIS       
C       
      NP1 = N+1   
         CALL TIME2 (N,IA,JA,A,WKSP,WKSP(NP1),ITER,TIM1,TIM2,       
     A                DIGIT1,DIGIT2,DIGIT3,IWORK,NOUT)    
C       
      RETURN      
      END 
      SUBROUTINE TIME2 (N,IA,JA,A,V,W,ITER,TIM1,TIM2,DIGIT1,DIGIT2, 
     A                  DIGIT3,IWORK,NOUT)      
      INTEGER ITER(7),IA(1),JA(1),IWORK(7)      
      REAL V(N),W(N),REL(7),A(1),DIGIT1(7),DIGIT2(7),DIGIT3(7),     
     A     TIM1(7),TIM2(7)  
      DATA MAXLP/50/
C       
      CALL VFILL (N,V,1.E0) 
      TIMI1 = TIMER(0.0)    
      DO 10 I=1,MAXLP       
         CALL PMULT (N,IA,JA,A,V,W)   
   10 CONTINUE    
      TIMI2 = TIMER(0.0)    
      TIMMAT = (TIMI2 - TIMI1)/FLOAT(MAXLP)     
C       
      WRITE (NOUT,30)       
      DO 20 I=1,7 
          REL(I) = 0.E0     
          TEMP = FLOAT(ITER(I))       
          IF ((TIMI2 .EQ. TIMI1) .OR. (ITER(I) .EQ. 0))  GO TO 20   
          REL(I) = TIM1(I)/(TEMP*TIMMAT)
   20 CONTINUE    
C       
      WRITE (NOUT,40) (TIM1(I),TIM2(I),ITER(I),REL(I),DIGIT1(I),    
     A                   DIGIT2(I),DIGIT3(I),IWORK(I),I=1,7)
C       
      RETURN      
C       
   30 FORMAT(1H0,3(/),15X,15HTIMING ANALYSIS/15X,15(1H-)//
     A 35X,9HITERATION,3X,5HTOTAL,9X,6HNO. OF,4X,9HAVG ITER/,       
     B 2X,6HDIGITS,4X,6HDIGITS,5X,6HDIGITS,5X,4HWKSP/15X, 
     C 6HMETHOD,14X,10HTIME (SEC),2X,10HTIME (SEC),1X,    
     D 10HITERATIONS,1X,12HMTX-VCTR MLT,1X,7HSTP TST,     
     E 3X,7HRES/RHS,4X,4HTRUE,7X,4HUSED/15X,103(1H-))     
   40 FORMAT(15X,17HJACOBI CG        ,2F10.3,I14,F10.1,1X,3F10.1,I10/ 
     2       15X,17HJACOBI SI        ,2F10.3,I14,F10.1,1X,3F10.1,I10/ 
     3       15X,17HSOR              ,2F10.3,I14,F10.1,1X,3F10.1,I10/ 
     4       15X,17HSYMMETRIC SOR CG ,2F10.3,I14,F10.1,1X,3F10.1,I10/ 
     5       15X,17HSYMMETRIC SOR SI ,2F10.3,I14,F10.1,1X,3F10.1,I10/ 
     6       15X,17HREDUCED SYSTEM CG,2F10.3,I14,F10.1,1X,3F10.1,I10/ 
     7       15X,17HREDUCED SYSTEM SI,2F10.3,I14,F10.1,1X,3F10.1,I10/)
C       
C       
      END 
      SUBROUTINE CHKNRM(U,WKSP,DIGIT) 
C       
C     COMPUTE TRUE RATIO    
C       
      REAL U(1),WKSP(1)     
C       
      COMMON /TBK31/ SRELPR, ZETA     
      COMMON /TBK32/ IDEBUG, IERAN, ILEVEL, IORD, IPCT, IPLT, ISEED,
     A                 ISYM, ITMAX, LARGE, LEVEL, MAXNZ, MAXN, NOUT,
     B                 NBLACK, NB, NRBN, NRED, NW, NZBLK, NZRED, N  
C       
      DO 10 I=1,N 
   10 WKSP(I) = U(I) - 1.E0 
C       
      DIGIT = -ALOG10(SRELPR) 
      WKNRM = SDOT(N,WKSP,1,WKSP,1)   
      IF(WKNRM .EQ. 0.E0) GO TO 20    
      DIGIT = -( ALOG10(WKNRM) - ALOG10(FLOAT(N)) )/2.0E0 
C       
   20 IF(ILEVEL .EQ. 1) WRITE(NOUT,30) DIGIT    
   30 FORMAT(/13X,29HNO. OF DIGITS IN TRUE RATIO =,F5.1)  
C       
      RETURN      
      END 
      INTEGER FUNCTION IRAND ( I, J, ISEED)     
C       
C*****************************************************************  
C       
C      THIS SUBPROGRAM GENERATES UNIFORMLY DISTRIBUTED RANDOM       
C      INTEGERS BETWEEN  I  AND  J  (INCLUSIVE) 
C       
C****************************************************************** 
C       
       INTEGER  I, J, ISEED 
C       
C==================================================================== 
C       
      IRAND = IFIX(FLOAT(J-I+1)*RANDOM(ISEED)) + I
C       
       RETURN     
       END
      FUNCTION RANDOM(ISEED)
C       
C     RANDOM NUMBER GENERATOR - UNIFORMLY DISTRIBUTED IN (0,1)      
C                               ISEED IN (1,2147483647)   
C       
C     FOLLOWING CODE USED BECAUSE OF POSSIBLE USE WITH    
C     SHORT WORD LENGTH COMPUTERS     
C       
      DOUBLE PRECISION DL   
      DL = DMOD(16807.0D0*DBLE(FLOAT(ISEED)),2147483647.0D0)
      ISEED = IDINT(DL)     
      RANDOM = SNGL(DL*4.6566128752458D-10)     
C       
C     ON LONG WORD LENGTH COMPUTERS THE FOLLOWING CODE    
C     MAY BE USED OR LOCAL RANDOM NUMBER GENERATOR
C       
C     ISEED = MOD(16807*ISEED,2147483647)       
C     RANDOM = FLOAT(ISEED)*4.6566128752458E-10 
C       
      RETURN      
      END 
      SUBROUTINE PLTADJ ( N, IA, JA, ISYM, IROW, ICOL, NOUT )       
C       
C******************************************************************** 
C       
C     THIS SUBROUTINE PLOTS THE ADJACENCY STRUCTURE OF A SPARE      
C     MATRIX STORED IN SYMMETRIC/NONSYMMETRIC FORMAT      
C       
C     WRITTEN BY ROGER G GRIMES    AUGUST 1980  
C       
C*********************************************************************
C       
      INTEGER  N, IA(1), JA(1), ICOL(1), IROW(N)
C       
      INTEGER  BLANK, STAR, I, M, IM1, KADD, IBGN, IEND, J, JJ      
C       
      DATA   BLANK / 1H   /,   STAR / 1H*  /    
C       
C==================================================================== 
C       
C ... SET WORKSPACE ARRAYS TO ALL BLANKS.  IROW WILL BE USED TO INDICATE
C     THE ADJACENCY STRUCTURE FOR THE CURRENT ROW.  ICOL WILL STORE THE 
C     ADJACENCY STRUCTURE FOR THE LOWER TRIANGLE FOR COLUMNS 1 THRU 
C     THE CURRENT COLUMN ( SAME AS CURRENT ROW ). 
C       
         DO 10 I = 1, N     
            IROW(I) = BLANK 
  10     CONTINUE 
         IF (ISYM .NE. 0) GO TO 25    
         M = N * ( N + 1 ) / 2
         DO 20 I = 1, M     
            ICOL(I) = BLANK 
  20     CONTINUE 
C       
C ... LOOP OVER EACH ROW ( AND COLUMN ) 
C       
 25      KADD = 0 
         DO 80 I = 1, N     
            IM1 = I - 1     
            KADD = KADD + IM1 
            IBGN = IA(I)    
            IEND = IA(I+1) - 1
            IF ( IBGN .GT. IEND ) GO TO 40      
            DO 30 J = IBGN, IEND      
               JJ = JA(J)   
               IROW(JJ) = STAR
               IF (ISYM .NE. 0) GO TO 30
               K = ( JJ-1 ) * JJ / 2   +   I    
               ICOL(K) = STAR 
  30        CONTINUE
            IF ( I .EQ. 1 ) GO TO 55  
            IF (ISYM .NE. 0) GO TO 55 
  40        DO 50 J = 1, IM1
              ITEMP = KADD + J
              IROW(J) = ICOL ( ITEMP )
  50        CONTINUE
  55        WRITE(NOUT,60) I, (IROW(K),K=1,N)   
  60        FORMAT ( 2X,I5,3X,120A1)  
            DO 70 J = 1, N  
               IROW(J) = BLANK
  70        CONTINUE
  80     CONTINUE 
C       
C ... RETURN      
C       
         RETURN   
      END 
      SUBROUTINE SYSOUT(N,NZRED,NZBLK,IA,JA,A,RHS,NOUT,ISYM)
C       
C     PRINT SYSTEM OUT IN 2X2 FORMAT IF  N .LE. 10
C       
      INTEGER IA(1),JA(1)   
      REAL  A(1),RHS(1),TMP(10,10)    
C       
      WRITE(NOUT,51)
   51 FORMAT(16H0  SYSTEM (A,B)  )    
C       
      IF (N .GT. 10) GO TO 50 
C       
      DO 5 I=1,N  
      DO 5 J=1,N  
    5 TMP(I,J) = 0.E0       
C       
      DO 10 I=1,N 
      IBGN = IA(I)
      IEND = IA(I+1) - 1    
      IF(IBGN .GT. IEND)  GO TO 10    
      DO 20 J=IBGN,IEND     
      JAJ = JA(J) 
      TMP(I,JAJ) = A(J)     
      IF(ISYM .EQ. 0)  TMP(JAJ,I) = A(J)
20    CONTINUE    
10    CONTINUE    
C       
      DO 15 I=1,N 
      WRITE(NOUT,992) (TMP(I,J),J=1,N),RHS(I)   
15    CONTINUE    
      RETURN      
C       
50    CONTINUE    
      WRITE(NOUT,880)       
880   FORMAT(//2X,8HIA ARRAY )
            IEND = N + 1    
            WRITE(NOUT,991) (IA(I),I=1,IEND)    
991   FORMAT(2X,10I8)       
            IEND = IA(IEND) - 1       
      WRITE(NOUT,881)       
881   FORMAT(//2X,8HJA ARRAY )
            WRITE(NOUT,991) ( JA(I),I=1,IEND )  
            NZEND = NZRED + N 
            IF(ISYM .NE. 0) NZEND = NZEND + NZBLK 
      WRITE(NOUT,882)       
882   FORMAT(//2X,8HA ARRAY  )
            WRITE(NOUT,992) ( A(I),I=1,NZEND)   
      WRITE(NOUT,883)       
883   FORMAT(//2X,10HRHS ARRAY )      
      WRITE(NOUT,992) ( RHS(I),I=1,N) 
992   FORMAT(/2X,11(1X,G11.4))
C       
      RETURN      
      END 
      SUBROUTINE TEST4(EPSI)
C       
C ... TEST4 EXERCISES ITPACK 2C WITH VARIOUS SWITCHES SET 
C     ON THE LINEAR SYSTEM CORRESPONDING TO THE FINITE    
C     DIFFERENCE SOLUTION TO THE PDE  
C       
C              U   + U   + BU  = F    
C               XX    YY     X
C       
C     SUBJECT TO U(.,0) = U(.,1) = U(0,.) = U(1,.) = 0    
C       
C     WHERE F = 2X(X-1) + Y(Y-1)(2-B(1-2X)). THE TRUE SOLUTION      
C     IS U = XY(1-X)(1-Y). THE MESH SIZE IS 1/4.
C       
      INTEGER IA(10),JA(33),IPARM(12),IWKSP(27) 
      REAL    A(33),RHS(9),U(9),RPARM(12),WKSP(460)       
C       
      F(X,Y,BTA) = 2.*X*(X-1.)+Y*(Y-1.)*(2.-BTA*(1.-2.*X))
C       
      NW=460      
      N=9 
      LEVEL=0     
      IDGTS=1     
      NOUT=6      
      WRITE(NOUT,2) 
    2 FORMAT(1H1 //15X,32HITPACK 2C  TEST PROGRAM -- TEST4  )       
C       
C     ************* START LOOP TO DO BOTH CASES ******************  
C       
      DO 80 K=2,11,9
         BETA=FLOAT(K-2)    
         H=1./4.  
         B=1./4.  
         C=1./2.  
         D=3./4.  
         RHS(1)=-H*H*F(B,B,BETA)      
         RHS(2)=-H*H*F(C,B,BETA)      
         RHS(3)=-H*H*F(D,B,BETA)      
         RHS(4)=-H*H*F(B,C,BETA)      
         RHS(5)=-H*H*F(C,C,BETA)      
         RHS(6)=-H*H*F(D,C,BETA)      
         RHS(7)=-H*H*F(B,D,BETA)      
         RHS(8)=-H*H*F(C,D,BETA)      
         RHS(9)=-H*H*F(D,D,BETA)      
         IBGN=2   
         IF(K .EQ. 2) IBGN=1
      DO 80 I=IBGN,2
         WRITE(NOUT,5)      
         WRITE(NOUT,6) BETA 
    5    FORMAT(50H0 *********************************************** )
    6    FORMAT(5X,7H BETA =,F10.5)   
         ISYM=I-1 
         IF(ISYM .EQ. 0)  CALL SSEXP(IA,JA,A,NOUT)
         IF(ISYM .NE. 0)  CALL NSEXP(IA,JA,A,BETA,H,NOUT) 
C       
C       
C        TEST JCG 
C       
      DO 10 KK=1,2
      IADAPT=KK-1 
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=12
         IPARM(2)=LEVEL     
         IF(IADAPT .EQ. 0) IPARM(2)=5 
         IPARM(5)=ISYM      
         IPARM(6)=IADAPT    
         IPARM(12)=IDGTS    
         RPARM(1)=1.E-20    
         RPARM(2)=.75E0     
         RPARM(3)=-.65E0    
         CALL VFILL (N,U,5.E-2)       
         CALL JCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
   10 CONTINUE    
C       
C        TEST JSI 
C       
      DO 20 ICASE=1,2       
      DO 20 KK=1,2
      IADAPT=KK-1 
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=27
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(6)=IADAPT    
         IPARM(7)=ICASE     
         IPARM(12)=IDGTS    
         RPARM(1)=1.E-1     
         RPARM(2)=.75E0     
         RPARM(3)=-.65E0    
         RPARM(4)=1.E0      
         CALL VFILL (N,U,1.E0)
         CALL JSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
   20 CONTINUE    
C       
C        TEST SOR 
C       
      DO 30 KK=1,2
      IADAPT=KK-1 
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=10
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(6)=IADAPT    
         IPARM(7)=2 
         IPARM(12)=IDGTS    
         RPARM(1)=1.E-3     
         RPARM(2)=.71E0     
         RPARM(3)=-.89E0    
         RPARM(5)=1.2       
         CALL VFILL (N,U,0.E0)
         CALL SOR (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)   
   30 CONTINUE    
C       
C        TEST SSORCG
C       
      DO 40 ICASE=1,2       
      DO 40 KK=1,4
      IADAPT=KK-1 
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=4 
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(6)=IADAPT    
         IPARM(7)=ICASE     
         IPARM(12)=IDGTS    
         RPARM(1)=1.0E-6    
         RPARM(2)=.8E0      
         RPARM(3)=-.25E0    
         RPARM(5)=1.2E0     
         RPARM(6)=.9E0      
         RPARM(7)=.26E0     
         CALL VFILL (N,U,0.E0)
         CALL SSORCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)
   40 CONTINUE    
C       
C        TEST SSORSI
C       
      DO 50 ICASE=1,2       
      DO 50 KK=1,4
      IADAPT=KK-1 
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=8 
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(6)=IADAPT    
         IPARM(7)=ICASE     
         IPARM(12)=IDGTS    
         RPARM(2)=.22E0     
         RPARM(5)=1.25E0    
         RPARM(6)=.25E0     
         RPARM(7)=.24E0     
         CALL VFILL (N,U,0.E0)
         CALL SSORSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)
   50 CONTINUE    
C       
C        TEST RSCG
C       
      DO 60 KK=1,2
      IADAPT=KK-1 
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=4 
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(6)=IADAPT    
         IPARM(12)=IDGTS    
      IF(IADAPT .EQ. 0) IPARM(12) = 4 
         RPARM(1)=1.0E-20   
         RPARM(2)=.85E0     
         RPARM(3)=-.5E0     
         RPARM(4)=.5E0      
         CALL VFILL (N,U,5.E-2)       
         CALL RSCG (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  
   60 CONTINUE    
C       
C        TEST RSSI
C       
      DO 70 KK=1,2
      IADAPT=KK-1 
         CALL DFAULT (IPARM,RPARM)    
         IPARM(1)=20
         IPARM(2)=LEVEL     
         IPARM(5)=ISYM      
         IPARM(6)=IADAPT    
         IPARM(12)=IDGTS    
         RPARM(2)=.9E0      
         RPARM(3)=-.75E0    
         CALL VFILL (N,U,0.E0)
         CALL RSSI (N,IA,JA,A,RHS,U,IWKSP,NW,WKSP,IPARM,RPARM,IER)  
   70 CONTINUE    
C       
   80 CONTINUE    
C       
      RETURN      
      END 
      SUBROUTINE SSEXP(IA,JA,A,NOUT)  
      INTEGER IA(10),JA(33) 
      REAL    A(33) 
      WRITE(NOUT,10)
   10 FORMAT(24H0 SYMMETRIC STORAGE USED )      
      A(21)=4.0   
      A(19)=A(21) 
      A(17)=A(19) 
      A(15)=A(17) 
      A(12)=A(15) 
      A(9)=A(12)  
      A(7)=A(9)   
      A(4)=A(7)   
      A(1)=A(4)   
      A(20)=-1.0  
      A(18)=A(20) 
      A(16)=A(18) 
      A(14)=A(16) 
      A(13)=A(14) 
      A(11)=A(13) 
      A(10)=A(11) 
      A(8)=A(10)  
      A(6)=A(8)   
      A(5)=A(6)   
      A(3)=A(5)   
      A(2)=A(3)   
      JA(1)=1     
      JA(2)=2     
      JA(3)=4     
      JA(4)=2     
      JA(5)=3     
      JA(6)=5     
      JA(7)=3     
      JA(8)=6     
      JA(9)=4     
      JA(10)=5    
      JA(11)=7    
      JA(12)=5    
      JA(13)=6    
      JA(14)=8    
      JA(15)=6    
      JA(16)=9    
      JA(17)=7    
      JA(18)=8    
      JA(19)=8    
      JA(20)=9    
      JA(21)=9    
      IA(1)=1     
      IA(2)=4     
      IA(3)=7     
      IA(4)=9     
      IA(5)=12    
      IA(6)=15    
      IA(7)=17    
      IA(8)=19    
      IA(9)=21    
      IA(10)=22   
      RETURN      
C       
      END 
      SUBROUTINE NSEXP(IA,JA,A,BETA,H,NOUT)     
      INTEGER IA(10),JA(33) 
      REAL    A(33) 
      WRITE(NOUT,10)
   10 FORMAT(27H0 NONSYMMETRIC STORAGE USED )   
      JA(1)=1     
      JA(2)=2     
      JA(3)=4     
      JA(4)=1     
      JA(5)=2     
      JA(6)=3     
      JA(7)=5     
      JA(8)=2     
      JA(9)=3     
      JA(10)=6    
      JA(11)=1    
      JA(12)=4    
      JA(13)=5    
      JA(14)=7    
      JA(15)=2    
      JA(16)=4    
      JA(17)=5    
      JA(18)=6    
      JA(19)=8    
      JA(20)=3    
      JA(21)=5    
      JA(22)=6    
      JA(23)=9    
      JA(24)=4    
      JA(25)=7    
      JA(26)=8    
      JA(27)=5    
      JA(28)=7    
      JA(29)=8    
      JA(30)=9    
      JA(31)=6    
      JA(32)=8    
      JA(33)=9    
      IA(1)=1     
      IA(2)=4     
      IA(3)=8     
      IA(4)=11    
      IA(5)=15    
      IA(6)=20    
      IA(7)=24    
      IA(8)=27    
      IA(9)=31    
      IA(10)=34   
      A(33)=4.0   
      A(29)=A(33) 
      A(25)=A(29) 
      A(22)=A(25) 
      A(17)=A(22) 
      A(12)=A(17) 
      A(9)=A(12)  
      A(5)=A(9)   
      A(1)=A(5)   
      A(31)=-1.0  
      A(27)=A(31) 
      A(24)=A(27) 
      A(23)=A(24) 
      A(20)=A(23) 
      A(19)=A(20) 
      A(15)=A(19) 
      A(14)=A(15) 
      A(11)=A(14) 
      A(10)=A(11) 
      A(7)=A(10)  
      A(3)=A(7)   
      A(30)=-(1.+0.5*H*BETA)
      A(26)=A(30) 
      A(18)=A(26) 
      A(13)=A(18) 
      A(6)=A(13)  
      A(2)=A(6)   
      A(32)=-(1.-0.5*H*BETA)
      A(28)=A(32) 
      A(21)=A(28) 
      A(16)=A(21) 
      A(8)=A(16)  
      A(4)=A(8)   
      RETURN      
C       
      END 
