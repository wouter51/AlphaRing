#pragma once

class i_manager {
public:
	virtual int initialize() = 0;
	virtual int shutdown() = 0;
};
