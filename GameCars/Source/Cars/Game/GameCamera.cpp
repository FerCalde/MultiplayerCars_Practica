// Fill out your copyright notice in the Description page of Project Settings.

#include "GameCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Car.h"

// Sets default values
AGameCamera::AGameCamera()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGameCamera::BeginPlay()
{
  Super::BeginPlay();
  /*
  *Just Valid for Monoplayer Game
  APlayerController* OurPC = UGameplayStatics::GetPlayerController(this, 0);
  if (OurPC)
  {
    OurPC->SetViewTarget(this);
    m_pTarget = OurPC->GetPawn<ACar>();
  }*/
}

void AGameCamera::SetTarget()
{
    APlayerController* OurPC = UGameplayStatics::GetPlayerController(this, 0);
    if (OurPC)
    {
        OurPC->SetViewTarget(this);
        m_pTarget = OurPC->GetPawn<ACar>();
    }
}

// Called every frame
void AGameCamera::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  if (m_pTarget)
  {
      FVector vNewPos = m_pTarget->GetActorLocation();
      vNewPos.Z += m_fMinDistance +
          m_fDistanceFromVelocityFactor *
          m_pTarget->GetCarMovementComponent()->GetVelocityMagnitude();
      SetActorLocation(vNewPos);
  }
  else 
  {
      SetTarget();
  }
}
