// Programme console : lit une partition et la joue (Windows) ou l'écrit dans un fichier WAV.
#include <clocale>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "biblioWAV.h"
#include "partition.h"

int main(int argc, char* argv[])
{
    std::setlocale(LC_ALL, "");

    if (argc < 2 || argc > 3)
    {
        std::cout << "Usage : " << argv[0] << " <partition.txt> [sortie.wav]" << std::endl;
        std::cout << "  Sans fichier de sortie, la partition est jouée (Windows) ou écrite dans partition.wav." << std::endl;
        return 1;
    }

    std::ifstream fichier{ argv[1] };
    if (!fichier)
    {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << argv[1] << "." << std::endl;
        return 2;
    }

    std::vector<Note> notes{};
    std::string erreur{};
    if (!lirePartition(fichier, notes, erreur))
    {
        std::cerr << "Erreur : " << erreur << std::endl;
        return 2;
    }

    const bool sortieDemandee{ argc == 3 };
#ifdef _WIN32
    if (!sortieDemandee)
    {
        jouerSons(notes);
        return 0;
    }
#endif

    const std::string nomSortie{ sortieDemandee ? argv[2] : "partition.wav" };
    if (!ecrireSons(notes, nomSortie))
    {
        std::cerr << "Erreur : impossible d'écrire " << nomSortie << "." << std::endl;
        return 3;
    }
    std::cout << notes.size() << " notes écrites dans " << nomSortie << "." << std::endl;
    return 0;
}
