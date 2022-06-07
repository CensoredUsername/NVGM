
    ; SECTION .vectors

0000:    00 80  RESET_LOC: .word RESET
0002:    00 80 INTBT/4_LOC: .word RESET
0004:    00 80   INT0_LOC: .word RESET
0006:    00 80   INT1_LOC: .word RESET
0008:    05 cb INTCSI_LOC: .word INTCSI
000a:    05 e3  INTT0_LOC: .word INTT0
000c:    10 10  INTT1_LOC: .word sub_1010
000e:    10 10             .word sub_1010
0010:    10 10             .word sub_1010
0012:    10 10             .word sub_1010
0014:    10 10             .word sub_1010
0016:    10 10             .word sub_1010
0018:    10 10             .word sub_1010
001a:    10 10             .word sub_1010
001c:    10 10             .word sub_1010
001e:    10 10             .word sub_1010

    ; SECTION .geti

0020:    00 80     JMPRST: TBR RESET
0022:    10 10             TBR sub_1010
0024:    10 10             TBR sub_1010
0026:    10 10             TBR sub_1010
0028:    10 10             TBR sub_1010
002a:    10 10             TBR sub_1010
002c:    10 10             TBR sub_1010
002e:    10 10             TBR sub_1010
0030:    10 10             TBR sub_1010
0032:    10 10             TBR sub_1010
0034:    10 10             TBR sub_1010
0036:    10 10             TBR sub_1010
0038:    10 10             TBR sub_1010
003a:    10 10             TBR sub_1010
003c:    10 10             TBR sub_1010
003e:    10 10             TBR sub_1010
0040:    10 10             TBR sub_1010
0042:    10 10             TBR sub_1010
0044:    10 10             TBR sub_1010
0046:    10 10             TBR sub_1010
0048:    10 10             TBR sub_1010
004a:    10 10             TBR sub_1010
004c:    10 10             TBR sub_1010
004e:    10 10             TBR sub_1010
0050:    10 10             TBR sub_1010
0052:    10 10             TBR sub_1010
0054:    10 10             TBR sub_1010
0056:    10 10             TBR sub_1010
0058:    10 10             TBR sub_1010
005a:    10 10             TBR sub_1010
005c:    10 10             TBR sub_1010
005e:    10 10             TBR sub_1010
0060:    10 10             TBR sub_1010
0062:    10 10             TBR sub_1010
0064:    10 10             TBR sub_1010
0066:    10 10             TBR sub_1010
0068:    10 10             TBR sub_1010
006a:    10 10             TBR sub_1010
006c:    10 10             TBR sub_1010
006e:    10 10             TBR sub_1010
0070:    10 10             TBR sub_1010
0072:    10 10             TBR sub_1010
0074:    10 10             TBR sub_1010
0076:    10 10             TBR sub_1010
0078:    10 10             TBR sub_1010
007a:    10 10             TBR sub_1010
007c:    10 10             TBR sub_1010
007e:    10 10             TBR sub_1010

    ; SECTION .text


    ; FUNCTION RESET CALLS
        ; sub_0580,
        ; sub_0c06,
        ; sub_050f,
        ; sub_0c90,
        ; sub_0418,
        ; sub_0c98,
        ; sub_0598,
        ; sub_0399,
        ; sub_069c,
        ; sub_0198,
        ; sub_01a5,
        ; sub_05ab,
        ; sub_05b1,
        ; sub_06b1,
        ; sub_06b7,
        ; sub_05b7,
        ; sub_06bc,
        ; sub_0d43,
        ; sub_0c51,
        ; sub_05d2,
        ; sub_06d5,
        ; sub_0563,
        ; sub_02e4,
        ; sub_0c66,
        ; sub_03f9,
        ; sub_067e,
        ; sub_0eff
0080:    9c b2      RESET: DI
0082:       78             MOV A, #8
0083:    93 85             MOV [BTM/?85], A
0085:    b5 8b             SET1 [WDTM.3/?8B.3]
0087:    89 06             MOV XA, #6
0089:    92 98             MOV [WM/?98], XA
008b:    9f 8a   lbl_008b: SKTCLR [IRQW]
008d:       fd             BR lbl_008b
008e:       73             MOV A, #3
008f:    93 b3             MOV [PCC/?B3], A
0091:       71             MOV A, #1
0092:    93 cf             MOV [SOS/?CF], A
0094:       78             MOV A, #8
0095:    93 84             MOV [SBS/?84], A
0097:    9d 90             SET1 [MBE]
0099:    99 10             SEL MB0
009b:    8b 08             MOV HL, #8
009d:       70             MOV A, #0
009e:       e8   lbl_009e: MOV @HL, A
009f:       c2             INCS L
00a0:       fd             BR lbl_009e
00a1:       c3             INCS H
00a2:       fb             BR lbl_009e
00a3:    9c b2   lbl_00a3: DI
00a5:    9c 90             CLR1 [MBE]
00a7:    b5 85             SET1 [BTM.3/?85.3]
00a9:    89 00             MOV XA, #0
00ab:    92 80             MOV [SP/?80], XA
00ad: ab 46 7e             CALL sub_067e
00b0: ab 45 0f             CALL sub_050f
00b3: ab 45 63             CALL sub_0563
00b6: ab 46 9c             CALL sub_069c
00b9:    9d 9c             EI IET0
00bb:    9d b2             EI
00bd:    9c 90   lbl_00bd: CLR1 [MBE]
00bf:    99 10             SEL MB0
00c1:    84 4b             CLR1 [?4B.0]
00c3:    be c0             SKF [PORT0.0]
00c5:       0b             BR lbl_00d1
00c6:    be c0             SKF [PORT0.0]
00c8:       08             BR lbl_00d1
00c9:    be c0             SKF [PORT0.0]
00cb:       05             BR lbl_00d1
00cc:    be c0             SKF [PORT0.0]
00ce:       02             BR lbl_00d1
00cf:    51 de             BRCB lbl_01de
00d1:       70   lbl_00d1: MOV A, #0
00d2:    93 09             MOV [?09], A
00d4:       74             MOV A, #4
00d5:    93 08             MOV [?08], A
00d7: ab 46 b1   lbl_00d7: CALL sub_06b1
00da:    b6 2d             SKF [?2D.3]
00dc:    51 77             BRCB lbl_0177
00de:    bf c0             SKT [PORT0.0]
00e0:    51 77             BRCB lbl_0177
00e2:    a6 08             SKF [?08.2]
00e4:       f2             BR lbl_00d7
00e5: ab 44 18             CALL sub_0418
00e8:    51 77             BRCB lbl_0177
00ea: ab 46 b1             CALL sub_06b1
00ed:    bf eb             SKT [PORT11.2]
00ef:    51 77             BRCB lbl_0177
00f1: ab 41 a5             CALL sub_01a5
00f4:    51 77             BRCB lbl_0177
00f6: ab 46 b1             CALL sub_06b1
00f9:    89 64             MOV XA, #64
00fb:    92 3a             MOV [?3A], XA
00fd:    95 4a             SET1 [?4A.1]
00ff:    b6 08   lbl_00ff: SKF [?08.3]
0101:    51 14             BRCB lbl_0114
0103:    b6 2d             SKF [?2D.3]
0105:    51 2c             BRCB lbl_012c
0107:    96 4a             SKF [?4A.1]
0109:       f5             BR lbl_00ff
010a: ab 46 b1             CALL sub_06b1
010d:    89 32             MOV XA, #32
010f:    92 3c             MOV [?3C], XA
0111:    a5 4a             SET1 [?4A.2]
0113:       0d             BR lbl_0121
0114: ab 46 b1   lbl_0114: CALL sub_06b1
0117:    89 00             MOV XA, #0
0119:    92 3a             MOV [?3A], XA
011b:    89 64             MOV XA, #64
011d:    92 3c             MOV [?3C], XA
011f:    a5 4a             SET1 [?4A.2]
0121:    b6 2d   lbl_0121: SKF [?2D.3]
0123:       08             BR lbl_012c
0124: ab 46 b1             CALL sub_06b1
0127:    a6 4a             SKF [?4A.2]
0129:       f7             BR lbl_0121
012a:    51 77             BRCB lbl_0177
012c:    89 00   lbl_012c: MOV XA, #0
012e:    92 3a             MOV [?3A], XA
0130:    89 00             MOV XA, #0
0132:    92 3c             MOV [?3C], XA
0134: ab 46 b1   lbl_0134: CALL sub_06b1
0137:    b7 2d             SKT [?2D.3]
0139:       01             BR lbl_013b
013a:       f9             BR lbl_0134
013b:    b5 4b   lbl_013b: SET1 [?4B.3]
013d:    a2 68             MOV XA, [?68]
013f:    92 3e             MOV [?3E], XA
0141:    9a 01             SKE X, #0
0143:    51 77             BRCB lbl_0177
0145:    9a 00             SKE A, #0
0147:    51 77             BRCB lbl_0177
0149:    9d 90             SET1 [MBE]
014b:    99 10             SEL MB0
014d:    89 01             MOV XA, #1
014f:    92 88             MOV [?88], XA
0151:    9c 90             CLR1 [MBE]
0153: ab 46 b1             CALL sub_06b1
0156:    89 32             MOV XA, #32
0158:    92 36             MOV [?36], XA
015a:    85 4a             SET1 [?4A.0]
015c: ab 46 b1   lbl_015c: CALL sub_06b1
015f:    86 4a             SKF [?4A.0]
0161:       fa             BR lbl_015c
0162: ab 41 98             CALL sub_0198
0165:    a3 31             MOV A, [?31]
0167:    99 71             MOV X, A
0169:    a3 30             MOV A, [?30]
016b:    92 3e             MOV [?3E], XA
016d:    92 60             MOV [?60], XA
016f:    b4 4b             CLR1 [?4B.3]
0171: ab 46 b1   lbl_0171: CALL sub_06b1
0174:    bf eb             SKT [PORT11.2]
0176:       fa             BR lbl_0171
0177:    b7 08   lbl_0177: SKT [?08.3]
0179:    51 90             BRCB lbl_0190
017b: ab 46 b1   lbl_017b: CALL sub_06b1
017e:    b6 08             SKF [?08.3]
0180:       fa             BR lbl_017b
0181:    89 64             MOV XA, #64
0183:    92 3c             MOV [?3C], XA
0185:    a5 4a             SET1 [?4A.2]
0187:    b6 08   lbl_0187: SKF [?08.3]
0189:       f1             BR lbl_017b
018a: ab 46 b1             CALL sub_06b1
018d:    a6 4a             SKF [?4A.2]
018f:       f7             BR lbl_0187
0190:       70   lbl_0190: MOV A, #0
0191:    93 09             MOV [?09], A
0193:       70             MOV A, #0
0194:    93 08             MOV [?08], A
0196:    50 bd             BRCB lbl_00bd

    ; FUNCTION sub_0198 CALLS
        ; sub_06b1
0198:    89 fa   sub_0198: MOV XA, #FA
019a:    92 3c             MOV [?3C], XA
019c:    a5 4a             SET1 [?4A.2]
019e: ab 46 b1   lbl_019e: CALL sub_06b1
01a1:    a6 4a             SKF [?4A.2]
01a3:       fa             BR lbl_019e
01a4:       ee             RET

    ; FUNCTION sub_01a5 CALLS
        ; sub_01cc
01a5:    a3 30   sub_01a5: MOV A, [?30]
01a7:    9a 00             SKE A, #0
01a9:       06             BR lbl_01b0
01aa:    a3 31             MOV A, [?31]
01ac:    9a 00             SKE A, #0
01ae:       01             BR lbl_01b0
01af:       ee             RET
01b0:    a2 3e   lbl_01b0: MOV XA, [?3E]
01b2:    92 68             MOV [?68], XA
01b4:    8b 68             MOV HL, #68
01b6:       e6             CLR1 CY
01b7:    a3 34             MOV A, [?34]
01b9: ab 41 cc             CALL sub_01cc
01bc:    a3 35             MOV A, [?35]
01be: ab 41 cc             CALL sub_01cc
01c1:       d6             NOT1 CY
01c2:       d7             SKT CY
01c3:       ee             RET
01c4:       e0             RETS
01c5:       66             ADDS A, #6 ; DEAD
01c6:       a9             ADDC A, @HL ; DEAD
01c7:       6a             ADDS A, #A ; DEAD
01c8:       e8             MOV @HL, A ; DEAD
01c9:       c2             INCS L ; DEAD
01ca:       60             NOP ; DEAD
01cb:       ee             RET ; DEAD

    ; FUNCTION sub_01cc
01cc:       e9   sub_01cc: XCH A, @HL
01cd:       b8             SUBC A, @HL
01ce:       6a             ADDS A, #A
01cf:       e8             MOV @HL, A
01d0:       c2             INCS L
01d1:       60             NOP
01d2:       ee             RET
01d3:    99 0c             SKE A, E ; DEAD
01d5:       ee             RET ; DEAD
01d6:       d9             XCH A, X ; DEAD
01d7:    99 0d             SKE A, D ; DEAD
01d9:       02             BR lbl_01dc ; DEAD
01da:       d9             XCH A, X ; DEAD
01db:       e0             RETS ; DEAD

    ; DEAD BLOCK lbl_01dc
01dc:       d9   lbl_01dc: XCH A, X
01dd:       ee             RET
01de:    be c0   lbl_01de: SKF [PORT0.0]
01e0:    50 bd             BRCB lbl_00bd
01e2:    be c0             SKF [PORT0.0]
01e4:    50 bd             BRCB lbl_00bd
01e6:    be c0             SKF [PORT0.0]
01e8:    50 bd             BRCB lbl_00bd
01ea:    be c0             SKF [PORT0.0]
01ec:    50 bd             BRCB lbl_00bd
01ee:    be d1             SKF [PORT1.1]
01f0:       08             BR lbl_01f9
01f1:    be d1             SKF [PORT1.1]
01f3:       05             BR lbl_01f9
01f4:    be d1             SKF [PORT1.1]
01f6:       02             BR lbl_01f9
01f7:    52 48             BRCB lbl_0248
01f9: ab 43 99   lbl_01f9: CALL sub_0399
01fc:    50 a3             BRCB lbl_00a3
01fe:    9c b2             DI
0200: ab 4d 43             CALL sub_0d43
0203:    52 42             BRCB lbl_0242
0205: ab 4c 90             CALL sub_0c90
0208: ab 46 b7             CALL sub_06b7
020b: ab 4c 51             CALL sub_0c51
020e: ab 4c 98             CALL sub_0c98
0211: ab 46 b7             CALL sub_06b7
0214: ab 4c 06             CALL sub_0c06
0217:    9c 88             CLR1 [IRQBT]
0219:    9d b2             EI
021b:       d7             SKT CY
021c:    50 bd             BRCB lbl_00bd
021e:    89 04             MOV XA, #4
0220:    92 98             MOV [WM/?98], XA
0222:    9c 8a             CLR1 [IRQW]
0224:    84 2f             CLR1 [?2F.0]
0226:    94 2f             CLR1 [?2F.1]
0228: ab 46 b1   lbl_0228: CALL sub_06b1
022b:    9f 8a             SKTCLR [IRQW]
022d:       fa             BR lbl_0228
022e:    85 2f             SET1 [?2F.0]
0230:    95 2f             SET1 [?2F.1]
0232: ab 46 b1   lbl_0232: CALL sub_06b1
0235:    9f 8a             SKTCLR [IRQW]
0237:       fa             BR lbl_0232
0238:    84 2f             CLR1 [?2F.0]
023a:    94 2f             CLR1 [?2F.1]
023c:    89 00             MOV XA, #0
023e:    92 98             MOV [WM/?98], XA
0240:    51 f9             BRCB lbl_01f9
0242:    9c 88   lbl_0242: CLR1 [IRQBT]
0244:    9d b2             EI
0246:    51 f9             BRCB lbl_01f9
0248:    a3 f4   lbl_0248: IN A, PORT4
024a:    99 33             AND A, #3
024c:    9a 00             SKE A, #0
024e:       01             BR lbl_0250
024f:       0b             BR lbl_025b
0250:    9a 10   lbl_0250: SKE A, #1
0252:       02             BR lbl_0255
0253:    52 cd             BRCB lbl_02cd
0255:    9a 20   lbl_0255: SKE A, #2
0257:    53 68             BRCB lbl_0368
0259:    53 09             BRCB lbl_0309
025b: ab 43 f9   lbl_025b: CALL sub_03f9
025e:    50 bd             BRCB lbl_00bd
0260:    89 64             MOV XA, #64
0262:    92 3a             MOV [?3A], XA
0264: ab 44 18             CALL sub_0418
0267:       60             NOP
0268: ab 46 bc             CALL sub_06bc
026b:    9d c2             SET1 [PORT2.0]
026d:    a2 42             MOV XA, [?42]
026f: ab 45 b7             CALL sub_05b7
0272: ab 45 b1             CALL sub_05b1
0275:    a2 10             MOV XA, [?10]
0277: ab 45 80             CALL sub_0580
027a:    a2 12             MOV XA, [?12]
027c: ab 45 80             CALL sub_0580
027f: ab 46 b7             CALL sub_06b7
0282:    a2 14             MOV XA, [?14]
0284: ab 45 80             CALL sub_0580
0287:    a2 16             MOV XA, [?16]
0289: ab 45 80             CALL sub_0580
028c:    a2 18             MOV XA, [?18]
028e: ab 45 80             CALL sub_0580
0291: ab 46 b7             CALL sub_06b7
0294:    a2 1a             MOV XA, [?1A]
0296: ab 45 80             CALL sub_0580
0299:    a2 1c             MOV XA, [?1C]
029b: ab 45 80             CALL sub_0580
029e:    a3 1e             MOV A, [?1E]
02a0:    9a 09             MOV X, #0
02a2: ab 45 80             CALL sub_0580
02a5: ab 46 b7             CALL sub_06b7
02a8:    a3 1f             MOV A, [?1F]
02aa:    9a 09             MOV X, #0
02ac: ab 45 80             CALL sub_0580
02af: ab 45 b1             CALL sub_05b1
02b2: ab 46 b7             CALL sub_06b7
02b5:    a2 6c             MOV XA, [?6C]
02b7: ab 45 80             CALL sub_0580
02ba:    a2 6e             MOV XA, [?6E]
02bc: ab 45 80             CALL sub_0580
02bf: ab 45 ab             CALL sub_05ab
02c2: ab 46 b7             CALL sub_06b7
02c5: ab 45 d2             CALL sub_05d2
02c8: ab 46 d5             CALL sub_06d5
02cb:    52 5b             BRCB lbl_025b
02cd: ab 46 bc   lbl_02cd: CALL sub_06bc
02d0: ab 42 e4             CALL sub_02e4
02d3: ab 46 b7             CALL sub_06b7
02d6: ab 45 d2             CALL sub_05d2
02d9: ab 46 d5             CALL sub_06d5
02dc: ab 46 b7   lbl_02dc: CALL sub_06b7
02df:    be c0             SKF [PORT0.0]
02e1:    50 bd             BRCB lbl_00bd
02e3:       f8             BR lbl_02dc

    ; FUNCTION sub_02e4 CALLS
        ; sub_0580,
        ; sub_05ab,
        ; sub_06b7
02e4:    99 11   sub_02e4: SEL MB1
02e6:    8b 00             MOV HL, #0
02e8:    9a ff             MOV B, #F
02ea:    9a 7e   lbl_02ea: MOV C, #7
02ec: ab 46 b7   lbl_02ec: CALL sub_06b7
02ef:    9d 90             SET1 [MBE]
02f1:    aa 18             MOV XA, @HL
02f3:    9c 90             CLR1 [MBE]
02f5:       4f             PUSH BC
02f6: ab 45 80             CALL sub_0580
02f9:       4e             POP BC
02fa:       c2             INCS L
02fb:       c2             INCS L
02fc:       60             NOP
02fd:       ce             DECS C
02fe:    52 ec             BRCB lbl_02ec
0300: ab 45 ab             CALL sub_05ab
0303:       c3             INCS H
0304:       60             NOP
0305:       cf             DECS B
0306:    52 ea             BRCB lbl_02ea
0308:       ee             RET
0309: ab 43 f9   lbl_0309: CALL sub_03f9
030c:    50 bd             BRCB lbl_00bd
030e:    89 64             MOV XA, #64
0310:    92 3a             MOV [?3A], XA
0312: ab 46 b1   lbl_0312: CALL sub_06b1
0315:    b7 2d             SKT [?2D.3]
0317:       fa             BR lbl_0312
0318:    9d 90             SET1 [MBE]
031a:    99 10             SEL MB0
031c:    8b e0             MOV HL, #E0
031e:    a2 10             MOV XA, [?10]
0320: ab 4e ff             CALL sub_0eff
0323:       8a             INCS HL
0324:       8a             INCS HL
0325:    a2 12             MOV XA, [?12]
0327: ab 4e ff             CALL sub_0eff
032a:       8a             INCS HL
032b:       8a             INCS HL
032c:    a2 14             MOV XA, [?14]
032e: ab 4e ff             CALL sub_0eff
0331:       8a             INCS HL
0332:       8a             INCS HL
0333:    a2 16             MOV XA, [?16]
0335: ab 4e ff             CALL sub_0eff
0338:       8a             INCS HL
0339:       8a             INCS HL
033a:    a2 18             MOV XA, [?18]
033c: ab 4e ff             CALL sub_0eff
033f:       8a             INCS HL
0340:       8a             INCS HL
0341:    a2 1a             MOV XA, [?1A]
0343: ab 4e ff             CALL sub_0eff
0346:       8a             INCS HL
0347:       8a             INCS HL
0348:    a2 1c             MOV XA, [?1C]
034a: ab 4e ff             CALL sub_0eff
034d:    9c 90             CLR1 [MBE]
034f:    9c b2             DI
0351: ab 4c 90             CALL sub_0c90
0354:    9d 90             SET1 [MBE]
0356:    8b e0             MOV HL, #E0
0358: ab 4c 66             CALL sub_0c66
035b: ab 4c 98             CALL sub_0c98
035e:    9c 90             CLR1 [MBE]
0360:    9c 88             CLR1 [IRQBT]
0362:    b5 85             SET1 [BTM.3/?85.3]
0364:    9d b2             EI
0366:    53 09             BRCB lbl_0309
0368: ab 46 bc   lbl_0368: CALL sub_06bc
036b:    99 10             SEL MB0
036d:    8b 00             MOV HL, #0
036f:    9a 7e   lbl_036f: MOV C, #7
0371: ab 46 b7   lbl_0371: CALL sub_06b7
0374:    9d 90             SET1 [MBE]
0376:    aa 18             MOV XA, @HL
0378:    9c 90             CLR1 [MBE]
037a:       4f             PUSH BC
037b: ab 45 98             CALL sub_0598
037e:       4e             POP BC
037f:       c2             INCS L
0380:       c2             INCS L
0381:       60             NOP
0382:       ce             DECS C
0383:    53 71             BRCB lbl_0371
0385: ab 45 ab             CALL sub_05ab
0388:       c3             INCS H
0389:    53 6f             BRCB lbl_036f
038b: ab 45 d2             CALL sub_05d2
038e: ab 46 d5             CALL sub_06d5
0391: ab 46 b7   lbl_0391: CALL sub_06b7
0394:    be c0             SKF [PORT0.0]
0396:    50 bd             BRCB lbl_00bd
0398:       f8             BR lbl_0391

    ; FUNCTION sub_0399 CALLS
        ; sub_03f9,
        ; sub_06b1,
        ; sub_0eff
0399:    9d 90   sub_0399: SET1 [MBE]
039b:    99 10             SEL MB0
039d:    9a ff             MOV B, #F
039f:    8b 00             MOV HL, #0
03a1: ab 43 f9   lbl_03a1: CALL sub_03f9
03a4:    53 ee             BRCB lbl_03ee
03a6:    89 64             MOV XA, #64
03a8:    92 3a             MOV [?3A], XA
03aa: ab 46 b1   lbl_03aa: CALL sub_06b1
03ad:    b7 2d             SKT [?2D.3]
03af:       fa             BR lbl_03aa
03b0:    85 2f             SET1 [?2F.0]
03b2:    95 2f             SET1 [?2F.1]
03b4:    9a 0a             MOV L, #0
03b6:    a2 10             MOV XA, [?10]
03b8: ab 4e ff             CALL sub_0eff
03bb:       c2             INCS L
03bc:       c2             INCS L
03bd:    a2 12             MOV XA, [?12]
03bf: ab 4e ff             CALL sub_0eff
03c2:       c2             INCS L
03c3:       c2             INCS L
03c4:    a2 14             MOV XA, [?14]
03c6: ab 4e ff             CALL sub_0eff
03c9:       c2             INCS L
03ca:       c2             INCS L
03cb:    a2 16             MOV XA, [?16]
03cd: ab 4e ff             CALL sub_0eff
03d0:       c2             INCS L
03d1:       c2             INCS L
03d2:    a2 18             MOV XA, [?18]
03d4: ab 4e ff             CALL sub_0eff
03d7:       c2             INCS L
03d8:       c2             INCS L
03d9:    a2 1a             MOV XA, [?1A]
03db: ab 4e ff             CALL sub_0eff
03de:       c3             INCS H
03df:       60             NOP
03e0:       cf             DECS B
03e1:    53 a1             BRCB lbl_03a1
03e3:    9c 90             CLR1 [MBE]
03e5:    84 2f             CLR1 [?2F.0]
03e7:    94 2f             CLR1 [?2F.1]
03e9:    a3 2f             MOV A, [?2F]
03eb:    93 f3             OUT PORT3, A
03ed:       e0             RETS
03ee:    9c 90   lbl_03ee: CLR1 [MBE]
03f0:    84 2f             CLR1 [?2F.0]
03f2:    94 2f             CLR1 [?2F.1]
03f4:    a3 2f             MOV A, [?2F]
03f6:    93 f3             OUT PORT3, A
03f8:       ee             RET

    ; FUNCTION sub_03f9 CALLS
        ; sub_06b1
03f9:       70   sub_03f9: MOV A, #0
03fa:    93 09             MOV [?09], A
03fc:       74             MOV A, #4
03fd:    93 08             MOV [?08], A
03ff: ab 46 b1   lbl_03ff: CALL sub_06b1
0402:    bf c0             SKT [PORT0.0]
0404:       0e             BR lbl_0413
0405:    bf c0             SKT [PORT0.0]
0407:       0b             BR lbl_0413
0408:    bf c0             SKT [PORT0.0]
040a:       08             BR lbl_0413
040b:    bf c0             SKT [PORT0.0]
040d:       05             BR lbl_0413
040e:    89 00             MOV XA, #0
0410:    92 08             MOV [?08], XA
0412:       ee             RET
0413:    a6 08   lbl_0413: SKF [?08.2]
0415:    53 ff             BRCB lbl_03ff
0417:       e0             RETS

    ; FUNCTION sub_0418 CALLS
        ; sub_04cb
0418:    89 20   sub_0418: MOV XA, #20
041a:    92 42             MOV [?42], XA
041c:    9d 90             SET1 [MBE]
041e:    8b 00             MOV HL, #0
0420:    9a 6f             MOV B, #6
0422:       4b   lbl_0422: PUSH HL
0423:    99 11             SEL MB1
0425:       e1             MOV A, @HL
0426:    99 71             MOV X, A
0428:       c2             INCS L
0429:       e1             MOV A, @HL
042a:    9a f0             SKE A, #F
042c:       05             BR lbl_0432
042d:    9a f1             SKE X, #F
042f:       02             BR lbl_0432
0430:    54 c3             BRCB lbl_04c3
0432:       c2   lbl_0432: INCS L
0433:    aa 18             MOV XA, @HL
0435:    99 10             SEL MB0
0437:    92 44             MOV [?44], XA
0439:    99 11             SEL MB1
043b:       c2             INCS L
043c:       c2             INCS L
043d:    aa 18             MOV XA, @HL
043f:       c2             INCS L
0440:       c2             INCS L
0441:    99 10             SEL MB0
0443:    92 80             MOV [SP/?80], XA
0445:    9a 5e             MOV C, #5
0447:    89 10             MOV XA, #10
0449:    99 10   lbl_0449: SEL MB0
044b:    92 28             MOV [?28], XA
044d:    aa 44             XCH XA, DE
044f:       e4             MOV A, @DE
0450:       c4             INCS E
0451:    99 71             MOV X, A
0453:       e4             MOV A, @DE
0454:       d9             XCH A, X
0455: ab 44 cb             CALL sub_04cb
0458:    54 c3             BRCB lbl_04c3
045a:       8a             INCS HL
045b:       8a             INCS HL
045c:       8a             INCS HL
045d:       8a             INCS HL
045e:    a2 28             MOV XA, [?28]
0460:       c0             INCS A
0461:       c0             INCS A
0462:       60             NOP
0463:       ce             DECS C
0464:    54 49             BRCB lbl_0449
0466:    87 80             SKT [SP.0/?80.0]
0468:       0c             BR lbl_0475
0469:    a2 1c             MOV XA, [?1C]
046b:    aa 54             MOV DE, XA
046d:    89 2c             MOV XA, #2C
046f:       e6             CLR1 CY
0470:    aa fc             SUBC XA, DE
0472:       d7             SKT CY
0473:    54 c3             BRCB lbl_04c3
0475:    97 80   lbl_0475: SKT [SP.1/?80.1]
0477:       06             BR lbl_047e
0478:    a3 1e             MOV A, [?1E]
047a:    9a 00             SKE A, #0
047c:    54 c3             BRCB lbl_04c3
047e:    a7 80   lbl_047e: SKT [SP.2/?80.2]
0480:       06             BR lbl_0487
0481:    a3 1f             MOV A, [?1F]
0483:    9a f0             SKE A, #F
0485:    54 c3             BRCB lbl_04c3
0487:    b7 81   lbl_0487: SKT [?81.3]
0489:    54 b0             BRCB lbl_04b0
048b:    be c0             SKF [PORT0.0]
048d:       08             BR lbl_0496
048e:    be c0             SKF [PORT0.0]
0490:       05             BR lbl_0496
0491:    be c0             SKF [PORT0.0]
0493:       02             BR lbl_0496
0494:    54 b0             BRCB lbl_04b0
0496:    be c1   lbl_0496: SKF [PORT1.0]
0498:       0a             BR lbl_04a3
0499:    be c1             SKF [PORT1.0]
049b:       fa             BR lbl_0496
049c:    be c1             SKF [PORT1.0]
049e:       f7             BR lbl_0496
049f:    be c1             SKF [PORT1.0]
04a1:       f4             BR lbl_0496
04a2:       0d             BR lbl_04b0
04a3:    bf c1   lbl_04a3: SKT [PORT1.0]
04a5:       f0             BR lbl_0496
04a6:    bf c1             SKT [PORT1.0]
04a8:    54 96             BRCB lbl_0496
04aa:    bf c1             SKT [PORT1.0]
04ac:    54 96             BRCB lbl_0496
04ae:    54 c3             BRCB lbl_04c3
04b0:       4a   lbl_04b0: POP HL
04b1:    99 7b             MOV A, H
04b3:       e6             CLR1 CY
04b4:       98             RORC A
04b5:    9a 39             MOV X, #3
04b7:       c0             INCS A
04b8:    92 42             MOV [?42], XA
04ba:    99 11             SEL MB1
04bc:    aa 18             MOV XA, @HL
04be:    99 10             SEL MB0
04c0:    92 34             MOV [?34], XA
04c2:       e0             RETS
04c3:       4a   lbl_04c3: POP HL
04c4:       c3             INCS H
04c5:       c3             INCS H
04c6:       60             NOP
04c7:       cf             DECS B
04c8:    54 22             BRCB lbl_0422
04ca:       ee             RET

    ; FUNCTION sub_04cb
04cb:       4b   sub_04cb: PUSH HL
04cc:    92 26             MOV [?26], XA
04ce:    aa 54             MOV DE, XA
04d0:    99 11             SEL MB1
04d2:    aa 18             MOV XA, @HL
04d4:    99 10             SEL MB0
04d6:       e6             CLR1 CY
04d7:    aa fc             SUBC XA, DE
04d9:       d6             NOT1 CY
04da:       d7             SKT CY
04db:    54 f1             BRCB lbl_04f1
04dd:       8a             INCS HL
04de:       8a             INCS HL
04df:    99 11             SEL MB1
04e1:    aa 18             MOV XA, @HL
04e3:    99 10             SEL MB0
04e5:    aa 54             MOV DE, XA
04e7:    a2 26             MOV XA, [?26]
04e9:       e6             CLR1 CY
04ea:    aa fc             SUBC XA, DE
04ec:       d6             NOT1 CY
04ed:       d7             SKT CY
04ee:       02             BR lbl_04f1
04ef:       4a             POP HL
04f0:       e0             RETS
04f1:       4a   lbl_04f1: POP HL
04f2:       ee             RET
04f3:    8b 44             MOV HL, #44 ; DEAD
04f5:    a3 30             MOV A, [?30] ; DEAD
04f7:       90             AND A, @HL ; DEAD
04f8:    9a 00             SKE A, #0 ; DEAD
04fa:       ee             RET ; DEAD
04fb:       e0             RETS ; DEAD

    ; FUNCTION sub_04fc
04fc:    8d 50   sub_04fc: MOV DE, #50
04fe:    9a ff             MOV B, #F
0500:    9d 90             SET1 [MBE]
0502:    99 10   lbl_0502: SEL MB0
0504:       e4             MOV A, @DE
0505:    99 11             SEL MB1
0507:       e8             MOV @HL, A
0508:       c2             INCS L
0509:       c4             INCS E
050a:       01             BR lbl_050c
050b:       c5             INCS D
050c:       cf   lbl_050c: DECS B
050d:       f4             BR lbl_0502
050e:       ee             RET

    ; FUNCTION sub_050f CALLS
        ; sub_0d3b,
        ; sub_0d2e,
        ; sub_050f
050f:    9c 90   sub_050f: CLR1 [MBE]
0511:    b5 85             SET1 [BTM.3/?85.3]
0513:    9a 39   lbl_0513: MOV X, #3
0515: ab 4d 2e             CALL sub_0d2e
0518: ab 4d 3b             CALL sub_0d3b
051b:    8d a7             MOV DE, #A7
051d:       e6             CLR1 CY
051e:    aa fc             SUBC XA, DE
0520:       d6             NOT1 CY
0521:       d7             SKT CY
0522:       f0             BR lbl_0513
0523:    9a 0e             MOV C, #0
0525:    9a 09             MOV X, #0
0527: ab 4d 2e             CALL sub_0d2e
052a: ab 4d 3b             CALL sub_0d3b
052d:    8d c3             MOV DE, #C3
052f:       e6             CLR1 CY
0530:    aa fc             SUBC XA, DE
0532:       d7             SKT CY
0533:       06             BR lbl_053a
0534:    99 7e             MOV A, C
0536:    99 41             OR A, #1
0538:    99 76             MOV C, A
053a:    9a 19   lbl_053a: MOV X, #1
053c: ab 4d 2e             CALL sub_0d2e
053f: ab 4d 3b             CALL sub_0d3b
0542:    8d c3             MOV DE, #C3
0544:       e6             CLR1 CY
0545:    aa fc             SUBC XA, DE
0547:       d7             SKT CY
0548:       06             BR lbl_054f
0549:    99 7e             MOV A, C
054b:    99 42             OR A, #2
054d:    99 76             MOV C, A
054f:    bf f1   lbl_054f: SKT [PORT1.3]
0551:       0c             BR lbl_055e
0552:    bf f1             SKT [PORT1.3]
0554:       09             BR lbl_055e
0555:    bf f1             SKT [PORT1.3]
0557:       06             BR lbl_055e
0558:    99 7e             MOV A, C
055a:    99 44             OR A, #4
055c:    99 76             MOV C, A
055e:       ce   lbl_055e: DECS C
055f:       01             BR lbl_0561
0560:       ee             RET
0561:    55 0f   lbl_0561: BRCB sub_050f

    ; FUNCTION sub_0563 CALLS
        ; sub_0bd8,
        ; sub_0563,
        ; sub_0c06
0563:    9c 90   sub_0563: CLR1 [MBE]
0565: ab 4b d8             CALL sub_0bd8
0568: ab 4c 06             CALL sub_0c06
056b:       d7             SKT CY
056c:       01             BR lbl_056e
056d:       f5             BR sub_0563
056e:    9c 90   lbl_056e: CLR1 [MBE]
0570:    89 ff             MOV XA, #FF
0572:    92 2c             MOV [?2C], XA
0574:       ee             RET
0575:    92 3c             MOV [?3C], XA ; DEAD
0577:    8b 3d             MOV HL, #3D ; DEAD

    ; DEAD BLOCK lbl_0579
0579:    a3 3c   lbl_0579: MOV A, [?3C]
057b:       a0             OR A, @HL
057c:    9a 00             SKE A, #0
057e:       fa             BR lbl_0579
057f:       ee             RET

    ; FUNCTION sub_0580 CALLS
        ; sub_05c1,
        ; sub_06b1,
        ; sub_05d6,
        ; sub_05b7
0580:    aa 46   sub_0580: XCH XA, BC
0582:    89 20             MOV XA, #20
0584: ab 45 b7             CALL sub_05b7
0587:    99 7f             MOV A, B
0589: ab 45 d6             CALL sub_05d6
058c: ab 45 b7             CALL sub_05b7
058f:    99 7e             MOV A, C
0591: ab 45 d6             CALL sub_05d6
0594: ab 45 b7             CALL sub_05b7
0597:       ee             RET

    ; FUNCTION sub_0598 CALLS
        ; sub_05c1,
        ; sub_06b1,
        ; sub_05d6,
        ; sub_05b7
0598:    aa 46   sub_0598: XCH XA, BC
059a:    99 7e             MOV A, C
059c: ab 45 d6             CALL sub_05d6
059f: ab 45 b7             CALL sub_05b7
05a2:    99 7f             MOV A, B
05a4: ab 45 d6             CALL sub_05d6
05a7: ab 45 b7             CALL sub_05b7
05aa:       ee             RET

    ; FUNCTION sub_05ab CALLS
        ; sub_05c1,
        ; sub_06b1,
        ; sub_05b7
05ab:    89 0d   sub_05ab: MOV XA, #D
05ad: ab 45 b7             CALL sub_05b7
05b0:       ee             RET

    ; FUNCTION sub_05b1 CALLS
        ; sub_05c1,
        ; sub_06b1,
        ; sub_05b7
05b1:    89 20   sub_05b1: MOV XA, #20
05b3: ab 45 b7             CALL sub_05b7
05b6:       ee             RET

    ; FUNCTION sub_05b7 CALLS
        ; sub_05c1,
        ; sub_06b1
05b7:    aa 54   sub_05b7: MOV DE, XA
05b9:       e6             CLR1 CY
05ba:    aa dc             ADDC XA, DE
05bc: ab 45 c1             CALL sub_05c1
05bf:    89 ff             MOV XA, #FF

    ; FUNCTION sub_05c1 CALLS
        ; sub_05c1,
        ; sub_06b1
05c1:    86 4c   sub_05c1: SKF [?4C.0]
05c3:       fd             BR sub_05c1
05c4:    85 4c             SET1 [?4C.0]
05c6:    92 e4             MOV [SIO/?E4], XA
05c8:    56 b1             BRCB sub_06b1
05ca:       ee             RET ; DEAD

    ; FUNCTION INTCSI
05cb:    84 4c     INTCSI: CLR1 [?4C.0]
05cd:    9d 9d             EI IECSI
05cf:    9d b2             EI
05d1:       ef             RETI

    ; FUNCTION sub_05d2 CALLS
        ; sub_05d2
05d2:    86 4c   sub_05d2: SKF [?4C.0]
05d4:       fd             BR sub_05d2
05d5:       ee             RET

    ; FUNCTION sub_05d6
05d6:    99 71   sub_05d6: MOV X, A
05d8:       66             ADDS A, #6
05d9:       04             BR lbl_05de
05da:       c0             INCS A
05db:    9a 49             MOV X, #4
05dd:       ee             RET
05de:    99 79   lbl_05de: MOV A, X
05e0:    9a 39             MOV X, #3
05e2:       ee             RET

    ; FUNCTION INTT0 CALLS
        ; sub_0661,
        ; sub_0741,
        ; sub_0b64,
        ; sub_06ec,
        ; sub_082f,
        ; sub_0653,
        ; sub_0817,
        ; sub_0774,
        ; sub_0b96,
        ; sub_0b77
05e3:       49      INTT0: PUSH XA
05e4:       4f             PUSH BC
05e5:       4d             PUSH DE
05e6:       4b             PUSH HL
05e7:    99 07             PUSH BS
05e9:    9c 90             CLR1 [MBE]
05eb:    89 fa             MOV XA, #FA
05ed:    92 a6             MOV [TMOD0/?A6], XA
05ef:    a3 0e             MOV A, [?0E]
05f1:    9a 50             SKE A, #5
05f3:       06             BR lbl_05fa
05f4:       70             MOV A, #0
05f5:    93 0e             MOV [?0E], A
05f7:       78             MOV A, #8
05f8:    93 85             MOV [BTM/?85], A
05fa:    a3 49   lbl_05fa: MOV A, [?49]
05fc:       c0             INCS A
05fd:       60             NOP
05fe:    93 49             MOV [?49], A
0600: ab 48 17             CALL sub_0817
0603: ab 48 2f             CALL sub_082f
0606:    86 49             SKF [?49.0]
0608:    56 1a             BRCB lbl_061a
060a: ab 4b 77             CALL sub_0b77
060d: ab 46 ec             CALL sub_06ec
0610: ab 46 61             CALL sub_0661
0613: ab 46 53             CALL sub_0653
0616: ab 4b 64             CALL sub_0b64
0619:       0d             BR lbl_0627
061a:    96 49   lbl_061a: SKF [?49.1]
061c:       04             BR lbl_0621
061d: ab 4b 96             CALL sub_0b96
0620:       06             BR lbl_0627
0621: ab 47 41   lbl_0621: CALL sub_0741
0624: ab 47 74             CALL sub_0774
0627:    99 06   lbl_0627: POP BS
0629:       4a             POP HL
062a:       4c             POP DE
062b:       4e             POP BC
062c:       48             POP XA
062d:    9d 9c             EI IET0
062f:    9d b2             EI
0631:       ef             RETI

    ; FUNCTION sub_0632
0632:    a3 2e   sub_0632: MOV A, [?2E]
0634:    93 f2             OUT PORT2, A
0636:    a3 2f             MOV A, [?2F]
0638:    93 f3             OUT PORT3, A
063a:    a3 f0             IN A, PORT0
063c:    93 2c             MOV [?2C], A
063e:    a3 f1             IN A, PORT1
0640:    93 2d             MOV [?2D], A
0642:    a3 f4             IN A, PORT4
0644:    99 5f             XOR A, #F
0646:    93 30             MOV [?30], A
0648:    a3 f5             IN A, PORT5
064a:    99 5f             XOR A, #F
064c:    93 31             MOV [?31], A
064e:    a3 f6             IN A, PORT6
0650:    93 32             MOV [?32], A
0652:       ee             RET

    ; FUNCTION sub_0653
0653:    b6 4b   sub_0653: SKF [?4B.3]
0655:       ee             RET
0656:    a3 31             MOV A, [?31]
0658:    99 71             MOV X, A
065a:    a3 30             MOV A, [?30]
065c:    92 3e             MOV [?3E], XA
065e:    92 60             MOV [?60], XA
0660:       ee             RET

    ; FUNCTION sub_0661
0661:    86 2c   sub_0661: SKF [?2C.0]
0663:       03             BR lbl_0667
0664:    96 2d             SKF [?2D.1]
0666:       ee             RET
0667:    8b 37   lbl_0667: MOV HL, #37
0669:    a3 36             MOV A, [?36]
066b:       a0             OR A, @HL
066c:    9a 00             SKE A, #0
066e:       05             BR lbl_0674
066f:    84 2f             CLR1 [?2F.0]
0671:    84 4a             CLR1 [?4A.0]
0673:       ee             RET
0674:    85 2f   lbl_0674: SET1 [?2F.0]
0676:    a2 36             MOV XA, [?36]
0678:       c8             DECS A
0679:       01             BR lbl_067b
067a:       c9             DECS X
067b:    92 36   lbl_067b: MOV [?36], XA
067d:       ee             RET

    ; FUNCTION sub_067e
067e:       70   sub_067e: MOV A, #0
067f:    93 b8             MOV [INTA/?B8], A
0681:    93 ba             MOV [INTC/?BA], A
0683:    93 bc             MOV [INTE/?BC], A
0685:    93 bd             MOV [INTF/?BD], A
0687:    93 be             MOV [INTG/?BE], A
0689:    93 bf             MOV [INTH/?BF], A
068b:       70             MOV A, #0
068c:    93 f2             OUT PORT2, A
068e:    93 f3             OUT PORT3, A
0690:       7f             MOV A, #F
0691:    93 f6             OUT PORT6, A
0693:    89 ff             MOV XA, #FF
0695:    92 e8             MOV [PMGA/?E8], XA
0697:    89 04             MOV XA, #4
0699:    92 ec             MOV [PMGB/?EC], XA
069b:       ee             RET

    ; FUNCTION sub_069c
069c:    89 00   sub_069c: MOV XA, #0
069e:    92 98             MOV [WM/?98], XA
06a0:    89 fa             MOV XA, #FA
06a2:    92 a6             MOV [TMOD0/?A6], XA
06a4:    89 7c             MOV XA, #7C
06a6:    92 a0             MOV [TM0/?A0], XA
06a8:       ee             RET
06a9:    9c 90             CLR1 [MBE] ; DEAD
06ab:       78             MOV A, #8 ; DEAD
06ac:    93 85             MOV [BTM/?85], A ; DEAD
06ae:    b5 8b             SET1 [WDTM.3/?8B.3] ; DEAD
06b0:       ee             RET ; DEAD

    ; FUNCTION sub_06b1
06b1:    9c 90   sub_06b1: CLR1 [MBE]
06b3:       75             MOV A, #5
06b4:    93 0e             MOV [?0E], A
06b6:       ee             RET

    ; FUNCTION sub_06b7
06b7:    9c 90   sub_06b7: CLR1 [MBE]
06b9:    b5 85             SET1 [BTM.3/?85.3]
06bb:       ee             RET

    ; FUNCTION sub_06bc
06bc:    9c b2   sub_06bc: DI
06be:    9c 98             DI IEBT
06c0:    9c 9c             DI IET0
06c2:    85 e2             SET1 [RELT/?E2.0]
06c4:    89 85             MOV XA, #85
06c6:    92 e0             MOV [CSIM/?E0], XA
06c8:    89 6c             MOV XA, #6C
06ca:    92 a6             MOV [TMOD0/?A6], XA
06cc:    89 7c             MOV XA, #7C
06ce:    92 a0             MOV [TM0/?A0], XA
06d0:    9d 9d             EI IECSI
06d2:    9d b2             EI
06d4:       ee             RET

    ; FUNCTION sub_06d5 CALLS
        ; sub_069c
06d5:    9c b2   sub_06d5: DI
06d7:    9c 9d             DI IECSI
06d9:    89 01             MOV XA, #1
06db:    92 e0             MOV [CSIM/?E0], XA
06dd: ab 46 9c             CALL sub_069c
06e0:    9c 88             CLR1 [IRQBT]
06e2:    9d 9c             EI IET0
06e4:    9d 98             EI IEBT
06e6:    9d b2             EI
06e8:       ee             RET

    ; FUNCTION sub_06e9
06e9:    a3 30   sub_06e9: MOV A, [?30]
06eb:       e0             RETS

    ; FUNCTION sub_06ec
06ec:    9d 90   sub_06ec: SET1 [MBE]
06ee:    99 10             SEL MB0
06f0:    8b 89             MOV HL, #89
06f2:    a3 88             MOV A, [?88]
06f4:       a0             OR A, @HL
06f5:    9a 00             SKE A, #0
06f7:       03             BR lbl_06fb
06f8:    9d c6             SET1 [KR0]
06fa:       ee             RET
06fb:    96 8c   lbl_06fb: SKF [?8C.1]
06fd:    57 24             BRCB lbl_0724
06ff:    86 8c             SKF [?8C.0]
0701:       07             BR lbl_0709
0702:    89 32             MOV XA, #32
0704:    92 8a             MOV [?8A], XA
0706:    85 8c             SET1 [?8C.0]
0708:       ee             RET
0709:    9c c6   lbl_0709: CLR1 [KR0]
070b:    8b 8b             MOV HL, #8B
070d:    a3 8a             MOV A, [?8A]
070f:       a0             OR A, @HL
0710:    9a 00             SKE A, #0
0712:       09             BR lbl_071c
0713:    9d c6             SET1 [KR0]
0715:    89 32             MOV XA, #32
0717:    92 8a             MOV [?8A], XA
0719:    95 8c             SET1 [?8C.1]
071b:       ee             RET
071c:    a2 8a   lbl_071c: MOV XA, [?8A]
071e:       c8             DECS A
071f:       01             BR lbl_0721
0720:       c9             DECS X
0721:    92 8a   lbl_0721: MOV [?8A], XA
0723:       ee             RET
0724:    9d c6   lbl_0724: SET1 [KR0]
0726:    8b 8b             MOV HL, #8B
0728:    a3 8a             MOV A, [?8A]
072a:       a0             OR A, @HL
072b:    9a 00             SKE A, #0
072d:       0b             BR lbl_0739
072e:    a2 88             MOV XA, [?88]
0730:       c8             DECS A
0731:       01             BR lbl_0733
0732:       c9             DECS X
0733:    92 88   lbl_0733: MOV [?88], XA
0735:       70             MOV A, #0
0736:    93 8c             MOV [?8C], A
0738:       ee             RET
0739:    a2 8a   lbl_0739: MOV XA, [?8A]
073b:       c8             DECS A
073c:       01             BR lbl_073e
073d:       c9             DECS X
073e:    92 8a   lbl_073e: MOV [?8A], XA
0740:       ee             RET

    ; FUNCTION sub_0741
0741:    9d 90   sub_0741: SET1 [MBE]
0743:    99 10             SEL MB0
0745:    bf eb             SKT [PORT11.2]
0747:       0a             BR lbl_0752
0748:       70             MOV A, #0
0749:    93 85             MOV [BTM/?85], A
074b:       77             MOV A, #7
074c:    93 86             MOV [BT/?86], A
074e:       7e             MOV A, #E
074f:    93 87             MOV [?87], A
0751:       ee             RET
0752:    8b 86   lbl_0752: MOV HL, #86
0754:    99 02             INCS @HL
0756:    57 6a             BRCB lbl_076a
0758:    8b 87             MOV HL, #87
075a:    99 02             INCS @HL
075c:       0d             BR lbl_076a
075d:       77             MOV A, #7
075e:    93 86             MOV [BT/?86], A
0760:       7e             MOV A, #E
0761:    93 87             MOV [?87], A
0763:    a3 85             MOV A, [BTM/?85]
0765:    99 5f             XOR A, #F
0767:    93 85             MOV [BTM/?85], A
0769:       ee             RET
076a:    87 85   lbl_076a: SKT [BTM.0/?85.0]
076c:       ee             RET
076d:    a3 2f             MOV A, [?2F]
076f:    99 31             AND A, #1
0771:    93 2f             MOV [?2F], A
0773:       e0             RETS

    ; FUNCTION sub_0774 CALLS
        ; sub_07f1,
        ; sub_01cc,
        ; sub_109f
0774:    a2 60   sub_0774: MOV XA, [?60]
0776:    92 64             MOV [?64], XA
0778:    8b 64             MOV HL, #64
077a:       e6             CLR1 CY
077b:    a3 3e             MOV A, [?3E]
077d: ab 41 cc             CALL sub_01cc
0780:    a3 3f             MOV A, [?3F]
0782: ab 41 cc             CALL sub_01cc
0785:    a3 f3             IN A, PORT3
0787:    99 31             AND A, #1
0789:    93 f3             OUT PORT3, A
078b:    a3 40             MOV A, [?40]
078d:    9a 00             SKE A, #0
078f:       02             BR lbl_0792
0790:    93 41             MOV [?41], A
0792:    99 76   lbl_0792: MOV C, A
0794:       c0             INCS A
0795:    9a 30             SKE A, #3
0797:       01             BR lbl_0799
0798:       70             MOV A, #0
0799:    93 40   lbl_0799: MOV [?40], A
079b:       ce             DECS C
079c:    57 b2             BRCB lbl_07b2
079e:    a3 65             MOV A, [?65]
07a0:    9a 00             SKE A, #0
07a2:       09             BR lbl_07ac
07a3:    bf eb             SKT [PORT11.2]
07a5:       06             BR lbl_07ac
07a6:    89 00             MOV XA, #0
07a8:    9a 8e             MOV C, #8
07aa:    57 d3             BRCB lbl_07d3
07ac:    93 41   lbl_07ac: MOV [?41], A
07ae:    9a 8e             MOV C, #8
07b0:    57 d0             BRCB lbl_07d0
07b2:       ce   lbl_07b2: DECS C
07b3:    57 cb             BRCB lbl_07cb
07b5:    a3 64             MOV A, [?64]
07b7:    9a 00             SKE A, #0
07b9:       0e             BR lbl_07c8
07ba:    bf eb             SKT [PORT11.2]
07bc:       0b             BR lbl_07c8
07bd:    a3 41             MOV A, [?41]
07bf:    9a 00             SKE A, #0
07c1:       05             BR lbl_07c7
07c2:    89 00             MOV XA, #0
07c4:    9a 4e             MOV C, #4
07c6:       0c             BR lbl_07d3
07c7:       70   lbl_07c7: MOV A, #0
07c8:    9a 4e   lbl_07c8: MOV C, #4
07ca:       05             BR lbl_07d0
07cb:    89 3f   lbl_07cb: MOV XA, #3F
07cd:    9a 2e             MOV C, #2
07cf:       03             BR lbl_07d3
07d0: ab 50 9f   lbl_07d0: CALL sub_109f
07d3: ab 47 f1   lbl_07d3: CALL sub_07f1
07d6:       60             NOP
07d7:       60             NOP
07d8:       60             NOP
07d9:       60             NOP
07da:       60             NOP
07db:       60             NOP
07dc:    9d f6             SET1 [KR3]
07de:       60             NOP
07df:       60             NOP
07e0:       60             NOP
07e1:       60             NOP
07e2:       60             NOP
07e3:       60             NOP
07e4:    9c f6             CLR1 [KR3]
07e6:    a3 2f             MOV A, [?2F]
07e8:    99 31             AND A, #1
07ea:    aa ae             OR XA, BC
07ec:    93 2f             MOV [?2F], A
07ee:    93 f3             OUT PORT3, A
07f0:       ee             RET

    ; FUNCTION sub_07f1 CALLS
        ; sub_07f6
07f1:       d9   sub_07f1: XCH A, X
07f2: ab 47 f6             CALL sub_07f6
07f5:       d9             XCH A, X

    ; FUNCTION sub_07f6
07f6:    9a 3f   sub_07f6: MOV B, #3
07f8:       98   lbl_07f8: RORC A
07f9:       98             RORC A
07fa:       98             RORC A
07fb:       98             RORC A
07fc:       d7             SKT CY
07fd:       03             BR lbl_0801
07fe:    9d d6             SET1 [KR1]
0800:       02             BR lbl_0803
0801:    9c d6   lbl_0801: CLR1 [KR1]
0803:       60   lbl_0803: NOP
0804:       60             NOP
0805:       60             NOP
0806:       60             NOP
0807:       60             NOP
0808:       60             NOP
0809:    9c e6             CLR1 [KR2]
080b:       60             NOP
080c:       60             NOP
080d:       60             NOP
080e:       60             NOP
080f:       60             NOP
0810:       60             NOP
0811:    9d e6             SET1 [KR2]
0813:       cf             DECS B
0814:    57 f8             BRCB lbl_07f8
0816:       ee             RET

    ; FUNCTION sub_0817 CALLS
        ; sub_0632,
        ; sub_0d3b,
        ; sub_0d2e
0817:    9a 09   sub_0817: MOV X, #0
0819: ab 4d 2e             CALL sub_0d2e
081c: ab 46 32             CALL sub_0632
081f: ab 4d 3b             CALL sub_0d3b
0822:    92 0a             MOV [?0A], XA
0824:    9a 19             MOV X, #1
0826: ab 4d 2e             CALL sub_0d2e
0829: ab 4d 3b             CALL sub_0d3b
082c:    92 0c             MOV [?0C], XA
082e:       ee             RET

    ; FUNCTION sub_082f CALLS
        ; sub_0b42,
        ; sub_0a08,
        ; sub_0a4f,
        ; sub_0a79,
        ; sub_099e,
        ; sub_0a5f
082f:    a6 08   sub_082f: SKF [?08.2]
0831:    58 64             BRCB lbl_0864
0833:    86 08             SKF [?08.0]
0835:       05             BR lbl_083b
0836:    96 08             SKF [?08.1]
0838:    58 4d             BRCB lbl_084d
083a:       ee             RET
083b: ab 4a 79   lbl_083b: CALL sub_0a79
083e:       ee             RET
083f:    84 08             CLR1 [?08.0]
0841:       70             MOV A, #0
0842:    93 09             MOV [?09], A
0844:       78             MOV A, #8
0845:    93 4d             MOV [?4D], A
0847:       74             MOV A, #4
0848:    93 4e             MOV [?4E], A
084a:    93 4f             MOV [?4F], A
084c:       ee             RET
084d: ab 4a 5f   lbl_084d: CALL sub_0a5f
0850:       04             BR lbl_0855
0851: ab 4b 42             CALL sub_0b42
0854:       ee             RET
0855:       73   lbl_0855: MOV A, #3
0856:    93 08             MOV [?08], A
0858:       70             MOV A, #0
0859:    93 09             MOV [?09], A
085b:    84 4b             CLR1 [?4B.0]
085d:    be c0             SKF [PORT0.0]
085f:       ee             RET
0860:       70             MOV A, #0
0861:    93 08             MOV [?08], A
0863:       ee             RET
0864:    86 09   lbl_0864: SKF [?09.0]
0866:    58 a2             BRCB lbl_08a2
0868: ab 4a 79             CALL sub_0a79
086b:       ee             RET
086c:       71             MOV A, #1
086d:    93 09             MOV [?09], A
086f:    85 4b             SET1 [?4B.0]
0871:    a2 0a             MOV XA, [?0A]
0873:    92 10             MOV [?10], XA
0875:    92 18             MOV [?18], XA
0877:    92 20             MOV [?20], XA
0879:    92 24             MOV [?24], XA
087b:    92 74             MOV [?74], XA
087d:    92 2a             MOV [?2A], XA
087f:    a2 0c             MOV XA, [?0C]
0881:    92 70             MOV [?70], XA
0883:    92 72             MOV [?72], XA
0885:    a2 6c             MOV XA, [?6C]
0887:    92 12             MOV [?12], XA
0889:    a2 6e             MOV XA, [?6E]
088b:    92 14             MOV [?14], XA
088d:    89 00             MOV XA, #0
088f:    92 1c             MOV [?1C], XA
0891:    93 46             MOV [?46], A
0893:    93 47             MOV [?47], A
0895:    93 1e             MOV [?1E], A
0897:    93 48             MOV [?48], A
0899:       78             MOV A, #8
089a:    93 4d             MOV [?4D], A
089c:       74             MOV A, #4
089d:    93 4e             MOV [?4E], A
089f:    93 4f             MOV [?4F], A
08a1:       ee             RET
08a2: ab 4a 5f   lbl_08a2: CALL sub_0a5f
08a5:    b7 09             SKT [?09.3]
08a7:       04             BR lbl_08ac
08a8: ab 4b 42             CALL sub_0b42
08ab:       ee             RET
08ac: ab 4b 42   lbl_08ac: CALL sub_0b42
08af:    58 d1             BRCB lbl_08d1
08b1:    a2 12             MOV XA, [?12]
08b3:    aa 54             MOV DE, XA
08b5:    a2 10             MOV XA, [?10]
08b7:       e6             CLR1 CY
08b8:    aa fc             SUBC XA, DE
08ba:       d6             NOT1 CY
08bb:       d7             SKT CY
08bc:    89 00             MOV XA, #0
08be:    92 16             MOV [?16], XA
08c0:    a2 10             MOV XA, [?10]
08c2:    aa 54             MOV DE, XA
08c4:    a2 18             MOV XA, [?18]
08c6:       e6             CLR1 CY
08c7:    aa fc             SUBC XA, DE
08c9:       d6             NOT1 CY
08ca:       d7             SKT CY
08cb:    89 00             MOV XA, #0
08cd:    92 1a             MOV [?1A], XA
08cf:    58 55             BRCB lbl_0855
08d1:    a2 14   lbl_08d1: MOV XA, [?14]
08d3:    aa 54             MOV DE, XA
08d5:    a2 0c             MOV XA, [?0C]
08d7:       e6             CLR1 CY
08d8:    aa fc             SUBC XA, DE
08da:       d7             SKT CY
08db:       04             BR lbl_08e0
08dc:    a2 0c             MOV XA, [?0C]
08de:    92 14             MOV [?14], XA
08e0:    97 46   lbl_08e0: SKT [?46.1]
08e2:    58 f5             BRCB lbl_08f5
08e4:    a2 12             MOV XA, [?12]
08e6:    aa 54             MOV DE, XA
08e8:    a2 0a             MOV XA, [?0A]
08ea:       e6             CLR1 CY
08eb:    aa fc             SUBC XA, DE
08ed:       d7             SKT CY
08ee:    59 04             BRCB lbl_0904
08f0:    a2 0a             MOV XA, [?0A]
08f2:    92 12             MOV [?12], XA
08f4:       0f             BR lbl_0904
08f5:    a2 0a   lbl_08f5: MOV XA, [?0A]
08f7:    aa 54             MOV DE, XA
08f9:    a2 18             MOV XA, [?18]
08fb:       e6             CLR1 CY
08fc:    aa fc             SUBC XA, DE
08fe:       d7             SKT CY
08ff:       04             BR lbl_0904
0900:    a2 0a             MOV XA, [?0A]
0902:    92 18             MOV [?18], XA
0904: ab 49 9e   lbl_0904: CALL sub_099e
0907: ab 4a 08             CALL sub_0a08
090a: ab 4a 4f             CALL sub_0a4f
090d:    a2 0a             MOV XA, [?0A]
090f:    aa 54             MOV DE, XA
0911:    a2 20             MOV XA, [?20]
0913:       e6             CLR1 CY
0914:    aa fc             SUBC XA, DE
0916:       d7             SKT CY
0917:       08             BR lbl_0920
0918:    a2 0a             MOV XA, [?0A]
091a:    92 20             MOV [?20], XA
091c:    94 09             CLR1 [?09.1]
091e:    59 38             BRCB lbl_0938
0920:    96 09   lbl_0920: SKF [?09.1]
0922:    59 38             BRCB lbl_0938
0924:       68             ADDS A, #8
0925:    59 38             BRCB lbl_0938
0927:    95 09             SET1 [?09.1]
0929:    a2 20             MOV XA, [?20]
092b:    92 24             MOV [?24], XA
092d:    82 48             INCS [?48]
092f:       60             NOP
0930:    a3 48             MOV A, [?48]
0932:    9a 30             SKE A, #3
0934:       03             BR lbl_0938
0935:    b5 09             SET1 [?09.3]
0937:       ee             RET
0938:    a2 0c   lbl_0938: MOV XA, [?0C]
093a:    aa 54             MOV DE, XA
093c:    a2 70             MOV XA, [?70]
093e:       e6             CLR1 CY
093f:    aa fc             SUBC XA, DE
0941:       d7             SKT CY
0942:    59 70             BRCB lbl_0970
0944:    86 46             SKF [?46.0]
0946:    59 60             BRCB lbl_0960
0948:    a2 70             MOV XA, [?70]
094a:    aa 54             MOV DE, XA
094c:    a2 0c             MOV XA, [?0C]
094e:       e6             CLR1 CY
094f:    aa fc             SUBC XA, DE
0951:    8d 19             MOV DE, #19
0953:       e6             CLR1 CY
0954:    aa fc             SUBC XA, DE
0956:       d6             NOT1 CY
0957:       d7             SKT CY
0958:       ee             RET
0959:    85 46             SET1 [?46.0]
095b:    a2 0c             MOV XA, [?0C]
095d:    92 70             MOV [?70], XA
095f:       ee             RET
0960:    a2 0c   lbl_0960: MOV XA, [?0C]
0962:    aa 54             MOV DE, XA
0964:    a2 70             MOV XA, [?70]
0966:       e6             CLR1 CY
0967:    aa fc             SUBC XA, DE
0969:       d7             SKT CY
096a:       ee             RET
096b:    a2 0c             MOV XA, [?0C]
096d:    92 70             MOV [?70], XA
096f:       ee             RET
0970:    87 46   lbl_0970: SKT [?46.0]
0972:    59 8e             BRCB lbl_098e
0974:    a2 0c             MOV XA, [?0C]
0976:    aa 54             MOV DE, XA
0978:    a2 70             MOV XA, [?70]
097a:       e6             CLR1 CY
097b:    aa fc             SUBC XA, DE
097d:    8d 19             MOV DE, #19
097f:       e6             CLR1 CY
0980:    aa fc             SUBC XA, DE
0982:       d6             NOT1 CY
0983:       d7             SKT CY
0984:       ee             RET
0985:    84 46             CLR1 [?46.0]
0987:    b5 09             SET1 [?09.3]
0989:    a2 0c             MOV XA, [?0C]
098b:    92 70             MOV [?70], XA
098d:       ee             RET
098e:    a2 70   lbl_098e: MOV XA, [?70]
0990:    aa 54             MOV DE, XA
0992:    a2 0c             MOV XA, [?0C]
0994:       e6             CLR1 CY
0995:    aa fc             SUBC XA, DE
0997:       d7             SKT CY
0998:       ee             RET
0999:    a2 0c             MOV XA, [?0C]
099b:    92 70             MOV [?70], XA
099d:       ee             RET

    ; FUNCTION sub_099e
099e:    a2 0c   sub_099e: MOV XA, [?0C]
09a0:    aa 54             MOV DE, XA
09a2:    a2 22             MOV XA, [?22]
09a4:       e6             CLR1 CY
09a5:    aa fc             SUBC XA, DE
09a7:       d7             SKT CY
09a8:    59 c1             BRCB lbl_09c1
09aa:    a6 46             SKF [?46.2]
09ac:       0f             BR lbl_09bc
09ad:    8d f8             MOV DE, #F8
09af:       e6             CLR1 CY
09b0:    aa fc             SUBC XA, DE
09b2:       d7             SKT CY
09b3:       ee             RET
09b4:    a5 46             SET1 [?46.2]
09b6:    95 46             SET1 [?46.1]
09b8:    a2 6c             MOV XA, [?6C]
09ba:    92 12             MOV [?12], XA
09bc:    a2 0c   lbl_09bc: MOV XA, [?0C]
09be:    92 22             MOV [?22], XA
09c0:       ee             RET
09c1:    a7 46   lbl_09c1: SKT [?46.2]
09c3:       0c             BR lbl_09d0
09c4:    8d 08             MOV DE, #8
09c6:       e6             CLR1 CY
09c7:    aa fc             SUBC XA, DE
09c9:       d6             NOT1 CY
09ca:       d7             SKT CY
09cb:       ee             RET
09cc:    a4 46             CLR1 [?46.2]
09ce:    94 46             CLR1 [?46.1]
09d0:    a2 0c   lbl_09d0: MOV XA, [?0C]
09d2:    92 22             MOV [?22], XA
09d4:       ee             RET
09d5:    a2 0a             MOV XA, [?0A] ; DEAD
09d7:    aa 54             MOV DE, XA ; DEAD
09d9:    a2 2a             MOV XA, [?2A] ; DEAD
09db:       e6             CLR1 CY ; DEAD
09dc:    aa fc             SUBC XA, DE ; DEAD
09de:       d7             SKT CY ; DEAD
09df:    59 f2             BRCB lbl_09f2 ; DEAD
09e1:    b6 46             SKF [?46.3] ; DEAD
09e3:       09             BR lbl_09ed ; DEAD
09e4:    8d f8             MOV DE, #F8 ; DEAD
09e6:       e6             CLR1 CY ; DEAD
09e7:    aa fc             SUBC XA, DE ; DEAD
09e9:       d7             SKT CY ; DEAD
09ea:       ee             RET ; DEAD
09eb:    b5 46             SET1 [?46.3] ; DEAD

    ; DEAD BLOCK lbl_09ed
09ed:    a2 0a   lbl_09ed: MOV XA, [?0A]
09ef:    92 2a             MOV [?2A], XA
09f1:       ee             RET

    ; DEAD BLOCK lbl_09f2
09f2:    b7 46   lbl_09f2: SKT [?46.3]
09f4:       0e             BR lbl_0a03
09f5:    8d 08             MOV DE, #8
09f7:       e6             CLR1 CY
09f8:    aa fc             SUBC XA, DE
09fa:       d6             NOT1 CY
09fb:       d7             SKT CY
09fc:       ee             RET
09fd:    b4 46             CLR1 [?46.3]
09ff:    a2 2a             MOV XA, [?2A]
0a01:    92 18             MOV [?18], XA

    ; DEAD BLOCK lbl_0a03
0a03:    a2 0a   lbl_0a03: MOV XA, [?0A]
0a05:    92 2a             MOV [?2A], XA
0a07:       ee             RET

    ; FUNCTION sub_0a08
0a08:    a2 0c   sub_0a08: MOV XA, [?0C]
0a0a:    aa 54             MOV DE, XA
0a0c:    a2 72             MOV XA, [?72]
0a0e:       e6             CLR1 CY
0a0f:    aa fc             SUBC XA, DE
0a11:       d7             SKT CY
0a12:    5a 30             BRCB lbl_0a30
0a14:    a6 47             SKF [?47.2]
0a16:    5a 2b             BRCB lbl_0a2b
0a18:    a2 72             MOV XA, [?72]
0a1a:    aa 54             MOV DE, XA
0a1c:    a2 0c             MOV XA, [?0C]
0a1e:       e6             CLR1 CY
0a1f:    aa fc             SUBC XA, DE
0a21:    8d 02             MOV DE, #2
0a23:       e6             CLR1 CY
0a24:    aa fc             SUBC XA, DE
0a26:       d6             NOT1 CY
0a27:       d7             SKT CY
0a28:       ee             RET
0a29:    a5 47             SET1 [?47.2]
0a2b:    a2 0c   lbl_0a2b: MOV XA, [?0C]
0a2d:    92 72             MOV [?72], XA
0a2f:       ee             RET
0a30:    a7 47   lbl_0a30: SKT [?47.2]
0a32:    5a 4a             BRCB lbl_0a4a
0a34:    a2 0c             MOV XA, [?0C]
0a36:    aa 54             MOV DE, XA
0a38:    a2 72             MOV XA, [?72]
0a3a:       e6             CLR1 CY
0a3b:    aa fc             SUBC XA, DE
0a3d:    8d 02             MOV DE, #2
0a3f:       e6             CLR1 CY
0a40:    aa fc             SUBC XA, DE
0a42:       d6             NOT1 CY
0a43:       d7             SKT CY
0a44:       ee             RET
0a45:    a4 47             CLR1 [?47.2]
0a47:       7f             MOV A, #F
0a48:    93 1e             MOV [?1E], A
0a4a:    a2 0c   lbl_0a4a: MOV XA, [?0C]
0a4c:    92 72             MOV [?72], XA
0a4e:       ee             RET

    ; FUNCTION sub_0a4f CALLS
        ; sub_0f6c
0a4f:    a2 1c   sub_0a4f: MOV XA, [?1C]
0a51: ab 4f 6c             CALL sub_0f6c
0a54:    9a 00             SKE A, #0
0a56:       05             BR lbl_0a5c
0a57:    9a 01             SKE X, #0
0a59:       02             BR lbl_0a5c
0a5a:    89 ff             MOV XA, #FF
0a5c:    92 1c   lbl_0a5c: MOV [?1C], XA
0a5e:       ee             RET

    ; FUNCTION sub_0a5f
0a5f:    8b 4d   sub_0a5f: MOV HL, #4D
0a61:    99 02             INCS @HL
0a63:       ee             RET
0a64:    8b 4e             MOV HL, #4E
0a66:    99 02             INCS @HL
0a68:       ee             RET
0a69:    8b 4f             MOV HL, #4F
0a6b:    99 02             INCS @HL
0a6d:       ee             RET
0a6e:       74             MOV A, #4
0a6f:    93 08             MOV [?08], A
0a71:       70             MOV A, #0
0a72:    93 09             MOV [?09], A
0a74:    93 46             MOV [?46], A
0a76:    84 4b             CLR1 [?4B.0]
0a78:       ee             RET

    ; FUNCTION sub_0a79 CALLS
        ; sub_0b10,
        ; sub_0b52
0a79:    96 09   sub_0a79: SKF [?09.1]
0a7b:    5a 94             BRCB lbl_0a94
0a7d:    a2 0c             MOV XA, [?0C]
0a7f:    92 6e             MOV [?6E], XA
0a81:    a2 0a             MOV XA, [?0A]
0a83:    92 6c             MOV [?6C], XA
0a85:    95 09             SET1 [?09.1]
0a87:    a4 09             CLR1 [?09.2]
0a89:    b4 08             CLR1 [?08.3]
0a8b:       70             MOV A, #0
0a8c:    93 4d             MOV [?4D], A
0a8e:    93 4e             MOV [?4E], A
0a90:       7e             MOV A, #E
0a91:    93 4f             MOV [?4F], A
0a93:       ee             RET
0a94:    a6 09   lbl_0a94: SKF [?09.2]
0a96:    5a d6             BRCB lbl_0ad6
0a98:    a2 0a             MOV XA, [?0A]
0a9a:    aa 54             MOV DE, XA
0a9c:    a2 6c             MOV XA, [?6C]
0a9e:       e6             CLR1 CY
0a9f:    aa fc             SUBC XA, DE
0aa1:       d7             SKT CY
0aa2:       09             BR lbl_0aac
0aa3:    a2 0a             MOV XA, [?0A]
0aa5:    92 6c             MOV [?6C], XA
0aa7:    a2 0c             MOV XA, [?0C]
0aa9:    92 6e             MOV [?6E], XA
0aab:       ee             RET
0aac:    9a 01   lbl_0aac: SKE X, #0
0aae:       03             BR lbl_0ab2
0aaf:       64             ADDS A, #4
0ab0:    5a c4             BRCB lbl_0ac4
0ab2:    a5 09   lbl_0ab2: SET1 [?09.2]
0ab4:    b5 08             SET1 [?08.3]
0ab6:    a2 0a             MOV XA, [?0A]
0ab8:    92 2a             MOV [?2A], XA
0aba:       70             MOV A, #0
0abb:    93 1f             MOV [?1F], A
0abd:    93 46             MOV [?46], A
0abf:    93 4d             MOV [?4D], A
0ac1:    93 4e             MOV [?4E], A
0ac3:       ee             RET
0ac4:    8b 4d   lbl_0ac4: MOV HL, #4D
0ac6:    99 02             INCS @HL
0ac8:       ee             RET
0ac9:    8b 4e             MOV HL, #4E
0acb:    99 02             INCS @HL
0acd:       ee             RET
0ace:    8b 4f             MOV HL, #4F
0ad0:    99 02             INCS @HL
0ad2:       ee             RET
0ad3:    94 09             CLR1 [?09.1]
0ad5:       ee             RET
0ad6: ab 4b 52   lbl_0ad6: CALL sub_0b52
0ad9:       03             BR lbl_0add
0ada:    94 09             CLR1 [?09.1]
0adc:       ee             RET
0add:    8b 4d   lbl_0add: MOV HL, #4D
0adf:    99 02             INCS @HL
0ae1:       0a             BR lbl_0aec
0ae2:    8b 4e             MOV HL, #4E
0ae4:    99 02             INCS @HL
0ae6:       05             BR lbl_0aec
0ae7:       7f             MOV A, #F
0ae8:    93 4d             MOV [?4D], A
0aea:    93 4e             MOV [?4E], A
0aec: ab 4b 10   lbl_0aec: CALL sub_0b10
0aef:    a2 0c             MOV XA, [?0C]
0af1:    aa 54             MOV DE, XA
0af3:    a2 6e             MOV XA, [?6E]
0af5:       e6             CLR1 CY
0af6:    aa fc             SUBC XA, DE
0af8:       d7             SKT CY
0af9:       01             BR lbl_0afb
0afa:       ee             RET
0afb:    9a 01   lbl_0afb: SKE X, #0
0afd:       02             BR lbl_0b00
0afe:       64             ADDS A, #4
0aff:       ee             RET
0b00:    a3 4e   lbl_0b00: MOV A, [?4E]
0b02:    99 71             MOV X, A
0b04:    a3 4d             MOV A, [?4D]
0b06:    8d 06             MOV DE, #6
0b08:       e6             CLR1 CY
0b09:    aa fc             SUBC XA, DE
0b0b:       d7             SKT CY
0b0c:       e0             RETS
0b0d:    94 09             CLR1 [?09.1]
0b0f:       ee             RET

    ; FUNCTION sub_0b10
0b10:    a2 0a   sub_0b10: MOV XA, [?0A]
0b12:    aa 54             MOV DE, XA
0b14:    a2 2a             MOV XA, [?2A]
0b16:       e6             CLR1 CY
0b17:    aa fc             SUBC XA, DE
0b19:       d7             SKT CY
0b1a:    5b 30             BRCB lbl_0b30
0b1c:    86 46             SKF [?46.0]
0b1e:       0c             BR lbl_0b2b
0b1f:    8d fc             MOV DE, #FC
0b21:       e6             CLR1 CY
0b22:    aa fc             SUBC XA, DE
0b24:       d7             SKT CY
0b25:       ee             RET
0b26:    85 46             SET1 [?46.0]
0b28:       7f             MOV A, #F
0b29:    93 1f             MOV [?1F], A
0b2b:    a2 0a   lbl_0b2b: MOV XA, [?0A]
0b2d:    92 2a             MOV [?2A], XA
0b2f:       ee             RET
0b30:    87 46   lbl_0b30: SKT [?46.0]
0b32:       0a             BR lbl_0b3d
0b33:    8d 04             MOV DE, #4
0b35:       e6             CLR1 CY
0b36:    aa fc             SUBC XA, DE
0b38:       d6             NOT1 CY
0b39:       d7             SKT CY
0b3a:       ee             RET
0b3b:    84 46             CLR1 [?46.0]
0b3d:    a2 0a   lbl_0b3d: MOV XA, [?0A]
0b3f:    92 2a             MOV [?2A], XA
0b41:       ee             RET

    ; FUNCTION sub_0b42 CALLS
        ; sub_0b52,
        ; sub_0b4b
0b42: ab 4b 4b   sub_0b42: CALL sub_0b4b
0b45:       ee             RET
0b46: ab 4b 52             CALL sub_0b52
0b49:       ee             RET
0b4a:       e0             RETS

    ; FUNCTION sub_0b4b
0b4b:    a2 0c   sub_0b4b: MOV XA, [?0C]
0b4d:    aa 54             MOV DE, XA
0b4f:    a2 6e             MOV XA, [?6E]
0b51:       06             BR lbl_0b58

    ; FUNCTION sub_0b52
0b52:    a2 0a   sub_0b52: MOV XA, [?0A]
0b54:    aa 54             MOV DE, XA
0b56:    a2 6c             MOV XA, [?6C]
0b58:       e6   lbl_0b58: CLR1 CY
0b59:    aa fc             SUBC XA, DE
0b5b:       d6             NOT1 CY
0b5c:       d7             SKT CY
0b5d:       e0             RETS
0b5e:    9a 01             SKE X, #0
0b60:       ee             RET
0b61:       6a             ADDS A, #A
0b62:       e0             RETS
0b63:       ee             RET

    ; FUNCTION sub_0b64
0b64:    a3 3c   sub_0b64: MOV A, [?3C]
0b66:    8b 3d             MOV HL, #3D
0b68:       a0             OR A, @HL
0b69:    9a 00             SKE A, #0
0b6b:       03             BR lbl_0b6f
0b6c:    a4 4a             CLR1 [?4A.2]
0b6e:       ee             RET
0b6f:    a2 3c   lbl_0b6f: MOV XA, [?3C]
0b71:       c8             DECS A
0b72:       01             BR lbl_0b74
0b73:       c9             DECS X
0b74:    92 3c   lbl_0b74: MOV [?3C], XA
0b76:       ee             RET

    ; FUNCTION sub_0b77
0b77:    8b 3b   sub_0b77: MOV HL, #3B
0b79:    a3 3a             MOV A, [?3A]
0b7b:       a0             OR A, @HL
0b7c:    9a 00             SKE A, #0
0b7e:       09             BR lbl_0b88
0b7f:    a3 2e             MOV A, [?2E]
0b81:    99 3e             AND A, #E
0b83:    93 2e             MOV [?2E], A
0b85:    94 4a             CLR1 [?4A.1]
0b87:       ee             RET
0b88:    a3 2e   lbl_0b88: MOV A, [?2E]
0b8a:    99 41             OR A, #1
0b8c:    93 2e             MOV [?2E], A
0b8e:    a2 3a             MOV XA, [?3A]
0b90:       c8             DECS A
0b91:       01             BR lbl_0b93
0b92:       c9             DECS X
0b93:    92 3a   lbl_0b93: MOV [?3A], XA
0b95:       ee             RET

    ; FUNCTION sub_0b96 CALLS
        ; sub_0c98,
        ; sub_0ca7,
        ; sub_0ce9,
        ; sub_0d07
0b96:    87 2c   sub_0b96: SKT [?2C.0]
0b98:       ee             RET
0b99:    86 4b             SKF [?4B.0]
0b9b:       ee             RET
0b9c:    a2 38             MOV XA, [?38]
0b9e:    9a 00             SKE A, #0
0ba0:       07             BR lbl_0ba8
0ba1:    96 4b             SKF [?4B.1]
0ba3:       04             BR lbl_0ba8
0ba4:    95 4b             SET1 [?4B.1]
0ba6:    5c 98             BRCB sub_0c98
0ba8:    94 4b   lbl_0ba8: CLR1 [?4B.1]
0baa: ab 4d 07             CALL sub_0d07
0bad:    a2 38             MOV XA, [?38]
0baf:       49             PUSH XA
0bb0:       64             ADDS A, #4
0bb1:       02             BR lbl_0bb4
0bb2:       c1             INCS X
0bb3:       60             NOP
0bb4:    99 3c   lbl_0bb4: AND A, #C
0bb6:    92 38             MOV [?38], XA
0bb8:       4a             POP HL
0bb9:    9d 90             SET1 [MBE]
0bbb:       4b             PUSH HL
0bbc:    99 1f             SEL MBF
0bbe:    9a 69             MOV X, #6
0bc0: ab 4c a7             CALL sub_0ca7
0bc3: ab 4c e9             CALL sub_0ce9
0bc6:       4a             POP HL
0bc7:    a2 c0             MOV XA, [BSB0/?C0]
0bc9:    99 11             SEL MB1
0bcb:    aa 10             MOV @HL, XA
0bcd:       c2             INCS L
0bce:       c2             INCS L
0bcf:    99 1f             SEL MBF
0bd1:    a2 c2             MOV XA, [BSB2/?C2]
0bd3:    99 11             SEL MB1
0bd5:    aa 10             MOV @HL, XA
0bd7:       ee             RET

    ; FUNCTION sub_0bd8 CALLS
        ; sub_06b7,
        ; sub_0ce9,
        ; sub_0ca7,
        ; sub_0d07
0bd8: ab 4d 07   sub_0bd8: CALL sub_0d07
0bdb:    9d 90             SET1 [MBE]
0bdd:    8b 00             MOV HL, #0
0bdf: ab 46 b7   lbl_0bdf: CALL sub_06b7
0be2:       4b             PUSH HL
0be3:    99 1f             SEL MBF
0be5:    9a 69             MOV X, #6
0be7: ab 4c a7             CALL sub_0ca7
0bea: ab 4c e9             CALL sub_0ce9
0bed:       4a             POP HL
0bee:    a2 c0             MOV XA, [BSB0/?C0]
0bf0:    99 11             SEL MB1
0bf2:    aa 10             MOV @HL, XA
0bf4:       c2             INCS L
0bf5:       c2             INCS L
0bf6:    99 1f             SEL MBF
0bf8:    a2 c2             MOV XA, [BSB2/?C2]
0bfa:    99 11             SEL MB1
0bfc:    aa 10             MOV @HL, XA
0bfe:       c2             INCS L
0bff:       c2             INCS L
0c00:    5b df             BRCB lbl_0bdf
0c02:       c3             INCS H
0c03:    5b df             BRCB lbl_0bdf
0c05:       ee             RET

    ; FUNCTION sub_0c06 CALLS
        ; sub_0c20,
        ; sub_06b7,
        ; sub_0d07
0c06: ab 4d 07   sub_0c06: CALL sub_0d07
0c09:    9d 90             SET1 [MBE]
0c0b:    8b 00             MOV HL, #0
0c0d: ab 46 b7   lbl_0c0d: CALL sub_06b7
0c10: ab 4c 20             CALL sub_0c20
0c13:       d6             NOT1 CY
0c14:       d7             SKT CY
0c15:    5c 4f             BRCB lbl_0c4f
0c17:       c3             INCS H
0c18:       f4             BR lbl_0c0d
0c19:       e6             CLR1 CY
0c1a:       ee             RET
0c1b: ab 4d 07             CALL sub_0d07 ; DEAD
0c1e:    9d 90             SET1 [MBE] ; DEAD

    ; FUNCTION sub_0c20 CALLS
        ; sub_0c20,
        ; sub_0ce9,
        ; sub_0d1c,
        ; sub_0ca7
0c20:       4b   sub_0c20: PUSH HL
0c21:    99 1f             SEL MBF
0c23: ab 4d 1c             CALL sub_0d1c
0c26:    9a 69             MOV X, #6
0c28: ab 4c a7             CALL sub_0ca7
0c2b: ab 4c e9             CALL sub_0ce9
0c2e:       4a             POP HL
0c2f:    a2 c0             MOV XA, [BSB0/?C0]
0c31:    99 11             SEL MB1
0c33:       80             SKE A, @HL
0c34:    5c 4f             BRCB lbl_0c4f
0c36:       c2             INCS L
0c37:    99 79             MOV A, X
0c39:       80             SKE A, @HL
0c3a:    5c 4f             BRCB lbl_0c4f
0c3c:       c2             INCS L
0c3d:    99 1f             SEL MBF
0c3f:    a2 c2             MOV XA, [BSB2/?C2]
0c41:    99 11             SEL MB1
0c43:       80             SKE A, @HL
0c44:       0a             BR lbl_0c4f
0c45:       c2             INCS L
0c46:    99 79             MOV A, X
0c48:       80             SKE A, @HL
0c49:       05             BR lbl_0c4f
0c4a:       c2             INCS L
0c4b:    5c 20             BRCB sub_0c20
0c4d:       e6             CLR1 CY
0c4e:       ee             RET
0c4f:       e7   lbl_0c4f: SET1 CY
0c50:       ee             RET

    ; FUNCTION sub_0c51 CALLS
        ; sub_06b7,
        ; sub_0c66,
        ; sub_0d07
0c51: ab 4d 07   sub_0c51: CALL sub_0d07
0c54:    9d 90             SET1 [MBE]
0c56:    8b 00             MOV HL, #0
0c58: ab 46 b7   lbl_0c58: CALL sub_06b7
0c5b: ab 4c 66             CALL sub_0c66
0c5e:       c3             INCS H
0c5f:       f8             BR lbl_0c58
0c60:       ee             RET
0c61: ab 4d 07             CALL sub_0d07 ; DEAD
0c64:    9d 90             SET1 [MBE] ; DEAD

    ; FUNCTION sub_0c66 CALLS
        ; sub_0d1c,
        ; sub_0ccd,
        ; sub_0c66,
        ; sub_0ca7
0c66:       4b   sub_0c66: PUSH HL
0c67:    99 1f             SEL MBF
0c69: ab 4d 1c             CALL sub_0d1c
0c6c:    9a 59             MOV X, #5
0c6e: ab 4c a7             CALL sub_0ca7
0c71:       4a             POP HL
0c72:       4b             PUSH HL
0c73:    99 11             SEL MB1
0c75:    aa 18             MOV XA, @HL
0c77:    99 1f             SEL MBF
0c79:    92 c0             MOV [BSB0/?C0], XA
0c7b:       c2             INCS L
0c7c:       c2             INCS L
0c7d:    99 11             SEL MB1
0c7f:    aa 18             MOV XA, @HL
0c81:    99 1f             SEL MBF
0c83:    92 c2             MOV [BSB2/?C2], XA
0c85: ab 4c cd             CALL sub_0ccd
0c88:       4a             POP HL
0c89:       c2             INCS L
0c8a:       c2             INCS L
0c8b:       c2             INCS L
0c8c:       c2             INCS L
0c8d:    5c 66             BRCB sub_0c66
0c8f:       ee             RET

    ; FUNCTION sub_0c90 CALLS
        ; sub_0ca7,
        ; sub_0d07
0c90: ab 4d 07   sub_0c90: CALL sub_0d07
0c93:    9a fb             MOV H, #F
0c95:    9a 49             MOV X, #4
0c97:       07             BR lbl_0c9f

    ; FUNCTION sub_0c98 CALLS
        ; sub_0ca7,
        ; sub_0d07
0c98: ab 4d 07   sub_0c98: CALL sub_0d07
0c9b:    9a 0b             MOV H, #0
0c9d:    9a 49             MOV X, #4
0c9f: ab 4c a7   lbl_0c9f: CALL sub_0ca7
0ca2:    99 7c             MOV A, E
0ca4:    93 f2             OUT PORT2, A
0ca6:       ee             RET

    ; FUNCTION sub_0ca7
0ca7:    99 79   sub_0ca7: MOV A, X
0ca9:    93 c3             MOV [BSB3/?C3], A
0cab:    99 7b             MOV A, H
0cad:    93 c2             MOV [BSB2/?C2], A
0caf:    99 7a             MOV A, L
0cb1:    93 c1             MOV [BSB1/?C1], A
0cb3:    9a fa             MOV L, #F
0cb5:    bf 40   lbl_0cb5: SKT [FC0].@L
0cb7:       03             BR lbl_0cbb
0cb8:    99 7d             MOV A, D
0cba:       02             BR lbl_0cbd
0cbb:    99 7f   lbl_0cbb: MOV A, B
0cbd:    93 f2   lbl_0cbd: OUT PORT2, A
0cbf:    99 44             OR A, #4
0cc1:    93 f2             OUT PORT2, A
0cc3:    99 3b             AND A, #B
0cc5:    93 f2             OUT PORT2, A
0cc7:       ca             DECS L
0cc8:    9a 52             SKE L, #5
0cca:    5c b5             BRCB lbl_0cb5
0ccc:       ee             RET

    ; FUNCTION sub_0ccd
0ccd:    9a fa   sub_0ccd: MOV L, #F
0ccf:    bf 40   lbl_0ccf: SKT [FC0].@L
0cd1:       03             BR lbl_0cd5
0cd2:    99 7d             MOV A, D
0cd4:       02             BR lbl_0cd7
0cd5:    99 7f   lbl_0cd5: MOV A, B
0cd7:    93 f2   lbl_0cd7: OUT PORT2, A
0cd9:    99 44             OR A, #4
0cdb:    93 f2             OUT PORT2, A
0cdd:    99 3b             AND A, #B
0cdf:    93 f2             OUT PORT2, A
0ce1:       ca             DECS L
0ce2:    5c cf             BRCB lbl_0ccf
0ce4:    99 7c             MOV A, E
0ce6:    93 f2             OUT PORT2, A
0ce8:       ee             RET

    ; FUNCTION sub_0ce9
0ce9:    9a fa   sub_0ce9: MOV L, #F
0ceb:    99 7e   lbl_0ceb: MOV A, C
0ced:    93 f2             OUT PORT2, A
0cef:    99 7f             MOV A, B
0cf1:    93 f2             OUT PORT2, A
0cf3:    a3 f1             IN A, PORT1
0cf5:    99 34             AND A, #4
0cf7:    9a 00             SKE A, #0
0cf9:       03             BR lbl_0cfd
0cfa:    9c 40             CLR1 [FC0].@L
0cfc:       02             BR lbl_0cff
0cfd:    9d 40   lbl_0cfd: SET1 [FC0].@L
0cff:       ca   lbl_0cff: DECS L
0d00:    5c eb             BRCB lbl_0ceb
0d02:    99 7c             MOV A, E
0d04:    93 f2             OUT PORT2, A
0d06:       ee             RET

    ; FUNCTION sub_0d07
0d07:    a3 2e   sub_0d07: MOV A, [?2E]
0d09:    99 31             AND A, #1
0d0b:    99 74             MOV E, A
0d0d:    99 42             OR A, #2
0d0f:    99 77             MOV B, A
0d11:    99 44             OR A, #4
0d13:    99 76             MOV C, A
0d15:    99 48             OR A, #8
0d17:    99 3b             AND A, #B
0d19:    99 75             MOV D, A
0d1b:       ee             RET

    ; FUNCTION sub_0d1c
0d1c:    99 7f   sub_0d1c: MOV A, B
0d1e:    93 f2             OUT PORT2, A
0d20:       60             NOP
0d21:       60             NOP
0d22:    a3 f1   lbl_0d22: IN A, PORT1
0d24:    99 34             AND A, #4
0d26:    9a 00             SKE A, #0
0d28:       01             BR lbl_0d2a
0d29:       f8             BR lbl_0d22
0d2a:       60   lbl_0d2a: NOP
0d2b:       60             NOP
0d2c:       60             NOP
0d2d:       ee             RET

    ; FUNCTION sub_0d2e
0d2e:    9c 90   sub_0d2e: CLR1 [MBE]
0d30:       d9             XCH A, X
0d31:    99 48             OR A, #8
0d33:       d9             XCH A, X
0d34:       78             MOV A, #8
0d35:    92 d8             MOV [ADM/?D8], XA
0d37:    a6 d8   lbl_0d37: SKF [EOC/?D8.2]
0d39:       fd             BR lbl_0d37
0d3a:       ee             RET

    ; FUNCTION sub_0d3b
0d3b:    9c 90   sub_0d3b: CLR1 [MBE]
0d3d:    a7 d8   lbl_0d3d: SKT [EOC/?D8.2]
0d3f:       fd             BR lbl_0d3d
0d40:    a2 da             MOV XA, [SA/?DA]
0d42:       ee             RET

    ; FUNCTION sub_0d43 CALLS
        ; sub_1080,
        ; sub_0e21,
        ; sub_0e62,
        ; sub_06e9,
        ; sub_0d95,
        ; sub_06b7,
        ; sub_0bd8,
        ; sub_04fc
0d43: ab 46 e9   sub_0d43: CALL sub_06e9
0d46:       ee             RET
0d47:    9a 09             MOV X, #0
0d49:    aa c8             ADDS XA, XA
0d4b:    99 73             MOV H, A
0d4d:    9a 0a             MOV L, #0
0d4f:       4b             PUSH HL
0d50: ab 4d 95             CALL sub_0d95
0d53: ab 46 b7             CALL sub_06b7
0d56: ab 50 80             CALL sub_1080
0d59: ab 46 b7             CALL sub_06b7
0d5c: ab 4e 21             CALL sub_0e21
0d5f: ab 46 b7             CALL sub_06b7
0d62: ab 4b d8             CALL sub_0bd8
0d65: ab 46 b7             CALL sub_06b7
0d68:       4a             POP HL
0d69:       4b             PUSH HL
0d6a: ab 44 fc             CALL sub_04fc
0d6d: ab 4e 62             CALL sub_0e62
0d70: ab 46 b7             CALL sub_06b7
0d73:       4a             POP HL
0d74:       c3             INCS H
0d75: ab 44 fc             CALL sub_04fc
0d78:    9d 90             SET1 [MBE]
0d7a:    99 11             SEL MB1
0d7c:    a2 f0             IN XA, PORT0
0d7e:    92 f4             OUT PORT4, XA
0d80:    a2 f2             IN XA, PORT2
0d82:    92 f6             OUT PORT6, XA
0d84:    99 10             SEL MB0
0d86:    a2 6c             MOV XA, [?6C]
0d88:    99 11             SEL MB1
0d8a:    92 f0             OUT PORT0, XA
0d8c:    99 10             SEL MB0
0d8e:    a2 6e             MOV XA, [?6E]
0d90:    99 11             SEL MB1
0d92:    92 f2             OUT PORT2, XA
0d94:       e0             RETS

    ; FUNCTION sub_0d95 CALLS
        ; sub_0ef8,
        ; sub_0f48
0d95:    9d 90   sub_0d95: SET1 [MBE]
0d97:    99 10             SEL MB0
0d99:    89 00             MOV XA, #0
0d9b:    92 b0             MOV [PSW/?B0], XA
0d9d:    92 b2             MOV [IPS/?B2], XA
0d9f:    92 b4             MOV [IM0/?B4], XA
0da1:    92 b6             MOV [IM2/?B6], XA
0da3:    92 b8             MOV [INTA/?B8], XA
0da5:    92 ba             MOV [INTC/?BA], XA
0da7:    92 bc             MOV [INTE/?BC], XA
0da9:    92 be             MOV [INTG/?BE], XA
0dab:    92 c0             MOV [BSB0/?C0], XA
0dad:    92 c2             MOV [BSB2/?C2], XA
0daf:    92 c4             MOV [?C4], XA
0db1:    92 c6             MOV [?C6], XA
0db3:    92 6a             MOV [?6A], XA
0db5:    9a ff             MOV B, #F
0db7:    8b 00             MOV HL, #0
0db9:       4f   lbl_0db9: PUSH BC
0dba:    9a 0a             MOV L, #0
0dbc:       4b             PUSH HL
0dbd: ab 4e f8             CALL sub_0ef8
0dc0:    92 68             MOV [?68], XA
0dc2:    8b b0             MOV HL, #B0
0dc4:    8d 68             MOV DE, #68
0dc6: ab 4f 48             CALL sub_0f48
0dc9:       4a             POP HL
0dca:       c2             INCS L
0dcb:       c2             INCS L
0dcc:       4b             PUSH HL
0dcd: ab 4e f8             CALL sub_0ef8
0dd0:    92 68             MOV [?68], XA
0dd2:    8b b4             MOV HL, #B4
0dd4:    8d 68             MOV DE, #68
0dd6: ab 4f 48             CALL sub_0f48
0dd9:       4a             POP HL
0dda:       c2             INCS L
0ddb:       c2             INCS L
0ddc:       4b             PUSH HL
0ddd: ab 4e f8             CALL sub_0ef8
0de0:    92 68             MOV [?68], XA
0de2:    8b b8             MOV HL, #B8
0de4:    8d 68             MOV DE, #68
0de6: ab 4f 48             CALL sub_0f48
0de9:       4a             POP HL
0dea:       c2             INCS L
0deb:       c2             INCS L
0dec:       4b             PUSH HL
0ded: ab 4e f8             CALL sub_0ef8
0df0:    92 68             MOV [?68], XA
0df2:    8b bc             MOV HL, #BC
0df4:    8d 68             MOV DE, #68
0df6: ab 4f 48             CALL sub_0f48
0df9:       4a             POP HL
0dfa:       c2             INCS L
0dfb:       c2             INCS L
0dfc:       4b             PUSH HL
0dfd: ab 4e f8             CALL sub_0ef8
0e00:    92 68             MOV [?68], XA
0e02:    8b c0             MOV HL, #C0
0e04:    8d 68             MOV DE, #68
0e06: ab 4f 48             CALL sub_0f48
0e09:       4a             POP HL
0e0a:       c2             INCS L
0e0b:       c2             INCS L
0e0c:       4b             PUSH HL
0e0d: ab 4e f8             CALL sub_0ef8
0e10:    92 68             MOV [?68], XA
0e12:    8b c4             MOV HL, #C4
0e14:    8d 68             MOV DE, #68
0e16: ab 4f 48             CALL sub_0f48
0e19:       4a             POP HL
0e1a:       4e             POP BC
0e1b:       c3             INCS H
0e1c:       60             NOP
0e1d:       cf             DECS B
0e1e:    5d b9             BRCB lbl_0db9
0e20:       ee             RET

    ; FUNCTION sub_0e21 CALLS
        ; sub_0ed1,
        ; sub_0eaa,
        ; sub_1094
0e21:    9d 90   sub_0e21: SET1 [MBE]
0e23:    99 10             SEL MB0
0e25: ab 50 94             CALL sub_1094
0e28:    92 50             MOV [?50], XA
0e2a: ab 50 94             CALL sub_1094
0e2d:    92 52             MOV [?52], XA
0e2f: ab 50 94             CALL sub_1094
0e32:    92 54             MOV [?54], XA
0e34:    8b b0             MOV HL, #B0
0e36:    8d 68             MOV DE, #68
0e38: ab 4e aa             CALL sub_0eaa
0e3b:    92 56             MOV [?56], XA
0e3d:    8b b0             MOV HL, #B0
0e3f:    8d 68             MOV DE, #68
0e41: ab 4e d1             CALL sub_0ed1
0e44:    92 58             MOV [?58], XA
0e46:    8b b4             MOV HL, #B4
0e48:    8d 68             MOV DE, #68
0e4a: ab 4e aa             CALL sub_0eaa
0e4d:    92 5a             MOV [?5A], XA
0e4f:    8b b4             MOV HL, #B4
0e51:    8d 68             MOV DE, #68
0e53: ab 4e d1             CALL sub_0ed1
0e56:    92 5c             MOV [?5C], XA
0e58:    8b b8             MOV HL, #B8
0e5a:    8d 68             MOV DE, #68
0e5c: ab 4e aa             CALL sub_0eaa
0e5f:    92 5e             MOV [?5E], XA
0e61:       ee             RET

    ; FUNCTION sub_0e62 CALLS
        ; sub_0ed1,
        ; sub_0eaa
0e62:    9d 90   sub_0e62: SET1 [MBE]
0e64:    99 10             SEL MB0
0e66:    8b b8             MOV HL, #B8
0e68:    8d 68             MOV DE, #68
0e6a: ab 4e d1             CALL sub_0ed1
0e6d:    92 50             MOV [?50], XA
0e6f:    8b bc             MOV HL, #BC
0e71:    8d 68             MOV DE, #68
0e73: ab 4e aa             CALL sub_0eaa
0e76:    92 52             MOV [?52], XA
0e78:    8b bc             MOV HL, #BC
0e7a:    8d 68             MOV DE, #68
0e7c: ab 4e d1             CALL sub_0ed1
0e7f:    92 54             MOV [?54], XA
0e81:    8b c0             MOV HL, #C0
0e83:    8d 68             MOV DE, #68
0e85: ab 4e aa             CALL sub_0eaa
0e88:    92 56             MOV [?56], XA
0e8a:    8b c0             MOV HL, #C0
0e8c:    8d 68             MOV DE, #68
0e8e: ab 4e d1             CALL sub_0ed1
0e91:    92 58             MOV [?58], XA
0e93:    8b c4             MOV HL, #C4
0e95:    8d 68             MOV DE, #68
0e97: ab 4e aa             CALL sub_0eaa
0e9a:    92 5a             MOV [?5A], XA
0e9c:    8b c4             MOV HL, #C4
0e9e:    8d 68             MOV DE, #68
0ea0: ab 4e d1             CALL sub_0ed1
0ea3:    92 5c             MOV [?5C], XA
0ea5:    89 ff             MOV XA, #FF
0ea7:    92 5e             MOV [?5E], XA
0ea9:       ee             RET

    ; FUNCTION sub_0eaa CALLS
        ; sub_0f48,
        ; sub_1094,
        ; sub_0f06
0eaa: ab 4f 06   sub_0eaa: CALL sub_0f06
0ead: ab 50 94             CALL sub_1094
0eb0:    93 65             MOV [?65], A
0eb2:    99 79             MOV A, X
0eb4:    93 66             MOV [?66], A
0eb6:       70             MOV A, #0
0eb7:    93 64             MOV [?64], A
0eb9:    93 67             MOV [?67], A
0ebb:    8b 68             MOV HL, #68
0ebd:    8d 64             MOV DE, #64
0ebf: ab 4f 48             CALL sub_0f48
0ec2:    a3 6b             MOV A, [?6B]
0ec4:    9a 00             SKE A, #0
0ec6:       07             BR lbl_0ece
0ec7:    a3 6a             MOV A, [?6A]
0ec9:    99 71             MOV X, A
0ecb:    a3 69             MOV A, [?69]
0ecd:       ee             RET
0ece:    89 ff   lbl_0ece: MOV XA, #FF
0ed0:       ee             RET

    ; FUNCTION sub_0ed1 CALLS
        ; sub_1094,
        ; sub_0f06,
        ; sub_0f57
0ed1: ab 4f 06   sub_0ed1: CALL sub_0f06
0ed4: ab 50 94             CALL sub_1094
0ed7:    93 65             MOV [?65], A
0ed9:    99 79             MOV A, X
0edb:    93 66             MOV [?66], A
0edd:       70             MOV A, #0
0ede:    93 64             MOV [?64], A
0ee0:    93 67             MOV [?67], A
0ee2:    8b 68             MOV HL, #68
0ee4:    8d 64             MOV DE, #64
0ee6: ab 4f 57             CALL sub_0f57
0ee9:    a3 6b             MOV A, [?6B]
0eeb:    9a f0             SKE A, #F
0eed:       03             BR lbl_0ef1
0eee:    89 00             MOV XA, #0
0ef0:       ee             RET
0ef1:    a3 6a   lbl_0ef1: MOV A, [?6A]
0ef3:    99 71             MOV X, A
0ef5:    a3 69             MOV A, [?69]
0ef7:       ee             RET

    ; FUNCTION sub_0ef8
0ef8:    99 11   sub_0ef8: SEL MB1
0efa:    aa 18             MOV XA, @HL
0efc:    99 10             SEL MB0
0efe:       ee             RET

    ; FUNCTION sub_0eff
0eff:    99 11   sub_0eff: SEL MB1
0f01:    aa 10             MOV @HL, XA
0f03:    99 10             SEL MB0
0f05:       ee             RET

    ; FUNCTION sub_0f06
0f06:    9a 3f   sub_0f06: MOV B, #3
0f08:       e1   lbl_0f08: MOV A, @HL
0f09:       ec             XCH A, @DE
0f0a:       c2             INCS L
0f0b:       60             NOP
0f0c:       c4             INCS E
0f0d:       60             NOP
0f0e:       cf             DECS B
0f0f:       f8             BR lbl_0f08
0f10:       ee             RET
0f11:    89 00             MOV XA, #0 ; DEAD
0f13:    92 64             MOV [?64], XA ; DEAD
0f15:    92 66             MOV [?66], XA ; DEAD
0f17:    9a 3e             MOV C, #3 ; DEAD

    ; DEAD BLOCK lbl_0f19
0f19:    9a 7f   lbl_0f19: MOV B, #7
0f1b:    8b 64             MOV HL, #64
0f1d:       70             MOV A, #0

    ; DEAD BLOCK lbl_0f1e
0f1e:       e9   lbl_0f1e: XCH A, @HL
0f1f:       c2             INCS L
0f20:       60             NOP
0f21:       cf             DECS B
0f22:       fb             BR lbl_0f1e
0f23:       df             XCH A, B

    ; DEAD BLOCK lbl_0f24
0f24:       cf   lbl_0f24: DECS B
0f25:       03             BR lbl_0f29
0f26:       ce             DECS C
0f27:       f1             BR lbl_0f19
0f28:       ee             RET

    ; DEAD BLOCK lbl_0f29
0f29:       4f   lbl_0f29: PUSH BC
0f2a:       e6             CLR1 CY
0f2b:    9a 3f             MOV B, #3
0f2d:    8d 60             MOV DE, #60
0f2f:    8b 64             MOV HL, #64

    ; DEAD BLOCK lbl_0f31
0f31:       e4   lbl_0f31: MOV A, @DE
0f32:       66             ADDS A, #6
0f33:       a9             ADDC A, @HL
0f34:       6a             ADDS A, #A
0f35:       e8             MOV @HL, A
0f36:       c2             INCS L
0f37:       60             NOP
0f38:       c4             INCS E
0f39:       60             NOP
0f3a:       cf             DECS B
0f3b:       f5             BR lbl_0f31
0f3c:       4e             POP BC
0f3d:    8b 68             MOV HL, #68
0f3f:       d6             NOT1 CY
0f40:       d7             SKT CY

    ; DEAD BLOCK lbl_0f41
0f41:    99 02   lbl_0f41: INCS @HL
0f43:    5f 24             BRCB lbl_0f24
0f45:       c2             INCS L
0f46:       60             NOP
0f47:       f9             BR lbl_0f41

    ; FUNCTION sub_0f48
0f48:       4f   sub_0f48: PUSH BC
0f49:       e6             CLR1 CY
0f4a:    9a 3f             MOV B, #3
0f4c:       e4   lbl_0f4c: MOV A, @DE
0f4d:       a9             ADDC A, @HL
0f4e:       e9             XCH A, @HL
0f4f:       c2             INCS L
0f50:       60             NOP
0f51:       c4             INCS E
0f52:       60             NOP
0f53:       cf             DECS B
0f54:       f7             BR lbl_0f4c
0f55:       4e             POP BC
0f56:       ee             RET

    ; FUNCTION sub_0f57
0f57:       4f   sub_0f57: PUSH BC
0f58:       e6             CLR1 CY
0f59:    9a 3f             MOV B, #3
0f5b:    aa 5c             MOV XA, DE
0f5d:    aa 42             XCH XA, HL
0f5f:    aa 54             MOV DE, XA
0f61:       e4   lbl_0f61: MOV A, @DE
0f62:       b8             SUBC A, @HL
0f63:       ec             XCH A, @DE
0f64:       c2             INCS L
0f65:       60             NOP
0f66:       c4             INCS E
0f67:       60             NOP
0f68:       cf             DECS B
0f69:       f7             BR lbl_0f61
0f6a:       4e             POP BC
0f6b:       ee             RET

    ; FUNCTION sub_0f6c
0f6c:       c0   sub_0f6c: INCS A
0f6d:       ee             RET
0f6e:       c1             INCS X
0f6f:       60             NOP
0f70:       ee             RET
0f71:       10             GETI JMPRST ; DEAD
0f72:       10             GETI JMPRST ; DEAD
0f73:       10             GETI JMPRST ; DEAD
0f74:       10             GETI JMPRST ; DEAD
0f75:       10             GETI JMPRST ; DEAD
0f76:       10             GETI JMPRST ; DEAD
0f77:       10             GETI JMPRST ; DEAD
0f78:       10             GETI JMPRST ; DEAD
0f79:       10             GETI JMPRST ; DEAD
0f7a:       10             GETI JMPRST ; DEAD
0f7b:       10             GETI JMPRST ; DEAD
0f7c:       10             GETI JMPRST ; DEAD
0f7d:       10             GETI JMPRST ; DEAD
0f7e:       10             GETI JMPRST ; DEAD
0f7f:       10             GETI JMPRST ; DEAD
0f80:       10             GETI JMPRST ; DEAD
0f81:       10             GETI JMPRST ; DEAD
0f82:       10             GETI JMPRST ; DEAD
0f83:       10             GETI JMPRST ; DEAD
0f84:       10             GETI JMPRST ; DEAD
0f85:       10             GETI JMPRST ; DEAD
0f86:       10             GETI JMPRST ; DEAD
0f87:       10             GETI JMPRST ; DEAD
0f88:       10             GETI JMPRST ; DEAD
0f89:       10             GETI JMPRST ; DEAD
0f8a:       10             GETI JMPRST ; DEAD
0f8b:       10             GETI JMPRST ; DEAD
0f8c:       10             GETI JMPRST ; DEAD
0f8d:       10             GETI JMPRST ; DEAD
0f8e:       10             GETI JMPRST ; DEAD
0f8f:       10             GETI JMPRST ; DEAD
0f90:       10             GETI JMPRST ; DEAD
0f91:       10             GETI JMPRST ; DEAD
0f92:       10             GETI JMPRST ; DEAD
0f93:       10             GETI JMPRST ; DEAD
0f94:       10             GETI JMPRST ; DEAD
0f95:       10             GETI JMPRST ; DEAD
0f96:       10             GETI JMPRST ; DEAD
0f97:       10             GETI JMPRST ; DEAD
0f98:       10             GETI JMPRST ; DEAD
0f99:       10             GETI JMPRST ; DEAD
0f9a:       10             GETI JMPRST ; DEAD
0f9b:       10             GETI JMPRST ; DEAD
0f9c:       10             GETI JMPRST ; DEAD
0f9d:       10             GETI JMPRST ; DEAD
0f9e:       10             GETI JMPRST ; DEAD
0f9f:       10             GETI JMPRST ; DEAD
0fa0:       10             GETI JMPRST ; DEAD
0fa1:       10             GETI JMPRST ; DEAD
0fa2:       10             GETI JMPRST ; DEAD
0fa3:       10             GETI JMPRST ; DEAD
0fa4:       10             GETI JMPRST ; DEAD
0fa5:       10             GETI JMPRST ; DEAD
0fa6:       10             GETI JMPRST ; DEAD
0fa7:       10             GETI JMPRST ; DEAD
0fa8:       10             GETI JMPRST ; DEAD
0fa9:       10             GETI JMPRST ; DEAD
0faa:       10             GETI JMPRST ; DEAD
0fab:       10             GETI JMPRST ; DEAD
0fac:       10             GETI JMPRST ; DEAD
0fad:       10             GETI JMPRST ; DEAD
0fae:       10             GETI JMPRST ; DEAD
0faf:       10             GETI JMPRST ; DEAD
0fb0:       10             GETI JMPRST ; DEAD
0fb1:       10             GETI JMPRST ; DEAD
0fb2:       10             GETI JMPRST ; DEAD
0fb3:       10             GETI JMPRST ; DEAD
0fb4:       10             GETI JMPRST ; DEAD
0fb5:       10             GETI JMPRST ; DEAD
0fb6:       10             GETI JMPRST ; DEAD
0fb7:       10             GETI JMPRST ; DEAD
0fb8:       10             GETI JMPRST ; DEAD
0fb9:       10             GETI JMPRST ; DEAD
0fba:       10             GETI JMPRST ; DEAD
0fbb:       10             GETI JMPRST ; DEAD
0fbc:       10             GETI JMPRST ; DEAD
0fbd:       10             GETI JMPRST ; DEAD
0fbe:       10             GETI JMPRST ; DEAD
0fbf:       10             GETI JMPRST ; DEAD
0fc0:       10             GETI JMPRST ; DEAD
0fc1:       10             GETI JMPRST ; DEAD
0fc2:       10             GETI JMPRST ; DEAD
0fc3:       10             GETI JMPRST ; DEAD
0fc4:       10             GETI JMPRST ; DEAD
0fc5:       10             GETI JMPRST ; DEAD
0fc6:       10             GETI JMPRST ; DEAD
0fc7:       10             GETI JMPRST ; DEAD
0fc8:       10             GETI JMPRST ; DEAD
0fc9:       10             GETI JMPRST ; DEAD
0fca:       10             GETI JMPRST ; DEAD
0fcb:       10             GETI JMPRST ; DEAD
0fcc:       10             GETI JMPRST ; DEAD
0fcd:       10             GETI JMPRST ; DEAD
0fce:       10             GETI JMPRST ; DEAD
0fcf:       10             GETI JMPRST ; DEAD
0fd0:       10             GETI JMPRST ; DEAD
0fd1:       10             GETI JMPRST ; DEAD
0fd2:       10             GETI JMPRST ; DEAD
0fd3:       10             GETI JMPRST ; DEAD
0fd4:       10             GETI JMPRST ; DEAD
0fd5:       10             GETI JMPRST ; DEAD
0fd6:       10             GETI JMPRST ; DEAD
0fd7:       10             GETI JMPRST ; DEAD
0fd8:       10             GETI JMPRST ; DEAD
0fd9:       10             GETI JMPRST ; DEAD
0fda:       10             GETI JMPRST ; DEAD
0fdb:       10             GETI JMPRST ; DEAD
0fdc:       10             GETI JMPRST ; DEAD
0fdd:       10             GETI JMPRST ; DEAD
0fde:       10             GETI JMPRST ; DEAD
0fdf:       10             GETI JMPRST ; DEAD
0fe0:       10             GETI JMPRST ; DEAD
0fe1:       10             GETI JMPRST ; DEAD
0fe2:       10             GETI JMPRST ; DEAD
0fe3:       10             GETI JMPRST ; DEAD
0fe4:       10             GETI JMPRST ; DEAD
0fe5:       10             GETI JMPRST ; DEAD
0fe6:       10             GETI JMPRST ; DEAD
0fe7:       10             GETI JMPRST ; DEAD
0fe8:       10             GETI JMPRST ; DEAD
0fe9:       10             GETI JMPRST ; DEAD
0fea:       10             GETI JMPRST ; DEAD
0feb:       10             GETI JMPRST ; DEAD
0fec:       10             GETI JMPRST ; DEAD
0fed:       10             GETI JMPRST ; DEAD
0fee:       10             GETI JMPRST ; DEAD
0fef:       10             GETI JMPRST ; DEAD
0ff0:       10             GETI JMPRST ; DEAD
0ff1:       10             GETI JMPRST ; DEAD
0ff2:       10             GETI JMPRST ; DEAD
0ff3:       10             GETI JMPRST ; DEAD
0ff4:       10             GETI JMPRST ; DEAD
0ff5:       10             GETI JMPRST ; DEAD
0ff6:       10             GETI JMPRST ; DEAD
0ff7:       10             GETI JMPRST ; DEAD
0ff8:       10             GETI JMPRST ; DEAD
0ff9:       10             GETI JMPRST ; DEAD
0ffa:       10             GETI JMPRST ; DEAD
0ffb:       10             GETI JMPRST ; DEAD
0ffc:       10             GETI JMPRST ; DEAD
0ffd:       10             GETI JMPRST ; DEAD
0ffe:       10             GETI JMPRST ; DEAD
0fff:       10             GETI JMPRST ; DEAD

    ; SECTION .data

1000:       01             .byte #1
1001:       00             .byte #0
1002:       01             .byte #1
1003:       08             .byte #8
1004:       07             .byte #7
1005:       06             .byte #6
1006:       08             .byte #8
1007:       08             .byte #8
1008:       08             .byte #8
1009:       07             .byte #7
100a:       05             .byte #5
100b:       06             .byte #6
100c:       06             .byte #6
100d:       08             .byte #8
100e:       08             .byte #8
100f:       05             .byte #5
1010:       00   sub_1010: .byte #0
1011:       00             .byte #0
1012:       06             .byte #6
1013:       08             .byte #8
1014:       07             .byte #7
1015:       09             .byte #9
1016:       09             .byte #9
1017:       09             .byte #9
1018:       ff             .byte #FF
1019:       ff             .byte #FF
101a:       06             .byte #6
101b:       06             .byte #6
101c:       08             .byte #8
101d:       08             .byte #8
101e:       10             .byte #10
101f:       00             .byte #0
1020:       07             .byte #7
1021:       07             .byte #7
1022:       07             .byte #7
1023:       08             .byte #8
1024:       08             .byte #8
1025:       0a             .byte #A
1026:       09             .byte #9
1027:       ff             .byte #FF
1028:       ff             .byte #FF
1029:       06             .byte #6
102a:       06             .byte #6
102b:       08             .byte #8
102c:       08             .byte #8
102d:       50             .byte #50
102e:       00             .byte #0
102f:       05             .byte #5
1030:       06             .byte #6
1031:       06             .byte #6
1032:       06             .byte #6
1033:       06             .byte #6
1034:       06             .byte #6
1035:       06             .byte #6
1036:       06             .byte #6
1037:       05             .byte #5
1038:       06             .byte #6
1039:       06             .byte #6
103a:       08             .byte #8
103b:       08             .byte #8
103c:       50             .byte #50
103d:       00             .byte #0
103e:       05             .byte #5
103f:       06             .byte #6
1040:       07             .byte #7
1041:       06             .byte #6
1042:       06             .byte #6
1043:       07             .byte #7
1044:       05             .byte #5
1045:       06             .byte #6
1046:       05             .byte #5
1047:       06             .byte #6
1048:       06             .byte #6
1049:       08             .byte #8
104a:       08             .byte #8
104b:       ff             .byte #FF
104c:       00             .byte #0
104d:       00             .byte #0
104e:       00             .byte #0
104f:       00             .byte #0
1050:       00             .byte #0
1051:       00             .byte #0
1052:       00             .byte #0
1053:       00             .byte #0
1054:       00             .byte #0
1055:       00             .byte #0
1056:       00             .byte #0
1057:       00             .byte #0
1058:       00             .byte #0
1059:       00             .byte #0
105a:       ff             .byte #FF
105b:       00             .byte #0
105c:       00             .byte #0
105d:       00             .byte #0
105e:       00             .byte #0
105f:       00             .byte #0
1060:       00             .byte #0
1061:       00             .byte #0
1062:       00             .byte #0
1063:       00             .byte #0
1064:       00             .byte #0
1065:       00             .byte #0
1066:       00             .byte #0
1067:       00             .byte #0
1068:       00             .byte #0
1069:       ff             .byte #FF
106a:       00             .byte #0
106b:       00             .byte #0
106c:       00             .byte #0
106d:       00             .byte #0
106e:       00             .byte #0
106f:       00             .byte #0
1070:       00             .byte #0
1071:       00             .byte #0
1072:       00             .byte #0
1073:       00             .byte #0
1074:       00             .byte #0
1075:       00             .byte #0
1076:       00             .byte #0
1077:       00             .byte #0
1078:       00             .byte #0
1079:       0f             .byte #F
107a:       1e             .byte #1E
107b:       2d             .byte #2D
107c:       3c             .byte #3C
107d:       4b             .byte #4B
107e:       5a             .byte #5A
107f:       69             .byte #69

    ; SECTION .text2


    ; FUNCTION sub_1080 CALLS
        ; sub_06e9
1080: ab 46 e9   sub_1080: CALL sub_06e9
1083:       60             NOP
1084:    9a 09             MOV X, #0
1086:    8d 78             MOV DE, #78
1088:       e6             CLR1 CY
1089:    aa dc             ADDC XA, DE
108b:       d0             MOVT XA, @PCXA
108c:    8d 00             MOV DE, #0
108e:       e6             CLR1 CY
108f:    aa dc             ADDC XA, DE
1091:    92 60             MOV [?60], XA
1093:       ee             RET

    ; FUNCTION sub_1094 CALLS
        ; sub_0f6c
1094:    a2 60   sub_1094: MOV XA, [?60]
1096:       d0             MOVT XA, @PCXA
1097:    b2 60             XCH XA, [?60]
1099: ab 4f 6c             CALL sub_0f6c
109c:    b2 60             XCH XA, [?60]
109e:       ee             RET

    ; FUNCTION sub_109f
109f:    9a 09   sub_109f: MOV X, #0
10a1:    8d a8             MOV DE, #A8
10a3:       e6             CLR1 CY
10a4:    aa dc             ADDC XA, DE
10a6:       d0             MOVT XA, @PCXA
10a7:       ee             RET

    ; SECTION .data2

10a8:       3f             .byte #3F
10a9:       06             .byte #6
10aa:       5b             .byte #5B
10ab:       4f             .byte #4F
10ac:       66             .byte #66
10ad:       6d             .byte #6D
10ae:       7d             .byte #7D
10af:       27             .byte #27
10b0:       7f             .byte #7F
10b1:       6f             .byte #6F
10b2:       00             .byte #0
10b3:       00             .byte #0
10b4:       00             .byte #0
10b5:       00             .byte #0
10b6:       00             .byte #0
10b7:       00             .byte #0
