#pragma once

#include <istream>
#include <ostream>

// Nombre maximal de notes dans une partition
constexpr int NOTES_MAX{ 1000 };

// Lit une partition (tempo puis notes) depuis un flux.
// Remplit frequences et dureesEnMillisecondes, et met le nombre de notes dans taille.
bool lirePartitionFlux(std::istream& fichier, float frequences[], float dureesEnMillisecondes[], int& taille);

// Génère un fichier WAV complet dans le flux (défini dans biblioWAV.cpp)
void GenererWAV(std::ostream& memoire, float frequences[], float dureesMillisecondes[], int n);
