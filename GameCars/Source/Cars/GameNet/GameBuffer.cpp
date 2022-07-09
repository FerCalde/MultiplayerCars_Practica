// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBuffer.h"

CGameBuffer::CGameBuffer(size_t initsize, size_t delta)
	: Net::CBuffer(initsize, delta)
{
	
}

CGameBuffer::~CGameBuffer()
{
	
}

void CGameBuffer::write(const FVector& _data)
{
	write(_data.X);
	write(_data.Y);
	write(_data.Z);
}

void CGameBuffer::read(FVector& data_)
{
	read(data_.X);
	read(data_.Y);
	read(data_.Z);
}
void CGameBuffer::write(const FVector2D& _data)
{
	write(_data.X);
	write(_data.Y);
}


void CGameBuffer::read(FVector2D& data_)
{
	read(data_.X);
	read(data_.Y);
}

void CGameBuffer::write(const FTransform& _data)
{
	//FVector_NetQuantize vPos = _data.GetLocation(); esta optimizado para cosas de red

	//No necesitamos los 14 floats que da el transform
	write(_data.GetLocation());
	write(_data.GetRotation().GetAxisX());	//solo necesito conocer el forward del objeto para saber su rotacion	
											//(ahorra datos en red y opero con CPU)
	write(_data.GetScale3D());
}

void CGameBuffer::read(FTransform& _data)
{
	FVector vPos;
	read(vPos);
	FVector vDir;
	read(vDir);

	FVector vScale;
	read(vScale);

	//Reasigno a Matrix para Transform
	FMatrix tMatrix = FRotationMatrix::MakeFromX(vDir); //Creo la rotacion a partir del vector Dir
	_data.SetFromMatrix(tMatrix);
	_data.SetLocation(vPos);
	_data.SetScale3D(vScale);
}
