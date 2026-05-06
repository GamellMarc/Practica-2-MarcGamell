# Pràctica 2 - Processadors Digitals: Interrupcions

Aquest repositori conté els projectes i la memòria corresponents a la **Pràctica 2** de l'assignatura de Processadors Digitals. L'objectiu principal d'aquesta pràctica és comprendre i implementar el mecanisme d'interrupcions per abandonar el mètode de consulta contínua (*polling*) i optimitzar dràsticament el temps lliure del microcontrolador **ESP32-S3**.

## 📂 Estructura del Repositori

El repositori està dividit en les següents carpetes i fitxers:

* **`Practica2_Interrupcio/`**: Conté el projecte de la Pràctica A. 
    * Implementació d'una interrupció per maquinari (GPIO) associada a un botó.
    * Desenvolupament d'un filtre antirebots (*debounce*) per programari.
    * Ús d'un temporitzador no bloquejant per desconnectar la interrupció al cap d'un minut.

* **`Practica2B/`**: Conté el projecte de la Pràctica B.
    * Ús d'una interrupció programada per temporitzador (Hardware Timer).
    * Creació d'un cronòmetre exacte de segons en segon pla.
    * Gestió concurrent aturant el cronòmetre mitjançant una interrupció GPIO externa (botó) i protecció de variables amb seccions crítiques (Mutex).

* **`Practica2Extra/`**: Conté el projecte de l'activitat addicional generada mitjançant IA (ChatGPT).
    * Ús d'un temporitzador d'alta freqüència (1 ms).
    * Control de la freqüència de parpelleig d'un LED mitjançant dos polsadors (pujar i baixar velocitat).
    * Gestió centralitzada de lectures, temporització i antirebots exclusivament dins de la Rutina de Servei d'Interrupció (ISR).

* **`InformePràctica2PD.pdf`**: Memòria completa de la pràctica. Inclou l'explicació detallada de cada línia de codi important, diagrames de flux, diagrames de temps i una anàlisi exhaustiva de l'evolució de l'ocupació (temps lliure) del processador.

## 🛠️ Tecnologies i Maquinari Utilitzat

* **Placa:** YD-ESP32-S3 (Compatible amb ESP32-S3-DevKitC-1)
* **Entorn de desenvolupament:** Visual Studio Code + extensió PlatformIO
* **Llenguatge:** C++ (Framework d'Arduino)
* **Comunicació:** Monitor sèrie a 115200 bauds

## 👤 Autor
**Marc Gamell**
