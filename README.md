# mRoubaud
 Prononcer milliRoubaud

Afficheur de sonnets autonome utilisant un [TTGO T5 4.7](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47)

Le code source utilise la bibliothèque [epdiy](https://github.com/vroland/epdiy) et a été construit à partir de [l'exemple](https://github.com/vroland/epdiy/tree/master/examples/lilygo-t5-47-epd-platformio) fourni pour l'affichage de texte sur l'EDP47.

La durée d'affichage d'un poème est déterminée par 

`int64_t sleepTime = 60;          // in minutes`

En fonctionnant avec une batterie 18650, avec chargement d'un nouveau poème chaque jour on devrait disposer d'une autonomie d'au moins un an (l'ESP32 étant tout le temps en mode *deepsleep* sauf les quelques secondes où il affiche un nouveau poème). On peut forcer le changment de poème en utilisant le premier bouton (celui à côte des connecteurs).

La version actuelle ne propose qu'une cinquantaine de poèmes, la version cible en comportera 150 soit un milliRoubaud (unité de mesure adhoc inspirée par le mathématicien, romancier, poète [Jacques Roubaud](https://fr.wikipedia.org/wiki/Jacques_Roubaud) qui dit avoir lu 150 000 sonnets). 

Le texte des sonnets a été récupéré depuis http://michel.balmont.free.fr/pedago/sonnet/anthologie_sonnets.pdf et sera complété ultérieurement sans doute en puisant dans l'anthologie [Quasi-Cristaux](https://blogs.oulipo.net/qc/choix-premiere-partie/2-presentation-generale/) des sonnets depuis 1801 propsée par Roubaud.

La disposition du texte est limitée par les possibilités de la bibliothèque epdiy qui ne gère pas (encore) l'alignement (un paramètre alignement est prévu mais non implémenté) et surtout le passage à la ligne si le texte déborde de la zone d'affichage. De surcroit, un passage à la ligne standard ne conviendrait pas pour de la poésie.

Les fontes utilisées ont été reprises dans la bibliothèque qui propose un script pour en générer d'autres à partir d'un fichier ttf.
