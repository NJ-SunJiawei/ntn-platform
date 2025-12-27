#include "CustomPdu.h"

CustomPdu::CustomPdu()
	: SimulationManagementFamilyPdu()
	, _type(0)
{
	setPduType(129);
}

CustomPdu::~CustomPdu()
{

}

void CustomPdu::marshal(DIS::DataStream& dataStream) const
{
	SimulationManagementFamilyPdu::marshal(dataStream);
	dataStream << _type;
}

void CustomPdu::unmarshal(DIS::DataStream& dataStream)
{
	SimulationManagementFamilyPdu::unmarshal(dataStream);
	dataStream >> _type;
}

unsigned int CustomPdu::getType() const
{
	return _type;
}

void CustomPdu::setType(unsigned int type)
{
	_type = type;
}

int CustomPdu::getMarshalledSize() const
{
	int marshalSize = 0;

	marshalSize = SimulationManagementFamilyPdu::getMarshalledSize();
	marshalSize = marshalSize + 4;  // _type

	return marshalSize;
}

bool CustomPdu::operator==(const CustomPdu& rhs) const
{
	bool ivarsEqual = true;

	ivarsEqual = SimulationManagementFamilyPdu::operator==(rhs);

	if (!(_type == rhs._type)) ivarsEqual = false;

	return ivarsEqual;
}

