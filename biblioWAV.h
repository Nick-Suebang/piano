// Bibliothèque WAV fournie par Stephane Duguay (2024), adaptée.
#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "note.h"

// Écrit un fichier WAV complet (PCM 16 bits, stéréo, 44,1 kHz) dans le flux :
// une onde sinusoïdale par note, avec un fondu d'entrée et de sortie.
void genererWAV(std::ostream& sortie, const std::vector<Note>& notes);

// Écrit les sons dans un fichier WAV. Retourne false si le fichier n'a pas pu être écrit.
bool ecrireSons(const std::vector<Note>& notes, const std::string& nomFichierSortie);

#ifdef _WIN32
// Joue les sons sur la carte son (Windows seulement).
void jouerSons(const std::vector<Note>& notes);
#endif
