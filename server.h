#ifndef __CUSTOMERS__

#define DEBUG_ACCOUNTF "accountFile"
#define DEBUG_CLIENTF "clientFile"
#define WELCOME_CLIENT_MESSAGE "Welcome to your online bank!""\nYou can type the following commands:\n\t""- Connect {id} {password} :\n\t\tConnect yourself ""(provides for every {id] and {password}""\n\t- Add {id} {account_id} {password} {amount} :\n\t\t""Add {amount} to {accound_id}\n\t-"" Withdraw {id} {account_id} {password} {amount} :\n\t\t""Withdraw {amount} from {accound_id}\n\t- Balance {id}"" {account_id} {password} :\n\t\tCheck the balance of {account_id}\n\t""- Operations {id} {account_id} {password} :\n\t\t""Print the last ten operations on {account_id}";
#define NB_MAX_CO 10
#endif