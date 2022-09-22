#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "connection.h"


class Controller {
public:
	Controller(Connection& conn) : conn(&conn) {}
	
	static void handleDataCallback(std::vector<char>& buf, void* context) {
		reinterpret_cast<Controller*>(context)->handleData(buf);
	}

	void handleData(std::vector<char>& buf);
	void ret(int code);
	void sendOut(std::string out);
	void sendErr(std::string err);
	void sendBuffer(std::vector<char>& data);

	void loop();

private:
	Connection* conn;
};

#endif