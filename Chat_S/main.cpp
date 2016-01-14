#include "Serv.h"

inline void Server::NewClient()
{
	int newSocket=accept(Listener, NULL, NULL);
	if (newSocket < 0)
	{	// если произошла ошибка
		perror("error accept");
		exit(3);
	}
	fcntl(newSocket, F_SETFL, O_NONBLOCK);
		// добавляем нового клиента
	ClientData newClientData;
	newClientData.isRegistered_=false;
	Clients.insert(pair<int, ClientData>(newSocket, newClientData));
}

inline void Server::readClients()
{
	recMess.clear();		
	for (Iterator = Clients.begin(); 
		 Iterator != Clients.end();
		 Iterator++)
	{	// для каждого клиента
		if (FD_ISSET(Iterator->first, &socketsRead))
		{
			SenderClientMsg* newSenderMsg = new SenderClientMsg;
			int bytes_read = recv(Iterator->first, newSenderMsg, sizeof(SenderClientMsg), MSG_WAITALL);
			if (bytes_read <= 0)
			{
				clientConnectionDrop();
				if (Clients.empty())
					break;
				continue;
			}
			
			switch (newSenderMsg->msgType_)
			{	
			case NICK_REGISTRATION_REQUEST:
					// если пришел запрос на регистрацию
				if (registerNick(newSenderMsg->clientNickName_))
				{	
					RecieverClientMsg* regMsg = new RecieverClientMsg(serverLocalTime_);
					strcpy(regMsg->message_, "Welcome to chat");
					regMsg->msgType_=NICK_REGISTRATION_SUCCESS;
						// отсылаем клиенту сообщение
					send(Iterator->first, (void*)regMsg, sizeof(RecieverClientMsg), 0);
					RecieverClientMsg* newRecieverMsg = new RecieverClientMsg(serverLocalTime_);
					strcat(newRecieverMsg->message_, newSenderMsg->clientNickName_);
					strcat(newRecieverMsg->message_, "  join to chat!");
					recMess.push_back(newRecieverMsg);
					delete regMsg;
				}
				else
				{	
					RecieverClientMsg* regMsg = new RecieverClientMsg(serverLocalTime_);
					strcpy(regMsg->message_, "This nick is used!");
					regMsg->msgType_=NICK_REGISTRATION_FAIL;
						// отсылаем сообщение клиенту
					send(Iterator->first, (void*)regMsg, sizeof(RecieverClientMsg), 0);
					delete regMsg;
				}
			break;

			case NORMAL_CHAT_MSG: 
				// если это обычное сообщение
				RecieverClientMsg* newRecieverMsg = new RecieverClientMsg(Iterator->second.clientNickName_ ,newSenderMsg->message_, serverLocalTime_);
				recMess.push_back(newRecieverMsg);	
			break;
			} 
		}	
	}
	ClientMess.insert(ClientMess.end(),recMess.begin(),recMess.end());
}

void Server::clientConnectionDrop()
{
		
	if (Iterator->second.isRegistered_)
	{
		RecieverClientMsg* newRecieverMsg = new RecieverClientMsg(serverLocalTime_);
		strcpy(newRecieverMsg->message_, "SERVER: ");
		strcat(newRecieverMsg->message_, Iterator->second.clientNickName_);
		strcat(newRecieverMsg->message_, " exit\n");
		recMess.push_back(newRecieverMsg);	
	}
		// удаляем данные о клиенте
	close(Iterator->first);
	map <int, ClientData>::iterator iter=Iterator;
	iter--;
	Clients.erase(Iterator);
	Iterator=iter;	
}

bool Server::registerNick(char *regClientNickName)
{
	map<int, ClientData>::iterator iter;
	for (iter = Clients.begin(); iter != Clients.end(); iter++)
	{	// ищем клиента с совпадающим ником
		if (strcmp(regClientNickName, iter->second.clientNickName_) == 0)
			return false;
	}
		// регистрация
	strcpy(Iterator->second.clientNickName_, regClientNickName);
	Iterator->second.isRegistered_=true;
	return true;
	
}

inline void Server::SendMes()
{
	for (Iterator = Clients.begin(); 
		 Iterator != Clients.end();
		 Iterator++)
	{ // для каждого подключенного клиента
		for (cMsgIter = recMess.begin();
			 cMsgIter != recMess.end();
			 cMsgIter++) // высылаем принятые сообщения
		{
			if (Iterator->second.isRegistered_)
				send(Iterator->first, (void*)(*cMsgIter), sizeof(RecieverClientMsg), 0);
		}
	} 
}

int main(int argc, const char * argv[])
{
	Server serv;
        printf("Server works on %d port", PORT);
	serv.BeginChat();
}