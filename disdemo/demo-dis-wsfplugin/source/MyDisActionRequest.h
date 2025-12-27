#pragma once

#include "WsfDisActionRequest.hpp"

class MyDisActionRequest : public WsfDisActionRequest
{
public:
	MyDisActionRequest(WsfDisInterface* aInterfacePtr);
	MyDisActionRequest(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);

	int Process() override;

protected:
	WsfDisInterface* mInterfacePtr = nullptr;
};