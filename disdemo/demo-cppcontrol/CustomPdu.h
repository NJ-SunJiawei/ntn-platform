#pragma once

#include "dis6/SimulationManagementFamilyPdu.h"

class CustomPdu : public DIS::SimulationManagementFamilyPdu
{
protected:
	unsigned int _type; // 0 ÔÝÍ££¬1 »Ö¸´

public:
	CustomPdu();
	virtual ~CustomPdu();

	virtual void marshal(DIS::DataStream& dataStream) const;
	virtual void unmarshal(DIS::DataStream& dataStream);

	unsigned int getType() const;
	void setType(unsigned int type);

	virtual int getMarshalledSize() const;

	bool operator==(const CustomPdu& rhs) const;
};