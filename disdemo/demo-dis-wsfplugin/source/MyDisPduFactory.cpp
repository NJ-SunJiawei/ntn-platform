#include "MyDisPduFactory.h"
#include "DisPduEnums.hpp"

#include "MyCustomPdu.h"
#include "MyDisActionRequest.h"

MyDisPduFactory::MyDisPduFactory(WsfDisInterface* aInterfacePtr)
	: WsfDisPduFactory(aInterfacePtr)
{
	SetPduCreateFunction(129, new CreateFunctionT<MyCustomPdu>);
}

DisPdu* MyDisPduFactory::UserCreate(DisPdu& aDisPdu, GenI& aGenI)
{
	if (aDisPdu.GetPduType() == DisEnum::Pdu::Type::ActionRequest) 
	{
		return new MyDisActionRequest(aDisPdu, aGenI, mInterfacePtr);
	}
	else if (aDisPdu.GetPduType() == 129) // CustomPDU
	{
		return new MyCustomPdu(aDisPdu, aGenI, mInterfacePtr);
	}
	return nullptr;
}

