#ifndef DAO_DATA_ACCESS_OBJECT_H
#define DAO_DATA_ACCESS_OBJECT_H

#include "network/HttpClient.h"

class IDao{
public:
	virtual ~IDao(){};

	// Create
	virtual void Create(std::string requestString, std::string url) = 0;

	// Read
	virtual void Read(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) = 0;

	// Update
	virtual void Update(std::string requestString) = 0;

	// Delete
	virtual void Del() = 0;
private:
};

/*
Remote MySQL database Implementation. Communicates with the remote
apache server to store game analytics data into a MySQL database.
*/
class Dao : public IDao
{
public:
	virtual ~Dao(){};

	// Create user in database
	void Create(std::string requestString, std::string url);

	/*
	Read http response from the server

	@param cocos2d::network::HttpClient *sender, the calling function on callback
	@param cocos2d::network::HttpResponse *response, the response message from the server
	*/
	void Read(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);

	/*
	Update the remote database

	@param std::string requestString, server request message
	*/
	void Update(std::string requestString);

	// Delete data from remote database
	void Del();
private:
	bool m_bAccessGranted;

};

#endif
