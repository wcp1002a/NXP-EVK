# NXP-EVK
Quick instructions for practical use of NXP EVK

### Table of Contents

* [LPC802 EVK](#lpc802-evk)
* [i.MXRT 1010 EVK](#imxrt-1010-evk)

## LPC802 EVK

### Debugger
#### Use LPCLink2 to debug LPC802 EVK
* LPCLink2: JP1 short, JP2 open, 轉接板接J7
* LC802EVK: JP4 short

LPCLink2    | LPC802EVK
------------|----------------
J1.1        | VCC CN5.6
J1.2        | GND CN5.5
J1.5        | SWDIO   CN4.8
J1.7        | SWDCK   CN4.7

#### Use RT1010 EVK to debug LPC802 EVK
RT1010  | LPC802EVK
--------|-------------------
J57.16  | VCC CN5.6
J57.14  | GND CN5.5
J61.1   | SWDIO   CN4.8
J62.1   | SWDCK   CN4.7
J31.1   | <-RX TX->   MCU TX
J32.1   | <-TX RX->   MCU RX

#### Flash Magic
* HEX file
在MCUXpresso產生出的axf檔案上按右鍵 -> Binary Utility→Create hex
* 進入ISP(In-System Programming)模式
將ISP拉到Low並RESET  
以LPC802EVK為例，按下ISP button不放，再按下RESET鍵

UART Connection
LPC802EVK   | Signal Name       | PC
------------|-------------------|-------
CN4.6       | PIO0_4_TXD_SCK    | RX
CN5.2       | PIO0_0_RXD        | TX

## i.MXRT 1010 EVK

i.MXRT 1010 introduction

