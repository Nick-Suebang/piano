// Bibliothèque WAV fournie par Stephane Duguay (2024), adaptée.
#include "biblioWAV.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#endif

namespace
{
    constexpr int ECHANTILLONS_PAR_SECONDE{ 44100 };
    constexpr int NOMBRE_CANAUX{ 2 };
    constexpr int BITS_PAR_ECHANTILLON{ 16 };
    constexpr int OCTETS_PAR_BLOC{ NOMBRE_CANAUX * BITS_PAR_ECHANTILLON / 8 };
    constexpr std::uint16_t FORMAT_PCM{ 1 };
    constexpr std::uint32_t TAILLE_BLOC_FORMAT{ 16 };
    constexpr std::uint32_t TAILLE_ENTETE_SANS_RIFF{ 36 };   // « WAVE » + bloc « fmt » + en-tête « data »

    constexpr double AMPLITUDE_MAX{ 32760 };   // volume, juste sous la limite des 16 bits
    constexpr float RAMPE_FONDU{ 0.05f };      // 5 % de chaque note en fondu d'entrée et de sortie
    constexpr float PI{ 3.14159265f };

    // Écrit un entier en petit-boutiste (little-endian), comme l'exige le format WAV.
    template <typename Entier>
    void ecrireValeur(std::ostream& sortie, Entier valeur)
    {
        for (std::size_t octet{ 0 }; octet < sizeof(Entier); ++octet)
        {
            sortie.put(static_cast<char>(valeur & 0xFF));
            valeur >>= 8;
        }
    }

    // Atténue le début et la fin de la note pour éviter les clics.
    float appliquerFondu(float valeur, float tNormalise, float rampeNormalisee)
    {
        if (tNormalise < rampeNormalisee)
        {
            return valeur * tNormalise / rampeNormalisee;
        }
        if (tNormalise > 1 - rampeNormalisee)
        {
            return valeur * (1 - tNormalise) / rampeNormalisee;
        }
        return valeur;
    }

    int nombreEchantillons(const Note& note)
    {
        return static_cast<int>(ECHANTILLONS_PAR_SECONDE * note.dureeMs / 1000.f);
    }

    void ecrireEntete(std::ostream& sortie, std::uint32_t tailleDonnees)
    {
        sortie << "RIFF";
        ecrireValeur<std::uint32_t>(sortie, TAILLE_ENTETE_SANS_RIFF + tailleDonnees);
        sortie << "WAVE";

        sortie << "fmt ";
        ecrireValeur<std::uint32_t>(sortie, TAILLE_BLOC_FORMAT);
        ecrireValeur<std::uint16_t>(sortie, FORMAT_PCM);
        ecrireValeur<std::uint16_t>(sortie, NOMBRE_CANAUX);
        ecrireValeur<std::uint32_t>(sortie, ECHANTILLONS_PAR_SECONDE);
        ecrireValeur<std::uint32_t>(sortie, ECHANTILLONS_PAR_SECONDE * OCTETS_PAR_BLOC);   // octets par seconde
        ecrireValeur<std::uint16_t>(sortie, OCTETS_PAR_BLOC);
        ecrireValeur<std::uint16_t>(sortie, BITS_PAR_ECHANTILLON);

        sortie << "data";
        ecrireValeur<std::uint32_t>(sortie, tailleDonnees);
    }

    void ecrireNote(std::ostream& sortie, const Note& note)
    {
        const int echantillons{ nombreEchantillons(note) };
        for (int j{ 0 }; j < echantillons; ++j)
        {
            const float onde{ std::sin((2.f * PI * j * note.frequenceHz) / ECHANTILLONS_PAR_SECONDE) };
            const float valeur{ appliquerFondu(onde, static_cast<float>(j) / echantillons, RAMPE_FONDU) };
            const auto echantillon{ static_cast<std::int16_t>(AMPLITUDE_MAX * valeur) };
            for (int canal{ 0 }; canal < NOMBRE_CANAUX; ++canal)
            {
                ecrireValeur(sortie, echantillon);
            }
        }
    }
}

void genererWAV(std::ostream& sortie, const std::vector<Note>& notes)
{
    std::uint32_t totalEchantillons{ 0 };
    for (const Note& note : notes)
    {
        totalEchantillons += nombreEchantillons(note);
    }

    ecrireEntete(sortie, totalEchantillons * OCTETS_PAR_BLOC);
    for (const Note& note : notes)
    {
        ecrireNote(sortie, note);
    }
}

bool ecrireSons(const std::vector<Note>& notes, const std::string& nomFichierSortie)
{
    std::ofstream fichier{ nomFichierSortie, std::ios::binary };
    if (!fichier)
    {
        return false;
    }
    genererWAV(fichier, notes);
    return static_cast<bool>(fichier);
}

#ifdef _WIN32
void jouerSons(const std::vector<Note>& notes)
{
    std::ostringstream wav{};
    genererWAV(wav, notes);
    const std::string donnees{ wav.str() };   // doit rester en vie pendant toute la lecture
    PlaySoundA(donnees.data(), nullptr, SND_MEMORY | SND_SYNC);
}
#endif
