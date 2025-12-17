#pragma once
#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Weapons/Enum/WeaponsEnum.h"
#include "Weapons/Projectiles/ProjectileBase.h"
#include "ExecutorsSettings.generated.h"


#pragma region SettingsStructBase

USTRUCT(BlueprintType)
struct FAttackExecutorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Projectile")
	FName MuzzleSocketName = TEXT("Muzzle");
	
	UPROPERTY(EditAnywhere, Category="AOE")
	EAttackAreaType AreaType = EAttackAreaType::Single;

	UPROPERTY(EditAnywhere, Category="AOE", meta=(EditCondition="AreaType != EAttackAreaType::Single"))
	float ExplosionRadius = 300.f;

	UPROPERTY(EditAnywhere, Category="AOE", meta=(EditCondition="AreaType == EAttackAreaType::Cone"))
	float ConeAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	UNiagaraSystem* ImpactVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	USoundBase* ImpactSFX;
};
#pragma endregion

#pragma region SettingsStructHitBox

USTRUCT(BlueprintType)
struct FHitBoxExecutorSettings
{
	GENERATED_BODY()
	
};
#pragma endregion

#pragma region SettingsStructLineTrace
USTRUCT(BlueprintType)
struct FLineTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Line Trace")
	float TraceDistance = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Line Trace")
	UNiagaraSystem* TraceVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Line Trace")
	USoundBase* TraceSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	FString ParticleVariable = "None";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Line Trace")
	UNiagaraComponent* TraceParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	UNiagaraSystem* LaserBeamVFX;
};
#pragma endregion

#pragma region SettingsStructProjectile

USTRUCT(BlueprintType)
struct FProjectileExecutorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectileBase> ProjectileClass;
    
	UPROPERTY(EditAnywhere, Category = "Projectile|Spread")
	bool bUseSpread = false;
    
	UPROPERTY(EditAnywhere, Category = "Projectile|Spread", meta=(EditCondition="bUseSpread"))
	float SpreadAngle = 2.f;
    
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* MuzzleFlash;
    
	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	FName MuzzleSocketName;
};
#pragma endregion