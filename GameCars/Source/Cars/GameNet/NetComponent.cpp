// Fill out your copyright notice in the Description page of Project Settings.


#include "GameNet/NetComponent.h"
#include "Net/Manager.h"
#include "DrawDebugHelpers.h"
#include "GameNet/GameBuffer.h"
#include "Game/CarMovementComponent.h"
#include "Game/Car.h"


// Sets default values for this component's properties
UNetComponent::UNetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_pManager = Net::CManager::getSingletonPtr();
	// ...
}


// Called when the game starts
void UNetComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UNetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	//Check NetComponent of Server or Client
	if (m_pManager->getID() == Net::ID::SERVER)
	{
		SerializeData();
		DrawDebugString(GetWorld(), FVector::ZeroVector, *FString("REAL"), GetOwner(), FColor::Blue, 0.02f);
	}
	//Check NetComponent of Client if its the Car Owner 
	else if (m_pManager->getID() == m_uID)
	{
		SerializeData();
		DrawDebugString(GetWorld(), FVector::ZeroVector, *FString("OWNER"), GetOwner(), FColor::Black, 0.02f);
	}
	else
	{
		DrawDebugString(GetWorld(), FVector::ZeroVector, *FString("OTHER PUPPET"), GetOwner(), FColor::Red, 0.02f);
	}

}

void UNetComponent::SerializeData()
{
	CGameBuffer oData;
	Net::NetMessageType eMType = Net::NetMessageType::ENTITY_MSG; //Para mensaje de entidades
	oData.write(eMType);
	oData.write(m_uID);

	if (m_pManager->getID() == Net::ID::SERVER) //Como server
	{
		//Solo mando la Posicion/Rotacion
		//FVector vPos = GetOwner()->GetActorLocation();
		//oData.write(vPos);
		//Solo mando la Posicion/Rotacion mediante la Transform (Optimizado metodo write para enviar 6 bytes en vez de 14
		FTransform vTrans = GetOwner()->GetActorTransform();
		oData.write(vTrans);
		m_pManager->send(&oData, false);
	}
	else //Como cliente
	{
		/*
		//oData.write(eMType);
		//Net::NetID uID = m_pManager->getID(); //Aunque ya tendriamos el m_uID seteado, mejor asi (?)
		//oData.write(m_uID);
		*/

		//Al servidor solo le mando el input
		oData.write(m_vMovementInput);
		m_pManager->send(&oData, false);
	}
}

void UNetComponent::DeserializeData(CGameBuffer* _pData)
{
	ACar* pCar = GetOwner<ACar>();
	
	if (m_pManager->getID() == Net::ID::SERVER) //Server receive Input
	{
		FVector2D vInput;
		_pData->read(vInput);
		pCar->GetCarMovementComponent()->SetInput(vInput);
	}
	else //Client receive Transform //Location
	{
		FTransform vTrans;
		_pData->read(vTrans);
		pCar->SetActorTransform(vTrans);

		//FVector vPos;
		//_pData->read(vPos);
		//pCar->SetActorLocation(vPos);
		//This also will works! ;)
		//pCar->SetActorLocation(vTrans.GetLocation());
		//pCar->SetActorRotation(vTrans.GetRotation());
	}
}

