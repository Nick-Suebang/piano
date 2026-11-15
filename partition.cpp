#include "partition.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <sstream>

namespace
{
    constexpr int TEMPO_LENT{ 60 };
    constexpr int TEMPO_RAPIDE{ 120 };
    constexpr float MS_PAR_MINUTE{ 60000.0f };

    // Référence : le LA de l'octave 3 vaut 440 Hz.
    constexpr float FREQUENCE_LA_HZ{ 440.0f };
    constexpr int OCTAVE_LA{ 3 };
    constexpr float DEMI_TONS_PAR_OCTAVE{ 12.0f };

    // Position de chaque note dans l'octave, en demi-tons à partir de DO.
    struct NomDeNote
    {
        const char* nom;
        int demiTon;
    };
    constexpr std::array<NomDeNote, 7> GAMME{ {
        { "DO", 0 }, { "RE", 2 }, { "MI", 4 }, { "FA", 5 }, { "SO", 7 }, { "LA", 9 }, { "SI", 11 }
    } };
    constexpr int DEMI_TON_LA{ 9 };

    bool estVide(const std::string& ligne)
    {
        return ligne.find_first_not_of(" \t\r") == std::string::npos;
    }

    // Fréquence d'une note (tempérament égal), ou rien si le nom ou l'altération est inconnu.
    std::optional<float> calculerFrequence(int octave, const std::string& nom, const std::string& alteration)
    {
        const auto note{ std::find_if(GAMME.begin(), GAMME.end(),
            [&nom](const NomDeNote& candidat) { return nom == candidat.nom; }) };
        if (note == GAMME.end())
        {
            return std::nullopt;
        }

        int demiTon{ note->demiTon };
        if (alteration == "#")
        {
            ++demiTon;
        }
        else if (alteration == "b")
        {
            --demiTon;
        }
        else if (alteration != "-")
        {
            return std::nullopt;
        }

        const float exposant{ (octave - OCTAVE_LA) + (demiTon - DEMI_TON_LA) / DEMI_TONS_PAR_OCTAVE };
        return FREQUENCE_LA_HZ * std::pow(2.0f, exposant);
    }

    float calculerDureeMs(float temps, int tempo)
    {
        return temps * MS_PAR_MINUTE / tempo;
    }

    std::string erreurLigne(int numero, const std::string& message)
    {
        return "ligne " + std::to_string(numero) + " : " + message;
    }
}

bool lirePartition(std::istream& source, std::vector<Note>& notes, std::string& erreur)
{
    notes.clear();

    std::string ligne{};
    int tempo{ 0 };
    if (!std::getline(source, ligne) || !(std::istringstream{ ligne } >> tempo))
    {
        erreur = "la première ligne doit être le tempo (60 ou 120).";
        return false;
    }
    if (tempo != TEMPO_LENT && tempo != TEMPO_RAPIDE)
    {
        erreur = "le tempo de la partition doit être soit 60 bpm, soit 120 bpm.";
        return false;
    }

    int numeroLigne{ 1 };
    while (std::getline(source, ligne))
    {
        ++numeroLigne;
        if (estVide(ligne))
        {
            continue;
        }

        std::istringstream champs{ ligne };
        int octave{ 0 };
        std::string nom{};
        std::string alteration{};
        float temps{ 0.0f };
        if (!(champs >> octave >> nom >> alteration >> temps) || temps <= 0.0f)
        {
            erreur = erreurLigne(numeroLigne, "format attendu « octave NOTE altération durée », par exemple « 3 DO - 1 ».");
            return false;
        }

        const std::optional<float> frequence{ calculerFrequence(octave, nom, alteration) };
        if (!frequence)
        {
            erreur = erreurLigne(numeroLigne, "note « " + nom + " " + alteration
                + " » inconnue (notes : DO RE MI FA SO LA SI ; altérations : # b -).");
            return false;
        }

        notes.push_back({ *frequence, calculerDureeMs(temps, tempo) });
    }

    if (notes.empty())
    {
        erreur = "la partition ne contient aucune note.";
        return false;
    }
    return true;
}
