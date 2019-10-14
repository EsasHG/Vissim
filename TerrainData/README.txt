ZIP-filen i denne mappen inneholder den nedlastede kartdataen brukt i simuleringen.
Dette er i en .txt-fil, generert med programmet las2txt
Hvis du unzipper denne og endrer filepathen sendt inn i funksjonen mSurface->readTxtFiles() i linje 115 i renderwindow.cpp til mappen filen ligger i,
vil punktene trianguleres og du vil få overflaten vist i videoen. Dette kan ta en god stund.
Filen terrainData.txt inneholder den allerede triangulerte meshen, og hvis denne filen legges inn i build-mappa, 
burde programmet finne den av seg selv og ikke bry seg med å triangulere punktene på nytt. Dette vil være mye raskere enn å kjøre trianguleringen.