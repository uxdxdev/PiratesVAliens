#include "dao/data_access_object.h"
#include "manager/world_manager.h"

// MySQL Database Implemntation
void Dao::Create(std::string requestString, std::string url)
{
	// Remote MySQL
	// Update the remote database with POST request
	cocos2d::network::HttpRequest* remoteRequest = new (std::nothrow) cocos2d::network::HttpRequest();

	// Set the remote database URL
	remoteRequest->setUrl(url.c_str());

	// Request type is POST
	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded");
	remoteRequest->setHeaders(headers);
	remoteRequest->setRequestType(cocos2d::network::HttpRequest::Type::POST);	
	remoteRequest->setResponseCallback(CC_CALLBACK_2(Dao::Read, this));
	remoteRequest->setRequestData(requestString.c_str(), requestString.length());

	// Send the request to the PHP script on the server
	cocos2d::network::HttpClient::getInstance()->send(remoteRequest);
	//CCLOG("%s", requestString.c_str());	
	remoteRequest->release();
	/*
	// Local MySQL
	// Update the remote database with POST request
	cocos2d::network::HttpRequest* localRequest = new (std::nothrow) cocos2d::network::HttpRequest();

	// Set the remote database URL
	localRequest->setUrl("http://localhost/update_db.php/");

	// Request type is POST
	localRequest->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	localRequest->setResponseCallback(CC_CALLBACK_2(GameDAOMySQL::read, this));
	localRequest->setRequestData(requestString.c_str(), requestString.length());

	// Send the request to the PHP script on the server
	cocos2d::network::HttpClient::getInstance()->send(localRequest);
	//CCLOG("%s", requestString.c_str());
	localRequest->release();
	*/
}

void Dao::Read(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
	// Read data from HTTPResponse message	
	if (response->getResponseData()->size() > 0) {
		std::vector<char> *data = response->getResponseData();
		std::string responseString(&(data->front()), data->size());
		CCLOG("%s", ("Response message: " + responseString).c_str());
		if (responseString == "rejected")
		{
			WorldManager::getInstance()->SetAccessGranted(true); // allow access to DB for updates
			WorldManager::getInstance()->GetGameOverScene()->UpdateLoginLabel("username not valid");
		}
		else if (responseString == "accepted")
		{
			// Username available
			WorldManager::getInstance()->SetAccessGranted(true);
			WorldManager::getInstance()->GetGameOverScene()->UpdateLoginLabel("unique");
		}
		else if (responseString == "exists")
		{
			// Username available
			WorldManager::getInstance()->SetAccessGranted(true);
			WorldManager::getInstance()->GetGameOverScene()->UpdateLoginLabel("already exists");
		}

		// Check for the substring 'rank'
		if (responseString.find("Rank") != std::string::npos)
		{
			// Get rank from substring
			std::stringstream ss;
			std::string usernameLabel;
			std::string username;
			std::string rankLabel;
			std::string scoreLabel;
			int playerRank = 0;
			int playerScore = 0;
			ss << responseString;
			ss >> usernameLabel >> username >> rankLabel >> playerRank >> scoreLabel >> playerScore;
			
			PlayerStats playerStats;
			playerStats.playerUsername = username;
			playerStats.playerRank = playerRank;
			playerStats.playerScore = playerScore;
			WorldManager::getInstance()->GetGameOverScene()->UpdatePlayerStats(playerStats);
		}		
	}	
}

void Dao::Update(std::string requestString)
{
	// Remote MySQL
	// Update the remote database with POST request
	cocos2d::network::HttpRequest* remoteRequest = new (std::nothrow) cocos2d::network::HttpRequest();

	// Set the remote database URL
	remoteRequest->setUrl("http://piratesvsaliens.azurewebsites.net/update_db.php/");
	
	// Request type is POST
	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded");
	remoteRequest->setHeaders(headers);
	remoteRequest->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	remoteRequest->setResponseCallback(CC_CALLBACK_2(Dao::Read, this));
	remoteRequest->setRequestData(requestString.c_str(), requestString.length());

	// Send the request to the PHP script on the server
	cocos2d::network::HttpClient::getInstance()->send(remoteRequest);
	//CCLOG("%s", requestString.c_str());	
	remoteRequest->release();
	/*
	// Local MySQL
	// Update the remote database with POST request
	cocos2d::network::HttpRequest* localRequest = new (std::nothrow) cocos2d::network::HttpRequest();

	// Set the remote database URL
	localRequest->setUrl("http://localhost/update_db.php/");

	// Request type is POST
	localRequest->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	localRequest->setResponseCallback(CC_CALLBACK_2(GameDAOMySQL::read, this));
	localRequest->setRequestData(requestString.c_str(), requestString.length());

	// Send the request to the PHP script on the server
	cocos2d::network::HttpClient::getInstance()->send(localRequest);
	//CCLOG("%s", requestString.c_str());
	localRequest->release();
	*/
}

void Dao::Del()
{
	// Delete record from the remote database
}

