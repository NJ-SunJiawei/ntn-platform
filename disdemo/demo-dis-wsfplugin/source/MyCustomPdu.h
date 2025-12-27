#pragma once

#include "DisPdu.hpp"
#include "WsfDisInterface.hpp"

class MyCustomPdu : public DisPdu
{
public:
	MyCustomPdu(WsfDisInterface* aInterfacePtr);
	MyCustomPdu(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);

	virtual int Process() override;

private:
	DisUint32 m_type = 0; // 0 ÔÝÍ£, 1 »Ö¸´

protected:
	WsfDisInterface* mInterfacePtr = nullptr;
};