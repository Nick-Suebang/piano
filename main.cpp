
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "biblioWAV.h"
#include "partition.h"
using namespace std;

void calculerFrequences(int octaves, const string& notes, const string& alterations, float durees, float frequences[], int taille)
{
    int DemiTon{ 0 };
    if (notes == "DO")
    {
        DemiTon = 1;
    }
    else if (notes == "RE")
    {
        DemiTon = 3;
    }
    else if (notes == "MI")
    {
        DemiTon = 5;
    }
    else if (notes == "FA")
    {
        DemiTon = 6;
    }
    else if (notes == "SO")
    {
        DemiTon = 8;
    }
    else if (notes == "LA")
    {
        DemiTon = 10;
    }
    else if (notes == "SI")
    {
        DemiTon = 12;
    }
    if (alterations == "b")
    {
        DemiTon -= 1;
    }
    else if (alterations == "#")
    {
        DemiTon += 1;
    }
    float exposant{ (octaves - 3.0f) + ((DemiTon - 10.0f) / 12.0f) };
    frequences[taille] = 440.0f * pow(2.0f, exposant);
}

void calculerDurees(float durees, const int tempo, float dureeMillisecondes[1000], int taille)
{
    dureeMillisecondes[taille] = (durees * 60000) / tempo;
}

// Lit une partition depuis n'importe quel flux (fichier sur le PC, texte de la page web).
bool lirePartitionFlux(istream& fichier, float frequences[], float dureesEnMillisecondes[], int& taille)
{
    string contenu{ "" };
    int octaves{ 0 };
    string notes{ "" };
    string alterations{ "" };
    float durees{ 0.0 };
    getline(fichier, contenu);
    int tempo{ stoi(contenu) };
    if (tempo != 60 && tempo != 120)
    {
        cout << "Erreur : Le tempo de la partition doit être soit 60 bpm, soit 120 bpm." << endl;
        return false;
    }
    while (taille < NOTES_MAX && fichier >> octaves >> notes >> alterations >> durees)
    {
        calculerFrequences(octaves, notes, alterations, durees, frequences, taille);
        calculerDurees(durees, tempo, dureesEnMillisecondes, taille);
        taille++;
    }
    return true;
}

bool lirePartition(const string NomFichier, float frequences[], float dureesEnMillisecondes[], int& taille)
{
    ifstream fichier(NomFichier);
    if (!fichier)
    {
        cout << "Erreur : Impossible d'ouvrir le fichier." << endl;
        return false;
    }
    return lirePartitionFlux(fichier, frequences, dureesEnMillisecondes, taille);
}

// La version web (web/piano_web.cpp) a son propre point d'entrée : pas de main().
#ifndef __EMSCRIPTEN__
int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <fichier_partition.musique>" << endl;
        return 1;
    }
    int taille{ 0 };
    float frequences[NOTES_MAX];
    float dureesMillisecondes[NOTES_MAX];
    if (!lirePartition(argv[1], frequences, dureesMillisecondes, taille))
    {
        return 2;
    }
    JouerSons(frequences, dureesMillisecondes, taille);

    return 0;
}
#endif