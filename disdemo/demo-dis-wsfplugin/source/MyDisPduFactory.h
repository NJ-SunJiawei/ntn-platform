#pragma once

#include "WsfDisPduFactory.hpp"

class MyDisPduFactory : public WsfDisPduFactory
{
public:
	MyDisPduFactory(WsfDisInterface* aInterfacePtr);

	virtual DisPdu *UserCreate(DisPdu& aDisPdu, GenI& aGenI) override;
};