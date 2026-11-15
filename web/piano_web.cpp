// Point d'entrée de la version web (compilée en WebAssembly avec Emscripten).
// Réutilise la lecture de partition et la génération WAV de la version console :
// seule la sortie change (un WAV en mémoire rendu à la page, au lieu de PlaySound).
#include <emscripten/emscripten.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../biblioWAV.h"
#include "../partition.h"

namespace
{
    // Dernier résultat, lu ensuite par la page avec les fonctions ci-dessous.
    std::string wavGenere{};
    std::vector<Note> notesLues{};

    bool indiceValide(int i)
    {
        return i >= 0 && static_cast<std::size_t>(i) < notesLues.size();
    }
}

extern "C"
{
    // Lit la partition reçue de la page et génère le WAV.
    // Retourne la taille du WAV en octets, ou -1 en cas d'erreur (le message est affiché avec cout).
    EMSCRIPTEN_KEEPALIVE int piano_generer(const char* texte)
    {
        std::istringstream partition{ texte };
        std::string erreur{};
        if (!lirePartition(partition, notesLues, erreur))
        {
            std::cout << "Erreur : " << erreur << std::endl;
            return -1;
        }

        std::ostringstream wav{};
        genererWAV(wav, notesLues);
        wavGenere = wav.str();

        std::cout << notesLues.size() << " notes lues, WAV de " << wavGenere.size() / 1024 << " Ko généré." << std::endl;
        return static_cast<int>(wavGenere.size());
    }

    // Adresse du WAV généré dans la mémoire WebAssembly.
    EMSCRIPTEN_KEEPALIVE const char* piano_donnees()
    {
        return wavGenere.data();
    }

    // Notes calculées par le C++, pour que la page anime le clavier pendant la lecture.
    EMSCRIPTEN_KEEPALIVE int piano_nombre_notes()
    {
        return static_cast<int>(notesLues.size());
    }

    EMSCRIPTEN_KEEPALIVE float piano_frequence(int i)
    {
        return indiceValide(i) ? notesLues[i].frequenceHz : 0.0f;
    }

    EMSCRIPTEN_KEEPALIVE float piano_duree(int i)
    {
        return indiceValide(i) ? notesLues[i].dureeMs : 0.0f;
    }
}
