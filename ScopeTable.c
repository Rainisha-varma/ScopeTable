#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

#define TABLE_SIZE 100

typedef int data;

typedef struct Symbol_tag                    //Symbol is a node storing the variable name and data value;
{
    char* name;
    data value;
    struct Symbol_tag *next;
} Symbol;

typedef struct Scope_tag                     //Scope contains symbol table for that particular scope and it is connected 
{                                            //to previous scope so if a variable is not found in current scope it looks up in
    Symbol *Symbol_table[TABLE_SIZE];        //previous scope
    struct Scope_tag *parent;
}  Scope;

Scope *curr_scope = NULL;                    //Making the current scope as global so can be accessed from anywhere

int hash_function(char *var_name)            //hash function is to decide the index of a symbol in symbol tree
{
    int hash_val=3;                          //Using DJB2 hash function and initializing by an arbitrary prime value
    while(*var_name)                         
    {
        hash_val=(hash_val*33)+(*var_name);  //33 is an odd  number which helps in evenly distribution of symbols
        *var_name++;
    }
    
    return hash_val%TABLE_SIZE;
}

void Initialize_scope()                      //This function initializes a new scope whenever we have a new scope
{
    Scope *new_scope=(Scope *)malloc(sizeof(Scope));
    if(new_scope == NULL)
    {
         printf("MEMORY ALLOCATION FOR NEW SCOPE IS FAILED\n");
    }
    else
    {
        new_scope->parent=curr_scope;         //Keeping it connected with the previous scope
        curr_scope=new_scope;
        for(int i=0 ; i< TABLE_SIZE ; i++)    //Initially making all the pointers NULL in the symbol table
        {
             curr_scope->Symbol_table[i]=NULL;
        }
    }
}

void Insert( char *var_name , data val )                        //This function inserts a new symbol in the current scope
{
     Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));   
     
     if(new_symbol == NULL)
     {
          printf("MEMORY ALLOCATION FOR NEW SYMBOL FAILED\n");
     }
     else
     {
          new_symbol->name=strdup(var_name);
          new_symbol->value=val;
          new_symbol->next=NULL;
          
          int Key=hash_function(var_name);                       //It inserts at the index given by hash function so they will be
                                                                 //evenly distributed
          new_symbol->next=curr_scope->Symbol_table[Key];        //Inserting the new symbol at the start of the linked list of 
          curr_scope->Symbol_table[Key]=new_symbol;              //symbols already present at that particular index
     }
}

data LookUp(char *var_name)                                      //This function looks up for a value of a variable in all the symbol 
{                                                                //tables
       Symbol *temp_symbol;
       Scope *temp_scope=curr_scope;
       
       int Key=hash_function(var_name);                          //From the hash function we know at which index of the symbol table
                                                                 //this variable is present
       int found=0;
       
       data retVal=INT_MIN;                                      //if we didn't find any symbol named as the given string then return 
                                                                 //minimum possible value of integer
       while( temp_scope !=NULL && found == 0)
       {
            temp_symbol=temp_scope->Symbol_table[Key];
            while(temp_symbol != NULL && found == 0)
            {
                if(strcmp(var_name,temp_symbol->name)==0)            //Checks in the linked list present at the index
                {
                      found=1;
                      retVal=temp_symbol->value;
                }
                else
                {
                      temp_symbol=temp_symbol->next;
                }
            }
            
            temp_scope=temp_scope->parent;                       //If not present at current scope goes to previous scope
       }
       if(temp_symbol==NULL)                                     //If there is no symbol named as the given string
       {
            printf("FIRST USE OF UNDECLARED VARIABLE %s \n",var_name);
       }
       return retVal;
}
       
void Finalyze_Scope()                                            //When a scope ends we delete it
{
    Scope *temp_scope=curr_scope;
    curr_scope=curr_scope->parent;                               //current scope will become its previous scope
    Symbol *temp_symbol,*free_symbol;
    
    for ( int i=0 ; i<TABLE_SIZE ; i++ )
    {
        temp_symbol=temp_scope->Symbol_table[i];
        while(temp_symbol != NULL)                               //freeing all the space allotted for symbols in the current scope
        {
             free_symbol=temp_symbol;
             temp_symbol=temp_symbol->next;
             free(free_symbol);
        }
    }
    
    free(temp_scope);                                            //freeing the scope itself
    
} 
    
int main()
{
    //printf("MAIN\n");
    FILE *fptr=fopen("inputassign3.txt","r");
    if(fptr==NULL)
    {
         printf("No such file exists\n");
         return -1;
    }
    char word[100],var_name[50];
    int value;
    //printf("before while");
    while(fscanf(fptr,"%s",word) != EOF)
    {
        
            //printf("ELSE\n");
                                       

            if(strcmp(word,"begin")==0)
            {
                 //printf("Initialize\n");
                 Initialize_scope();
            }
            else if(strcmp(word,"assign")==0)
            {
                 fscanf(fptr,"%s",var_name);
                 fscanf(fptr,"%d",&value);
                 Insert(var_name,value);
            }
            else if(strcmp(word,"print")==0)
            {
                 fscanf(fptr,"%s",var_name);
                 value=LookUp(var_name);
                 printf("%d\n",value);
            }
            else if(strcmp(word,"end")==0)
            {
                 Finalyze_Scope();
            }
            else
            {
                  printf("No such function exists\n");
            }
            
        
     }
     fclose(fptr);
     return 0;
}
    
