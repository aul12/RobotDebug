| Anfangszeichen                              | Adresse                                 | Länge                                                               | Byte 0           | Byte ...            | Endzeichen                                   |
|---------------------------------------------|-----------------------------------------|---------------------------------------------------------------------|------------------|---------------------|----------------------------------------------|
| Erstes Zeichen des Datenpakets (zum parsen) | 8bit-Addresse um Daten zu unterscheiden | Länge (in Bytes) der Daten (Bit 7 gesetzt -> signed sonst unsigned) | Erstes Datenbyte | Folgende Datenbytes | Letztes Zeichen des Datenpakets (zum parsen) |
| &#124;                                           | 0xC2                                    | 4 | (1<<7)                                                          | 58               | 237                 | &                                            |