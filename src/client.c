#include "../hd/client.h"



int client(void)
{
	
	struct _client* client = (struct _client*)malloc(sizeof (struct _client));
	if (!client)
	{
		perror("failed alloc client");
		return EXIT_FAILURE;
	}
	
	client->port = PORT;
	
	client->sock_param = (struct socket_param*)malloc(sizeof (struct socket_param));
	if (!client->sock_param)
	{
		perror("failed alloc sock_param");
		return EXIT_FAILURE;
	}
	
	client->sock_param->domain = 	AF_INET;
	client->sock_param->type = 	SOCK_DGRAM;
	client->sock_param->protocol = 	0;
	
	struct _packet* packet = (struct _packet*)malloc(sizeof (struct _packet));
	if (!packet)
	{
		perror("failed alloc packet");
		return EXIT_FAILURE;
	}
	
	packet->packet_source = (struct sockaddr_in*)malloc(sizeof (struct sockaddr_in));
	if (!packet->packet_source)
	{
		perror("failed alloc packet_source");
		return EXIT_FAILURE;
	}
	
	packet->server_packet_source = (struct sockaddr_in*)malloc(sizeof (struct sockaddr_in));
	if (!packet->packet_source)
	{
		perror("failed alloc server_packet_source");
		return EXIT_FAILURE;
	}
	
	packet->packet_source_len = sizeof(struct sockaddr_in);
    	packet->server_packet_source_len = sizeof(struct sockaddr_in);
	
	packet->buffer = (char*)malloc(sizeof (char) * BUFFER_SIZE);
	if (!packet->buffer)
	{
		perror("failed alloc packet_buffer");
		return EXIT_FAILURE;
	}
	
	
	memset(packet->server_packet_source, 0, packet->server_packet_source_len);
    	packet->server_packet_source->sin_family = AF_INET;
    	packet->server_packet_source->sin_port = htons(PORT);
    	packet->server_packet_source->sin_addr.s_addr = INADDR_ANY;
	
			
	client->sockfd = socket(client->sock_param->domain, client->sock_param->type, client->sock_param->protocol);
	if (client->sockfd < 0)
	{
		perror("failed create socket");
		return EXIT_FAILURE;
	}
	
	if (bind(client->sockfd, (const struct sockaddr*)packet->server_packet_source, packet->server_packet_source_len) < 0)
	{
		perror("failed bind socket");
        	close(client->sockfd);
        	return EXIT_FAILURE;
	}
	
	LOG("Client active\n");
	
	
	const char* request = "ready";
	const char* response = "ready";
	
	while (true)
	{
		ssize_t received_bytes = recvfrom(client->sockfd, packet->buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)packet->packet_source, &packet->packet_source_len);
		if (received_bytes < 0)
		{
			perror("failed receive bytes");
		    	break;
		}
		
		packet->buffer[received_bytes] = '\0';
		LOG("Receive message: %s from %s\n", packet->buffer, inet_ntoa(packet->packet_source->sin_addr));
		if (strcmp(packet->buffer, request) == 0)
		{
			sendto(client->sockfd, response, strlen(response), 0, (struct sockaddr*)packet->packet_source, packet->packet_source_len);
			LOG("Response message: %s to %s\n", response, inet_ntoa(packet->packet_source->sin_addr));
		} 
	}
		
	free(packet->buffer);
    	free(packet->packet_source);
    	free(packet->server_packet_source);
    	free(packet);
    	close(client->sockfd);
    	free(client);
	
	return EXIT_SUCCESS;
}






















