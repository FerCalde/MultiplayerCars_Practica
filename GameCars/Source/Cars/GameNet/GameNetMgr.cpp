// Fill out your copyright notice in the Description page of Project Settings.


#include "GameNet/GameNetMgr.h"
#include "GameNet/GameBuffer.h"
#include "GameNet/NetComponent.h"
#include "Game/Car.h"
#include "Net/buffer.h"
#include "Net/packet.h"
#include "CarsGameInstance.h"

#include "Kismet/GameplayStatics.h"

CGameNetMgr::CGameNetMgr()
{
	Net::CManager::Init();
	m_pManager = Net::CManager::getSingletonPtr();
	m_pManager->addObserver(this);
}

CGameNetMgr::CGameNetMgr(UCarsGameInstance* _pOwner)
	: m_pCarsGameInstance(_pOwner)
{
	Net::CManager::Init();
	m_pManager = Net::CManager::getSingletonPtr();
	m_pManager->addObserver(this);
}

CGameNetMgr::~CGameNetMgr()
{
	m_pManager->removeObserver(this);
	Net::CManager::Release();
	m_pManager = nullptr;
}

void CGameNetMgr::dataPacketReceived(Net::CPacket* packet)
{
	CGameBuffer oData; //Buffer to have the msg from package
	oData.write(packet->getData(), packet->getDataLength()); //write the buffer with the package data
	oData.reset(); //Reset pointer to begin of the buffer
	Net::NetMessageType iID; //To read the first param of the package data
	oData.read(iID);

	switch (iID)
	{
	case Net::COMMAND:
	{

	}
	break;


	case Net::START_GAME:
	{

	}
	break;

	case Net::END_GAME:
	{

	}
	break;

	case Net::LOAD_MAP:
	{
		//the 2nd param when the package is a start game package is the level to Load
		char sLevel[32];
		oData.read(sLevel);
		UGameplayStatics::OpenLevel(m_pCarsGameInstance->GetWorld(), sLevel);

		//Respuesta Client to Server when the map is Loaded
		Net::NetMessageType iResponseID = Net::NetMessageType::MAP_LOADED;
		CGameBuffer oDataMapLoadedResponse;
		oDataMapLoadedResponse.write(iResponseID);
		m_pManager->send(&oDataMapLoadedResponse, true);
	}
	break;

	case Net::MAP_LOADED:
	{
		//Suponemos que los mensajes están llegando bien a su destino, este mensaje solo le deberia interesar al server
		//Deberia guardar un map con los clientes y su carga de mapa (Mejorar)
		++m_uMapLoadedNotifications;
		if (m_uMapLoadedNotifications >= m_pManager->getConnections().size())
		{
	#pragma region CreateCarForServer
			/*****
			//TAKE OUT COMMENT FOR CREATE CAR FOR SERVER AND LET HIM PLAY

			//For Server also Play as a "Client" (SpawnCar and asignate Controller) Create that car also in all the Clients
			CGameBuffer oDataLoadServerPlayer;
			Net::NetMessageType iType = Net::LOAD_PLAYER;
			oDataLoadServerPlayer.write(iType);

			oDataLoadServerPlayer.write(Net::ID::SERVER); //devuelvo el ID del server
			FVector vPos(220.f, -310.f, 0.f); //Location to spawn the players of the Server. NO Distancio un offset (solo por cada cliente) 
			oDataLoadServerPlayer.write(vPos);

			CreateCar(Net::ID::SERVER, vPos); //Cargar un coche para el Server
			m_pManager->send(&oDataLoadServerPlayer, true);
			*******/
	#pragma endregion CreateCarForServer
			
			
	#pragma region CreateCarsForClients
			for (auto& rClient : m_pManager->getConnections())
			{
				CGameBuffer oDataLoadPlayer;
				Net::NetMessageType iType = Net::LOAD_PLAYER;
				oDataLoadPlayer.write(iType);

				oDataLoadPlayer.write(rClient.first); //devuelvo el ID del cliente que envia el mensaje, primer parametro del map
				FVector vPos(220.f, -310.f + 40.f * rClient.first, 0.f); //Location to spawn the players of the clients. Distancio un offset por cada cliente 
				oDataLoadPlayer.write(vPos);

				CreateCar(rClient.first, vPos); //Cargar un coche por cada Cliente
				m_pManager->send(&oDataLoadPlayer, true);
			}
	#pragma endregion CreateCarsForClients
			
			
		}
	}
	break;

	case Net::LOAD_PLAYER:
	{
		unsigned int uClient;
		oData.read(uClient);
		FVector vPos;
		oData.read(vPos);
		CreateCar(uClient, vPos);
	}
	break;

	case Net::ENTITY_MSG:
	{
		//read ID who sent the package 
		Net::NetID uID;
		oData.read(uID);
		
		ACar* pCar = m_tPlayers[uID];
		pCar->GetNetComponent()->DeserializeData(&oData);
		
		/*if (m_pManager->getID() == Net::ID::SERVER)
		{
			pCar->GetNetComponent()->DeserializeData(&oData);
			//FVector2D vInput;
			//oData.read(vInput);
			//pCar->GetCarMovementComponent()->SetInput(vInput);
		}*/
	}
	break;

	/*
	case Net::PLAYER_LOADED:
	{
		
	}
	break;



	case Net::ASSIGNED_ID:
	{

	}
	break;*/
	default:
		break;
	}

	//if (m_pManager && m_pManager->getID() == Net::ID::SERVER)
	//{
	//	
	//}
	//else
	//{
	//	Net::CBuffer oData;
	//	oData.write(packet->getData(), packet->getDataLength());
	//	oData.reset();
	//	char sInfo[32];
	//	oData.read(sInfo);
	//	//oData.read(sInfo, oData.getSize());
	//	
	//	if (GEngine)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, sInfo);
	//	}
	//	
	//}
}

void CGameNetMgr::connectionPacketReceived(Net::CPacket* packet)
{
	if (m_pManager && m_pManager->getID() == Net::ID::SERVER)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Connection packet received: Client Connected!"));
		}

		//Net::CBuffer oData;
		//const char* sHello = "Connected!";
		//oData.write(sHello);
		////oData.write(sHello, strlen(sHello));

		//m_pManager->send(&oData, true);
	}

}

void CGameNetMgr::disconnectionPacketReceived(Net::CPacket* packet)
{
	++m_uMapLoadedNotifications; //MEJOR CON LA TABLA!
	//CONTROLARIA TODO MEJOR. PARA CADA CLIENTE HAY QUE CONTROLAR SI ESTA CONECTADO O NO. y asi no cuenta doble un cliente que ya se habria conectado antes.
	//Ademas permitiria controlar que el cliente que se desconecta en medio de una partida para borrar su coche o ponerle IA hasta que vuelva a conectarse
}

void CGameNetMgr::CreateCar(unsigned int _uClient, FVector _vPos)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Name = FName("Car", _uClient);
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Da igual con quien colisiones, siempre spawnea
	
	ACar* pCar = m_pCarsGameInstance->GetWorld()->SpawnActor<ACar>(_vPos, FRotator::ZeroRotator, SpawnInfo);
	
	if (pCar)
	{
		if (pCar->GetNetComponent())
		{
			pCar->GetNetComponent()->SetID(_uClient);
		}
		m_tPlayers[_uClient] = pCar;
		
		if (_uClient == m_pManager->getID()) //es el mismo cliente que esta conectado
		{
			APlayerController* pPC = GEngine->GetFirstLocalPlayerController(m_pCarsGameInstance->GetWorld());
			if (pPC)
			{
				pPC->Possess(pCar);
			}
		}
		
	}
	/*ACar* pCar = Cast<ACar>(UGameplayStatics::BeginDeferredActorSpawnFromClass(m_pCarsGameInstance->GetWorld(), ACar::StaticClass(), _vPos));
	if (pCar)
	{
		pCar->SetOwner(_uClient);
		pCar->SetNetMgr(this);
		pCar->FinishSpawning(_vPos);
		m_mCars[_uClient] = pCar;
	}*/
}
