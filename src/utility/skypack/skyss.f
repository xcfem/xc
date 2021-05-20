      SUBROUTINE SKYSS (LN,N,NX,A,D,X,DGPNT,BLOCK,NBLOCK,
     &                  FNAME,FUNIT,INFO)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYSS solves (A)*(X)=(Y) where (A) is stored in a skyline way    *
C*    and factorized as (A)=(L)*(D)*(L')                               *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    LN     (sii) : leading dimension of (X)                          *
C*    N      (sii) : dimension of the matrix                           *
C*    NX     (sii) : number of right-hand sides                        *
C*    A      (arb) : the factor (L) and the reciprocals of (D),        *
C*                   which are possibly partitioned into               *
C*                   NBLOCK blocks stored in the binary                *
C*                   file specified by FNAME                           *
C*    D      (arw) : reciprocal of the entries in (D)                  *
C*    X      (arb) : right-hand sides, LNxNX array                     *
C*    DGPNT  (aii) : addresses of the diagonal coefficients of (L)     *
C*    BLOCK  (aii) : blocking information                              *
C*    NBLOCK (sii) : number of blocks                                  *
C*    FNAME  (ssi) : file name associated with (A)                     *
C*    FUNIT  (sii) : file unit associated with (A), which is ignored   *
C*                   if FNAME is equal to "INCORE" or "incore"         *
C*    INFO   (sio) : information on the execution of SKYSS             *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                   = 2, the file specified by FNAME does not exist   *
C*                                                                     *
C*  - SUBPROGRAMS :                                                    *
C*    SKYSSB,SKYSSD,SKYSSF                                             *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),INFO,FUNIT,LN,N,NBLOCK,NX
      DOUBLE PRECISION A(*),D(*),X(LN,NX)
      CHARACTER*(*)    FNAME
C
C==== local variables ==================================================
C
      LOGICAL          ALIVE,JOINED
C
C**** executable statements ********************************************
C
C.... if FNAME is given check the status of the file ...................
C
      IF ( FNAME.NE.'INCORE' .AND. FNAME.NE.'incore' ) THEN
C
         INQUIRE (FILE=FNAME,EXIST=ALIVE,OPENED=JOINED,ERR=1) 
C
         IF      ( .NOT.ALIVE  ) THEN
                 INFO = 2
                 RETURN
         ELSE IF ( .NOT.JOINED ) THEN
                 OPEN   (UNIT  =FUNIT,
     &                   FILE  =FNAME,
     &                   FORM  ='UNFORMATTED',
     &                   ACCESS='SEQUENTIAL',
     &                   STATUS='OLD',
     &                   ERR   =1)
         END IF
C
      END IF
C
C.... forward substitution phase .......................................
C
      CALL SKYSSF (LN,NX,A,D,X,DGPNT,BLOCK,NBLOCK,FNAME,FUNIT,INFO)
C
      IF ( INFO .GT. 0 ) RETURN
C
C.... pivot division phase .............................................
C
      CALL SKYSSD (LN,N,NX,D,X)
C
C.... backward substitution phase ......................................
C
      CALL SKYSSB (LN,NX,A,X,DGPNT,BLOCK,NBLOCK,FUNIT,INFO)
C
      RETURN
C
C.... an IO error has occurred .........................................
C
    1 INFO = 1
C
      RETURN
C
C**** end of SKYSS *****************************************************
C
      END
