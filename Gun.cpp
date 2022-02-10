// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


#define OUT

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);


}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::PullTrigger()
{
	/*UE_LOG(LogTemp, Warning, TEXT("You've been shot!"));*/

	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* OwnerController = OwnerPawn->GetController();
	if (OwnerController == nullptr) return;

OUT	FVector Location;
OUT	FRotator Rotation;

	OwnerController->GetPlayerViewPoint(Location, Rotation);
	
	FVector End = Location + Rotation.Vector() * MaxRange;
	//todo linetrace

	/*DrawDebugCamera(GetWorld(), Location, Rotation), 90.f, 2, FColor::Red, true);*/

	OUT FHitResult Hit;
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);

	if (bSuccess)
	{
		FVector ShotDirection = -Rotation.Vector();
		/*DrawDebugPoint(GetWorld(), Hit.Location, 20, FColor::Red, true);*/
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactPoint, Hit.Location, ShotDirection.Rotation());

		AActor* HitActor = Hit.GetActor();

		if (HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
		}


	}

	else {}
}




/**Trace a ray against the world using object typesand return the first blocking hit
	* @param  OutHit          First blocking hit found
	* @param  Start           Start location of the ray
	* @param  End             End location of the ray
	* @param	ObjectQueryParams	List of object types it's looking for
	* @param  Params          Additional parameters used for the trace
	* @return TRUE if any hit is found*/