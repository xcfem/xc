      SUBROUTINE SKYMEF (DGPNT,BLOCK,LBLOCK,FBNAME,FBUNIT,
     &                   MBTYPE,N,A,B,SIGMA,INFO) 
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYMEF computes the effective matrix (A)=(A)+SIGMA*(B)           *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    DGPNT  (aii) : addresses of the diagonal coefficients of (A)     *
C*    BLOCK  (aii) : blocking information                              *
C*    LBLOCK (sii) : index of the current block                        *
C*    FBNAME (ssi) : file name associated with (B)                     *
C*    FBUNIT (sii) : file unit associated with (B), which is ignored   *
C*                   if FBNAME is equal to "INCORE" or "incore"        *
C*    MBTYPE (sii) : type of (B)                                       *
C*                   = 1, (B) is diagonal                              *
C*                   = 2, (B) is skyline (upper triangle)              *
C*    N      (sii) : dimension of the matrix (A)                       *
C*    A      (arb) : coefficients of the effective matrix              *
C*    B      (arb) : coefficients to be added to (A)                   *
C*    SIGMA  (sri) : scalar that multiplies (B)                        *
C*    INFO   (sio) : information on the execution of SKYMEF            *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                   = 3, the file specified by FBNAME does not exist  *
C*                                                                     *
C*  - BLAS kernel :                                                    *
C*    DAXPY                                                            *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),INFO,LBLOCK,FBUNIT,MBTYPE,N
      DOUBLE PRECISION A(*),B(*),SIGMA
      CHARACTER*(*)    FBNAME
C
C==== local variables ==================================================
C
      INTEGER          I,IMAX,IMIN
      LOGICAL          ALIVE,JOINED
C
C**** executable statements ********************************************
C
      IMIN = BLOCK(1,LBLOCK)
      IMAX = BLOCK(2,LBLOCK)
C
      IF ( SIGMA .NE. 0.0D0 ) THEN
C
C....... if FBNAME is given check the status of the file ...............
C
         IF ( FBNAME.NE.'INCORE' .AND. FBNAME.NE.'incore' ) THEN
C
            INQUIRE (FILE=FBNAME,EXIST=ALIVE,OPENED=JOINED,ERR=1)
C
            IF      ( .NOT.ALIVE  ) THEN
                    INFO = 3
                    RETURN
            ELSE IF ( .NOT.JOINED ) THEN
                 OPEN (UNIT  =FBUNIT,
     &                 FILE  =FBNAME,
     &                 FORM  ='UNFORMATTED',
     &                 ACCESS='SEQUENTIAL',
     &                 STATUS='OLD',
     &                 ERR   =1)
            END IF
C
         END IF
C
C....... compute (A)=(A)+SIGMA*(B) .....................................
C
	 IF      ( MBTYPE .EQ. 1 ) THEN
C
C............... (B) is diagonal .......................................
C
                 IF ( FBNAME.NE.'INCORE' .AND. FBNAME.NE.'incore' ) THEN
	            REWIND (UNIT=FBUNIT,ERR=1)
	            READ   (UNIT=FBUNIT,ERR=1) (B(I),I=1,N)
                 END IF
C
	         DO 010 I = IMIN,IMAX
	            A(DGPNT(I)) = A(DGPNT(I)) + SIGMA*B(I)
  010            CONTINUE
C
	 ELSE IF ( MBTYPE .EQ. 2 ) THEN
C
C............... (B) is skyline ........................................
C
                 IF ( FBNAME.NE.'INCORE' .AND. FBNAME.NE.'incore' ) THEN
                    IF (LBLOCK.EQ.1) REWIND (UNIT=FBUNIT,ERR=1)
 	            READ (UNIT=FBUNIT,ERR=1) (B(I),I=1,DGPNT(IMAX))
                 END IF
C
		 CALL DAXPY (DGPNT(IMAX),SIGMA,B,1,A,1)
C
         END IF
C
      END IF
C
      RETURN
C
C.... an IO error has occurred .........................................
C
    1 INFO = 1
C
      RETURN
C
C**** end of SKYMEF ****************************************************
C
      END
