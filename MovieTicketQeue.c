#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TICKETS 500
#define MAX_TIME 10^9
#define MAX_CUSTOMERS 500000
#define MAX_CHAR 50 

typedef struct Node Node;
typedef struct CustomerData CustomerData;
typedef struct Queue Queue;

//required structs
struct CustomerData { //create customers using dynamid memory
    char * name;
    int numTickets;
    int arrivalTime;
    int assignedQueue;
    int checkOutTime;
};

struct Node {   //storing each customer's data, creating linked list queue
    CustomerData * customer;    //pointing to customer info DATA
    Node * next;        //pointing to a node
};

struct Queue { //storing the linked list queue of customers
    Node * front, * back;   //point to the head node and tail node
    CustomerData * customer;
    int sizequeue;           //storing queue size, will be added by 1 for every customer
    int assignedBooth;      
};

//creating each of 12 queues that may/may not hold customers, also queue of ALL customers
Queue * createQueue() {
    //alocating memory for a new queue
    Queue * newQueue = (Queue *) malloc(sizeof(Queue));
    newQueue->front = newQueue->back = NULL;
    newQueue->sizequeue = 0;
    return newQueue;
}
//creating each node in queues
Node * createNode(CustomerData * customer) {
    //everytime you create a customer you create a node?
    Node * newNode = (Node *) malloc(sizeof(Node));
    newNode->customer = customer; //creating node of that customer's data
    newNode->next = NULL;
    return newNode;
}
//storing data when creating customer for each node
CustomerData * createCustomer(char * name, int numTickets, int arrivalTime) {
    CustomerData * customer = (CustomerData *) malloc(sizeof(CustomerData));
    customer->name = strdup(name);
    customer->numTickets = numTickets;
    customer->arrivalTime = arrivalTime;
    //
    return customer;
}

void removeHead(Queue * q) {
    if(!q || !q->front) return;  //if no queue or no list head then stop

    if(q->front == q->back) { //if head and tail are same node
        free(q->front);
        q->front = q->back = NULL; //telling queue there is no more head or tail
        return;
    }

    Node * newHead = q->front->next; //setting num after front to be new head
    free(q->front); //freeing old head
    q->front = newHead; //having queue point to a new head
}

void dequeue(Queue * q) {
    //need to remove the node entered first, which is the head/front
    if(q) {
        removeHead(q);//calling to remove the head
    }
}
//to use in the enqueue function
Node * insertTail(Node * oldHead, CustomerData * customer) {
    //empty list case, new node created is the head and tail now of that queue
    if (oldHead == NULL) return createNode(customer);

    Node * oldTail = oldHead;
    while(oldTail->next) {
        //move to next node until you reach actual old (tech. current) tail
        oldTail = oldTail->next;
    }
    
    oldTail->next = createNode(customer);

    return oldHead;
}
//adding customers onto their specific queue AND queue of ALL customers
void enqueue(Queue * q, CustomerData * customer) {
    //adding this customer to the end of the queue (or beginning if empty)
    insertTail(q->front, customer);
}
//use this to count number of queues being used when assigning booths
int Empty(Queue * q) { 
    if (q->sizequeue == 0) {
        return 1; //empty queue
    }
    return 0; //non-empty queue
}
//assigning specific queue per customer based on their first letter of name
int QueueNumber (char * name) {
    char firstLetter = name[0];
    int p = firstLetter - 65; //using the char ASCII value to get position on the capital alphabet
    int q = p%13;

    if (q == 0) {
        //the customer is sent to the queue with the least number of people, queue has to have atleast 1 person

    }
    //else the customer is sent to the remainder num of queue
    return q;
}
//returns front of queue
Node * Peek(Queue * q) {
    if(!q || !q->front) return NULL; //if queue is empty or front is NULL, return nothing idk

    Node * queueFront = q->front;
    return queueFront; //returning pointer that points to head 
}

int Size (Queue * q) {
    return q->sizequeue;
}
//freeing memory of queues
void deleteQueue(Queue * q) {
    free(q);
}

int main() {
    //initializing 12 queues right before customers enter
    Queue * q[12];
    for (int i = 0; i < 12; i++) {
        q[i] = createQueue();
    }
    
    int n, b; //number of customers and booths available, respectively 
    scanf("%i %i", &n, &b);

    //creating giant queue of all customers to maintain order of arrival time
    Queue * masterList[n];
    for (int i = 0; i < n; i++) {
        masterList[i] = createQueue();
    }
    
    char name[MAX_CHAR + 1];
    int numTickets, arrivalTime;
    

    // creating each customer and assigning them to a QUEUE
    for (int i = 0; i < n; i++) {
        scanf("%s %d %d", name, &numTickets, &arrivalTime);
        CustomerData * customer = createCustomer(name, numTickets, arrivalTime); //create n customers with new data in each
        
        int numQ = QueueNumber(customer->name);
        
        if(numQ != 0) {
            //assigning customer to that specific queue
            customer->assignedQueue = numQ - 1;
            enqueue(q[numQ - 1], customer);
            q[numQ - 1]->sizequeue++;
            //enqeue customer into that qeue
            //increase the size of that specific queue
        }
        else {
            //iterate through all queues looking for smallest size to place customer into
            int minSize = MAX_CUSTOMERS;
            for(int j = 0; j < 12; j++) {
                if((q[j]->sizequeue > 0) && (q[j]->sizequeue < minSize)) {
                    minSize = q[j]->sizequeue;
                    numQ = j + 1;
                    //printf("%i REASSIGNED TO THIS Q\n", j);
                }
            }
            
            customer->assignedQueue = numQ - 1;//assigning customer to that specific queue
            enqueue(q[numQ - 1], customer);
            q[numQ - 1]->sizequeue++;
        }
        masterList[i]->customer = customer; //putting all customers into a queue
    }

    int k = 0; //booths that have received at least 1 customer at some point in time
    for (int i = 0; i < 12; i++) {
        if (!Empty(q[i])) {
            k++;
        }
    }

    int evenSplit = k/b; 
    int specialBooths = k%b; 
    int queueCap = evenSplit + 1;
    
    int start, tracker = 0; //manages the queues being added
  
    for (int i = 0; i < b; i++) { //keeping track of each booth
       
            for (int j = start; (j < 12); j++) { //iterates through all queues 
                    // if we are within the first 2 booths and the queue is not empty, add k/b + k%b/k%b queues
                
                if ((!Empty(q[j])) && (i < specialBooths)) { 
                        tracker++;
                        q[j]->assignedBooth = i; 
        
                        if (tracker == queueCap) {
                            start = j + 1; 
                            tracker = 0;
                            break;
                        }
                    } 

                // if we are after the two booths, add k/b queues if queues are also not empty
                if ((!Empty(q[j])) && (i >= specialBooths)) { 
                        tracker++;
                        q[j]->assignedBooth = i; 

                        if (tracker == evenSplit) {
                       
                            start = j + 1; 
                            tracker = 0;
                            break;
                        }
                }
            }
        }

    int customerQueue;
    int boothMeetingTime = masterList[0]->customer->arrivalTime; //setting to very first customer arrival time
    
    for (int i = 0; i < b; i++) {
        int purchased = 0;
        printf("BOOTH %i\n", i + 1);
        for (int j = 0; j < n; j++) {
            if (purchased == 0) boothMeetingTime = masterList[j]->customer->arrivalTime; //if nobody has purchased yet, on first customer
           
            customerQueue = masterList[j]->customer->assignedQueue;
            if (q[customerQueue]->assignedBooth == i) { //if the assigned queue assigned booth matches current booth
                //use that customer's arrival time if its the first customer in booth
                //otherwise use booth meeting time for formula
                boothMeetingTime += (30 + (masterList[j]->customer->numTickets * 5));
                printf("%s from line %i checks out at time %i.\n", masterList[j]->customer->name, customerQueue + 1, boothMeetingTime);
                
                purchased++; //marks those who have purchased within current booth
            }
        
        }
        printf("\n");
    }

    return 0;
}

