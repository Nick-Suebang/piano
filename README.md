# Piano : de la partition au son

Application C++ qui lit une partition musicale au format texte, calcule la fréquence de chaque note
et génère un fichier audio WAV échantillon par échantillon. Sous Windows, le son est joué directement ;
sinon (ou sur demande), il est écrit dans un fichier `.wav`.

👉 **[Essayer la démo en ligne](https://nick-suebang.github.io/piano/)** : le même code C++, compilé en WebAssembly.

## Structure du projet

| Fichier | Rôle |
| --- | --- |
| `note.h` | La structure `Note` : une fréquence (Hz) et une durée (ms) |
| `partition.h` / `partition.cpp` | Lecture et validation d'une partition, calcul des fréquences et des durées |
| `biblioWAV.h` / `biblioWAV.cpp` | Génération du WAV, écriture dans un fichier, lecture sur la carte son (Windows) |
| `main.cpp` | Programme console : arguments, messages d'erreur, choix entre jouer et écrire |
| `web/` | Version web : point d'entrée Emscripten (`piano_web.cpp`), page (`index.html`), script de compilation |
| `partitions/` | Exemples de partitions |

La lecture de la partition ne fait aucun affichage : elle renvoie les notes ou un message d'erreur,
et c'est l'appelant (console ou page web) qui décide comment le montrer. C'est ce qui permet
de réutiliser exactement le même code dans la version console et la version web.

## Format de partition

```
120
3 DO - 1
3 RE # 0.5
3 MI - 2
```

- **Première ligne** : le tempo, `60` ou `120` battements par minute.
- **Puis une note par ligne** : `<octave> <NOTE> <altération> <durée>`
  - Octave : nombre entier ; l'octave 3 contient le LA à 440 Hz.
  - Note : `DO RE MI FA SO LA SI` (en majuscules).
  - Altération : `#` (dièse), `b` (bémol) ou `-` (aucune).
  - Durée en temps : `1` = noire, `2` = blanche, `0.5` = croche.
- Les lignes vides sont ignorées. Une ligne mal formée ou une note inconnue arrête la lecture
  avec un message qui indique le numéro de la ligne.

La fréquence suit le tempérament égal : `440 × 2^(octave − 3 + (demi-tons − 9) / 12)`.

## Compilation et exécution

### Windows (Visual Studio)

1. Ouvrir `Piano.sln` dans Visual Studio 2022 ou plus récent (C++17).
2. Choisir la configuration (Debug/Release, x64 recommandé), puis compiler (F7).
3. Lancer :

```bash
Piano.exe partitions/au_clair_de_la_lune.txt
```

```bash
Piano.exe partitions/au_clair_de_la_lune.txt sortie.wav
```

Le premier exemple joue la partition ; le second l'écrit dans `sortie.wav`.

### Linux / macOS

```bash
g++ -std=c++17 -Wall -Wextra -O2 main.cpp partition.cpp biblioWAV.cpp -o piano
```

```bash
./piano partitions/au_clair_de_la_lune.txt sortie.wav
```

Sans fichier de sortie, le son est écrit dans `partition.wav`.

### Codes de sortie

| Code | Signification |
| --- | --- |
| 0 | Succès |
| 1 | Mauvais nombre d'arguments |
| 2 | Fichier introuvable ou partition invalide |
| 3 | Impossible d'écrire le fichier WAV |

## Démo web (WebAssembly)

`partition.cpp` et `biblioWAV.cpp` sont compilés avec [Emscripten](https://emscripten.org) avec un point
d'entrée propre au web (`web/piano_web.cpp`) à la place de `main.cpp`. La page passe le texte de la
partition au C++, récupère le WAV généré et le joue en animant le clavier.

- `web/build.sh` : compile vers `dist/` (`em++` requis).
- `.github/workflows/pages.yml` : à chaque pull request, compile et teste la version console sous Linux
  et compile la version web ; à chaque push sur `main`, publie la démo sur GitHub Pages.

## Format WAV généré

PCM 16 bits, stéréo, 44 100 Hz. Chaque note est une onde sinusoïdale avec un fondu d'entrée et de sortie
(5 % de sa durée) pour éviter les clics.

```
Offset  Taille  Description
0-3     4       "RIFF"
4-7     4       Taille du fichier - 8
8-11    4       "WAVE"
12-15   4       "fmt "
16-19   4       Taille du bloc format (16)
20-21   2       Format audio (1 = PCM)
22-23   2       Nombre de canaux (2)
24-27   4       Fréquence d'échantillonnage (44100)
28-31   4       Octets par seconde (176400)
32-33   2       Octets par bloc (4)
34-35   2       Bits par échantillon (16)
36-39   4       "data"
40-43   4       Taille des données audio
44+     ...     Échantillons (gauche, droite, gauche, droite…)
```

## Crédits

La bibliothèque WAV (`biblioWAV`) est basée sur le code fourni par Stephane Duguay (2024).

## Licence

Projet sous licence MIT.
Copyright (c) 2024 Nick Suebang
