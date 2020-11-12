#ifndef __CUSTOMERS__

#define DEBUG_ACCOUNTF "accountFile"
#define DEBUG_CLIENTF "clientFile"

// Un client à un Id, un password et des ids de compte

typedef struct
{
   char* accountId;
   signed int balance;

}  account;


typedef struct
{
   char* id;
   char* pw;
   int nbAccount;
   account* accounts;
}  customer;


typedef struct{
    
    int nbCustomers;
    customer* c;
}
customerArray ;
int liberateCustomer(customer cus);
int liberateCustomerArray(customer * custs,int nbCustomers );
int getClientsAndAccountFrom(const char* clientFilename,const char* accountFileName,
						 customerArray * custs );
#endif