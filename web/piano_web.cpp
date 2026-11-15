// Point d'entrée de la version web (compilée en WebAssembly avec Emscripten).
// Réutilise la lecture de partition de main.cpp et la génération WAV de biblioWAV.cpp :
// seule la sortie change (un WAV en mémoire rendu à la page, au lieu de PlaySound).
#include <emscripten/emscripten.h>

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include "../partition.h"

using namespace std;

static string wavGenere{};
static float frequences[NOTES_MAX];
static float dureesMillisecondes[NOTES_MAX];
static int nombreNotes{ 0 };

extern "C" {

// Lit la partition reçue de la page et génère le WAV.
// Retourne la taille du WAV en octets, ou -1 en cas d'erreur (le message est affiché avec cout).
EMSCRIPTEN_KEEPALIVE int piano_generer(const char* texte)
{
    int taille{ 0 };
    nombreNotes = 0;
    istringstream partition{ texte };

    try
    {
        if (!lirePartitionFlux(partition, frequences, dureesMillisecondes, taille))
        {
            return -1;
        }
    }
    catch (const exception&)
    {
        cout << "Erreur : la première ligne doit être le tempo (60 ou 120)." << endl;
        return -1;
    }

    if (taille == 0)
    {
        cout << "Erreur : aucune note trouvée dans la partition." << endl;
        return -1;
    }

    stringstream memoire{};
    GenererWAV(memoire, frequences, dureesMillisecondes, taille);
    wavGenere = memoire.str();
    nombreNotes = taille;

    cout << taille << " notes lues, WAV de " << wavGenere.size() / 1024 << " Ko généré." << endl;
    return static_cast<int>(wavGenere.size());
}

// Adresse du WAV généré dans la mémoire WebAssembly (lue par la page).
EMSCRIPTEN_KEEPALIVE const char* piano_donnees()
{
    return wavGenere.data();
}

// Notes calculées par le C++, pour que la page anime le clavier pendant la lecture.
EMSCRIPTEN_KEEPALIVE int piano_nombre_notes()
{
    return nombreNotes;
}

EMSCRIPTEN_KEEPALIVE float piano_frequence(int i)
{
    return (i >= 0 && i < nombreNotes) ? frequences[i] : 0.0f;
}

EMSCRIPTEN_KEEPALIVE float piano_duree(int i)
{
    return (i >= 0 && i < nombreNotes) ? dureesMillisecondes[i] : 0.0f;
}

}
