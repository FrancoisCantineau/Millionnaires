# SaveFramework

Plugin de sauvegarde générique et découplé, réutilisable entre projets.
Le plugin ne connaît **aucun** type de gameplay — il ne manipule que
`ISaveable`, des GUID et des `FInstancedStruct`.

## Installation

Copie le dossier `SaveFramework/` dans `Engine/Plugins/` (partagé entre
tous tes projets) ou dans un repo Git séparé que tu ajoutes en submodule
au dossier `Plugins/` de chaque projet. Active-le dans chaque `.uproject`.

## Structure

```
Source/SaveFramework/
├── Public/Core/    Private/Core/     ← ISaveable, SaveGuidComponent, Registry, Subsystem, SaveGame
├── Public/Serialization/  Private/Serialization/   ← vide pour l'instant
├── Public/Runtime/  Private/Runtime/                ← USaveFrameworkWorldState
└── Public/Editor/   Private/Editor/                 ← vide pour l'instant (outils éditeur)
```

Les dossiers encore vides existent pour ne pas avoir à réorganiser tout le
plugin le jour où un vrai besoin y arrive — mais ils restent vides tant que
ce besoin n'existe pas concrètement sur un projet.

## Ce qu'il contient (v1)

- `ISaveable` — interface pour le **custom** (HP, quêtes, inventaire...), optionnelle
- `USaveGuidComponent` — GUID stable + gère seul le **générique** (position, actif/visible), sans code custom nécessaire
- `USaveableRegistrySubsystem` — liste en mémoire des composants actuellement chargés
- `USaveFrameworkWorldState` — point d'entrée unique pour agir sur un acteur par GUID, chargé ou non (`SetState`) ; si l'acteur n'est pas chargé, le changement est appliqué automatiquement à son prochain `BeginPlay()`
- `FSaveableActorReference` — référence design-friendly (drag-drop d'acteur en éditeur, même dans un niveau non chargé), résolue en GUID stable — à utiliser depuis un orchestrator ou tout autre système externe
- `USaveFrameworkSaveGame` — conteneur de données pur (générique + custom par acteur)
- `USaveFrameworkSubsystem` — `SaveGame(SlotName)` / `LoadGame(SlotName)`, lit le Registry, écrit générique + custom

## Comment un orchestrator bouge un acteur non chargé

```
Orchestrator (via FSaveableActorReference.CachedId)
    │
    ▼
USaveFrameworkWorldState.SetState(Guid, Patch)  // ex: FSaveableStatePatch::MakeTransform(NewTransform)
    │
    ├── Acteur chargé → SaveGuidComponent applique tout de suite
    │
    └── Acteur pas chargé → stocké, rien d'autre ne se passe
                │
                ▼
        (l'acteur charge, plus tard, peu importe quand ni pourquoi)
                │
                ▼
        SaveGuidComponent::BeginPlay() → consomme l'état en attente → s'applique
```

L'orchestrator n'appelle jamais un acteur ou un composant directement — il
ne parle qu'au `USaveFrameworkWorldState`, avec un GUID. Ça marche pareil que
l'acteur soit déchargé par streaming, dans un niveau différent non ouvert,
ou tout simplement pas encore spawné.

## Ce qu'il ne contient PAS encore (à ajouter seulement quand un vrai besoin apparaît)

- Sauvegarde async (la v1 est synchrone pour rester simple à débugger)
- Versioning / migration de structs
- Gestion des acteurs spawnés dynamiquement (spawn records)
- Backends multiples (cloud, Steam...)
- Détails-panel customization pour `FSaveableActorReference` (résolution auto sans appel manuel à `RefreshCachedId()`)

Ne les ajoute pas avant d'en avoir besoin sur un projet réel — c'est le
piège classique qui transforme un plugin "propre" en usine à gaz.

## Générique vs custom : deux canaux séparés, jamais un catalogue de fonctions

`USaveFrameworkWorldState` expose volontairement deux fonctions génériques
(`SetState`) et une seule fonction custom générique
(`ApplyCustomState`) — pas une fonction par comportement (`OpenDoor`,
`RotateActor`, `PlayAnimation`...). Le WorldState ne sait jamais ce qu'il y
a dans un `FInstancedStruct` custom ; c'est l'acteur, via `ISaveable`, qui
décide quoi en faire (jouer une Timeline, un son, un Niagara...).

```
Orchestrator
    │
    ├── SetState(Guid, FSaveableStatePatch)        ─ générique, géré par SaveGuidComponent
    │
    └── ApplyCustomState(Guid, FInstancedStruct)   ─ custom, géré par ISaveable de l'acteur
```

Les deux canaux fonctionnent à l'identique, chargé ou pas :

## Ce que `SaveGame()`/`LoadGame()` garantissent

- Un changement fait via `USaveFrameworkWorldState` (générique OU custom) sur un
  acteur déchargé est **écrit sur disque** dès le prochain `SaveGame()`,
  consommé ou pas — rien n'est perdu si le jeu ferme avant que l'acteur ne
  recharge.
- Un `LoadGame()` réinjecte dans le `USaveFrameworkWorldState` l'état (générique
  ET custom) de tout acteur pas encore chargé, pour qu'il s'applique
  automatiquement dès que cet acteur charge — même plus tard dans la même
  session, pas seulement après un redémarrage.

## Exemple : brancher une porte (générique seulement, pas de custom)

```cpp
// ADoor.h — même pas besoin d'implémenter ISaveable pour la position/l'état actif
UCLASS()
class ADoor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	USaveGuidComponent* SaveGuid;
};
```

C'est tout. Place-la dans le niveau, elle est automatiquement suivie en
position et en actif/inactif par `SaveGame()`/`LoadGame()`, et manipulable
à distance par `USaveFrameworkWorldState.SetState()` — sans une ligne de code
supplémentaire.

## Exemple : ajouter du custom (un état "ouvert" propre à la porte)

```cpp
// FDoorSaveData.h — struct spécifique au JEU, PAS dans le plugin
USTRUCT()
struct FDoorSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bOpen = false;
};

// ADoor.h
UCLASS()
class ADoor : public AActor, public ISaveable
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	USaveGuidComponent* SaveGuid;

	UPROPERTY()
	bool bOpen = false;

	virtual FInstancedStruct CaptureState_Implementation() const override
	{
		FDoorSaveData Data;
		Data.bOpen = bOpen;
		return FInstancedStruct::Make(Data);
	}

	virtual void RestoreState_Implementation(const FInstancedStruct& InState) override
	{
		if (const FDoorSaveData* Data = InState.GetPtr<FDoorSaveData>())
		{
			bOpen = Data->bOpen;
			// synchronise le visuel (jouer l'anim déjà ouverte, etc.)
		}
	}
};
```
