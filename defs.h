
// start of Hacking-network.h
int send_string(int sockfd, unsigned char *buffer)
{
	int sent_btyes, bytes_to_send;
	bytes_to_send = strlen(buffer);
	while(bytes_to_send > 0)
	{
		sent_btyes = send(sockfd, buffer, bytes_to_send, 0);
		if(sent_btyes == -1)
			return 0;
		bytes_to_send -= sent_btyes;
	}
	return 1;
}

int recv_line(int sockfd, unsigned char *dest_buffer)
{
#define EOL "\r\n"
#define EOL_SIZE 2
	unsigned char *ptr;
	int eol_matched = 0;
	
	ptr = dest_buffer;
	while(recv(sockfd, ptr, 1, 0) == 1) //read a single byte
	{
		if(*ptr == EOL[eol_matched])
		{
			eol_matched++;
			if(eol_matched == EOL_SIZE)
			{
				*(ptr+1-EOL_SIZE) = '\0';
				return strlen(dest_buffer);
			}
		}
		else
		{
			eol_matched = 0;
		}
		ptr++; // Increment the pointer to the next byter.
	}
	return 0;
}
//end of hacking-network.h