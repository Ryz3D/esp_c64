# ESP_C64

## Zusammenbauen

### Pinbelegung

| GPIO  | Belegung        |
| ----- | --------------- |
| 1-8   | Tastatur Port B |
| 9-16  | Tastatur Port A |
| 42    | RESTORE         |
| 18    | LCD RD          |
| 19    | LCD WR          |
| 20    | LCD RS          |
| 21    | LCD CS          |
| 33    | LCD RST         |
| 34-41 | LCD D0-D7       |

### Schaltplan

## Extras

### CPU Takt

Testprogramm an 0xC000 (49152) ausführen, Resultat wird über UART angezeigt:

```
SYS 49152
```

### UART

0xCFF0 (53232) setzen:

```
POKE 53232, 0 REM UART aus
POKE 53232, 1 REM UART an
```

### Zoom

0xCFF1 (53233) setzen:

```
POKE 53233, 0 REM 1x Zoom
POKE 53233, 1 REM 2x Zoom
```

### Bildschirmfarben

0xCFF2 (53234) setzen:

```
POKE 53234, 0 REM Blau
POKE 53234, 1 REM Gruen
POKE 53234, 2 REM Schwarz Weiss
POKE 53234, 3 REM Weiss Schwarz
POKE 53234, 4 REM Grau Blau
```
