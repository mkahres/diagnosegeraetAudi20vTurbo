@echo off
echo Stelle sicher, dass benoetigte Bibliotheken installiert sind...
py -m pip install pyserial matplotlib mplcursors

pythonw SensorenAuswertungAudi20vTurbo.py
