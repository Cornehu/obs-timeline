# OBS Timeline Monitor

Plugin OBS Studio qui affiche une barre de progression en temps réel pour chaque source média (vidéo/audio) dans la scène active.

## Téléchargement

**[⬇ Télécharger la dernière version](../../releases/latest)**

| Plateforme | Statut |
|---|---|
| macOS (Apple Silicon) | ✅ Disponible |
| Windows | 🔜 Bientôt |
| Linux | 🔜 Bientôt |

## Fonctionnalités

- Barre de progression live pour chaque source média dans la scène active
- Affichage du temps écoulé et restant
- Détection automatique des sources — aucune configuration
- Compatible scènes imbriquées et groupes
- Supporte `ffmpeg_source` (Source Média) et `vlc_source` (Source VLC)

## Installation — macOS

> **Requis :** OBS Studio 32.x, Mac Apple Silicon (M1/M2/M3/M4)

1. Télécharge `obs-timeline-monitor-macos-arm64.zip` depuis les [Releases](../../releases/latest)
2. Dézippe et copie `obs-timeline-monitor.plugin` dans :
   ```
   ~/Library/Application Support/obs-studio/plugins/
   ```
   *(crée le dossier s'il n'existe pas)*
3. Redémarre OBS
4. Va dans **Docks → obs-timeline-monitor**

> Si macOS bloque l'ouverture : **Réglages Système → Confidentialité et sécurité → Ouvrir quand même**

## Compiler depuis les sources — macOS

**Prérequis :** Xcode, CMake, Homebrew

```bash
brew install cmake qt@6 simde
git clone https://github.com/Cornehu/obs-timeline-monitor.git
cd obs-timeline-monitor
./deploy.sh
```

Le script clone automatiquement les sources OBS, compile le plugin et l'installe dans OBS.

## Licence

MIT
