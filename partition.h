#pragma once

#include <istream>
#include <string>
#include <vector>

#include "note.h"

// Format d'une partition :
//   - première ligne : le tempo, 60 ou 120 battements par minute ;
//   - puis une note par ligne : <octave> <NOTE> <altération> <durée en temps>
//     NOTE : DO RE MI FA SO LA SI ; altération : # (dièse), b (bémol) ou - (aucune).
//   Exemple : « 3 LA - 1 » est un LA à 440 Hz qui dure un temps.
//
// Lit une partition complète et calcule la fréquence et la durée de chaque note.
// En cas de problème, retourne false et explique la cause dans « erreur ».
bool lirePartition(std::istream& source, std::vector<Note>& notes, std::string& erreur);
