//************************ START OF CODE *********************************
#include<stdio.h>
#include<stdlib.h>
typedef struct TheNode {
    int key; 			//Also known as Rank
    int mark;          //To Mark the TheNode if it has lost a child
    int degree;
    struct TheNode *next, *prev, *parent, *child;
} TheNode;
typedef struct theHeap {
    struct TheNode *roots;  // just a node in the root list other than minptr
    struct TheNode *minptr; // Points to the minimum node in root-list
    int n; /* number of nodes in tree*/
} theHeap;
TheNode *t, *x;
theHeap *H,*H1,*H2;
TheNode * emptyNode(TheNode * t) {				//Make an Empty Node
    t = (TheNode*)malloc(sizeof(TheNode));
    t->key=0;
    t->degree=0;
    t->mark=0;
    t->prev=t->next=t;
    t->child=t->parent=NULL;
    return t;
}

theHeap* emptyHeap() { //Make an Empty Heap
    H=malloc(sizeof(theHeap));
    H->roots =NULL;
    H->minptr = NULL;
    H->n = 0;
    return H;
}

void swap(TheNode** x, TheNode** y) {
    struct TheNode* p = *x;
    *x = *y;
    *y = p;
}

//************************ INSERT *********************************
theHeap* insert(theHeap* H, int val) {
    TheNode *temp =(TheNode *)malloc(sizeof(TheNode));
    temp->key=val;
    temp->parent=temp->child=NULL;
    temp->prev =temp->next=temp;
    temp->degree=0;
    temp->mark=0;
    if(!H)
        H = emptyHeap();
    if(!H->roots)
        H->roots = temp;
    TheNode* last = H->roots->prev;
    last->next = temp;
    temp->prev = last;
    temp->next = H->roots;
    H->roots->prev = temp;
    if(H->minptr == NULL || temp->key < H->minptr->key)
        H->minptr = temp;
    H->n++;
    printf("********* VALUE INSERTED SUCCESSFULLY ********\n");
    return H;
}

//************************ FIND MINIMUM *********************************
void findmin() {
    printf("\n\n******************************************\n\n");
    printf("\n\t MINIMUM KEY IN THE HEAP IS  : %d ",H->minptr->key);
    printf("\n\n******************************************\n\n");
}
//concatenate nodes in root list: For ExtractMin function
TheNode* addNode(TheNode* old, TheNode* new) {
    if(!old)
        return new;
    TheNode*oldPrev = old->prev;
    oldPrev->next = new;
    new->prev = oldPrev;
    new->next = old;
    old->prev = new;
    return old;
}
// remove node :For ExtractMin function
TheNode* removeNode(TheNode* old, TheNode* toremove) {
    TheNode *l, *r;
    if(toremove->next == toremove && old == toremove) {
        old = NULL; //* now empty root list
    } else {
        l = toremove->prev;
        r = toremove->next;
        l->next = r;
        r->prev = l;
        toremove->next = toremove;
        toremove->prev = toremove;
        if(toremove == old)
            old = r;
    }
    return old;
}

//using Fibonacci sequence to calculate maximum degree
int maxdeg(int n) {
    int d, S;
    int S2 = 0;
    int S1 = 1;
    for(S=S1+S2, d=2; S<n; ++d) {
        S2 = S1;
        S1 = S;
        S = S1 + S2;
    }
    return d-2;
}

//************************ LINK *********************************
TheNode* link(TheNode * x,TheNode * y) {
    if(y->key < x->key)
        swap(&x, &y);
    x->child = addNode(x->child, y);
    y->parent = x;
    x->degree++;
    y->mark = 0;
    return x;

}

//************************ CONSOLIDATE *********************************
void consolidate(theHeap* H) {
    if(!H->roots) {
        printf("\n\n ************** CANNOT EXTRACT FROM EMPTY HEAP **************\n");
        return;
    }
    printf("\n************** CONSOLIDATE **************\n");
    int D = maxdeg(H->n)+1; ;//maximum degree
    //printf("\nmaxdeg is %d",D);
    TheNode *x, *y;
    TheNode* A[D+1]; //sizeof(TheNode*)*(D+1))
    int i,d;
    for(i=0; i<=D; ++i)
        A[i]=NULL;
    while(H->roots) { 	//for each node w in the root list of H
        x = H->roots;
        d= x->degree;
        H->roots = removeNode(H->roots, x);
        while(A[d]) {
            y = A[d];  // A hit when two tree or node of same degree
            x = link(x, y);
            A[d++]=NULL;
        };
        A[d]=x;
    };
    H->minptr = H->roots = NULL ;
    for(i=0; i<=D; ++i)
        if(A[i]) {
            printf("\nDEGREE OF ROOT-NODE (%d) IS  : %d",A[i]->key,i);
            H->roots = addNode(H->roots, A[i]);
            if(H->minptr == NULL || A[i]->key < H->minptr->key)
                H->minptr = A[i];
        }
    free(A);
}

//************************ EXTRACT-MIN *********************************

void extractMin(theHeap *H) {
    TheNode * z;
    TheNode * zChild;
    TheNode * temp;
    z=H->minptr;
    if(z) {
        zChild=z->child;
        if(zChild) {
            do {
                temp=zChild;
                zChild=zChild->next;
                addNode(H->roots, temp);
                temp->parent=NULL;
            } while(zChild!=z->child);
        }
        H->roots = removeNode(H->roots, z);
        H->n--;
        if(H->n==0) {
            printf("\n\n************** THIS WAS THE LAST NODE. HEAP IS NOW EMPTY **************\n");
        } else {
            consolidate(H);
            free(z);
        }
    }
}

//************************ CUT *********************************
void cut(theHeap* H,TheNode* x) {
    TheNode* y;
    y=x->parent;
    y->child=removeNode(y->child, x);
    y->degree--;
    H->roots = addNode(H->roots, x);
    x->parent = NULL;
    x->mark = 0;
}

//************************ CASCADE-CUT *********************************
void cascadingcut(theHeap* H, TheNode* z) {
    TheNode* p = (TheNode*)malloc(sizeof(TheNode));
    p = z->parent;
    if(p) {
        if(!z->mark)
            z->mark = 1;
        else {
            cut(H, z);
            cascadingcut(H, p);
        }
    }
}

//************************ FIND NODE *********************************
TheNode* searchnode(TheNode* x, int key) {

    TheNode* y = (TheNode*)malloc(sizeof(TheNode));
    TheNode* w = (TheNode*)malloc(sizeof(TheNode));
    w=x;
    y = NULL;
    if(x) {
        do
        {
            if(w->key == key) {
                y=w;
                break;
            } else
                y = searchnode(w->child,key);
            if(y)
                break;
            w = w->next;
        } while(w != x);
    }
    return y;

}

//************************ DECREASE-KEY *********************************

void decKey(theHeap *H,TheNode* x, int val) {

    printf("\nCurrent key is %d",x->key);
    printf("\nkey is changing to %d",val);
    if(val > x->key) {
        printf("\nEntered key is more than existing");
        return;
    }
    x->key=val;
    TheNode* y=x->parent;
    printf("\n\n    SELECT 8.DISPLAY TO PRINT HEAP    \n\n");
    if(y)
        if(y!=NULL && (x->key < y->key)) {
            cut(H,x);
            cascadingcut(H,y);
        }
    if(x->key < H->minptr->key)
        H->minptr = x;
}

//************************ DELETE-KEY *********************************
void deleteit(theHeap* H, TheNode* x) {
    double INFINITY = -1e99;
    decKey(H,x,INFINITY);
    extractMin(H);
}


// concatenate : For Merging operation
TheNode* concatenate(TheNode* old1, TheNode* old2) {
    struct TheNode* new1 = old1->prev;
    struct TheNode* new2 = old2->prev;
    new1->next = old2;
    old2->prev = new1;
    new2->next = old1;
    old1->prev = new2;
    return old1;
}

//************************ UNION/MERGE *********************************
theHeap* merge(theHeap* H1, theHeap* H2) {
    if((!H1) || (!H1->roots))
        return H2;
    if((!H2) || (!H2->roots))
        return H1;
    H = emptyHeap();
    H->roots = concatenate(H1->roots, H2->roots);
    if(H1->minptr->key < H2->minptr->key)
        H->minptr = H1->minptr;
    else
        H->minptr = H2->minptr;
    H->n = H1->n + H2->n;
    free(H1);
    free(H2);
    return H;
}

//************************ DISPLAY/PRINT *********************************
void display( TheNode *t) {
    TheNode *q;
    q=t;
    do
    {
        printf(" (");
        printf("%d",q->key);
        if(q->child!=NULL)
        {
            display(q->child);
        }
        printf(") ");
        q=q->next;
    }
    while(q!=t);
}

//************************ MAIN-FUNCTION *********************************
int main() {
    x = emptyNode(x);
    int option,val,o,k;
    //clrscr();
    H1=H2=NULL;
    do {
        printf("\n******************************************");
        printf("\n\tFIBONACCI HEAP: MAIN MENU\t");
        printf("\n******************************************");
        printf("\n\n   TWO HEAPS ARE AVAILABLE FOR DEMO   ");
        printf("\n\n ENTER 1 FOR HEAP 1 \n ENTER 2 FOR HEAP 2 \n ENTER 3 TO UNION AND STORE IN HEAP 2 \n ENTER 4 TO EXIT \n :->  ");
        scanf("%d",&o);

        switch(o)
        {
        case 1:
            if(!H1)
                H1=emptyHeap();
            H = H1;
            printf("\n\nxxxxxxxxxx CURRENTLY IN HEAP 1 nxxxxxxxxxx");
            break;
        case 2:
            if(!H2)
                H2=emptyHeap();
            H = H2;
            printf("\n\nxxxxxxxxxx CURRENTLY IN HEAP 2 xxxxxxxxxx");
            break;
        case 3:
            H = merge(H1, H2);
            break;
        case 4:
            exit(1);
        }
        if(o==3) {
            printf("\n\n********** UNION SUCCESSFULLY COMPLETED**********");
            printf("\n\n********** RESULT STORED IN HEAP 2 **************");
            printf("\n\n\txxxxx ENTER 8 TO DISPLAY MERGED HEAP xxxxxx \n\n");
        }
        do
        {

            printf("\n******************************************");
            printf("\n\tAVAILABLE OPERATIONS			");
            printf("\n******************************************");
            printf("\n1:INITIALIZE A NEW HEAP OR EMPTY THE EXISTING HEAP");
            printf("\n2:INSERT MULTIPLE NODE IN THE HEAP");
            printf("\n3:DISPLAY MINIMUM NODE IN THE HEAP");
            printf("\n4:EXTRACT MINIMUM NODE IN THE HEAP");
            printf("\n5:SEARCH SPECIFIC NODE IN THE HEAP");
            printf("\n6:DECREASE-KEY OF NODE IN THE HEAP");
            printf("\n7:DELETE SPECIFIC NODE IN THE HEAP");
            printf("\n8:DISPLAY ALL THE NODE IN THE HEAP");
            printf("\n9:RETURN: MAIN MENU");
            printf("\n\n\nENTER YOUR CHOICE  :  ");
            scanf("%d",&option);
            switch(option)
            {

            case 1:
                emptyHeap();
                printf("\n\n*************EMPTY HEAP CREATED SUCCESSFULLY **********\n\n");
                break;
            case 2:
                printf("\nENTER THE NUMBER OF NODES TO BE CREATED IN THE HEAP :  ");
                scanf("%d",&k);
                for(int i=1; i<=k; i++) {
                    printf("\n ENTER THE KEY %d/%d :   ",i,k);
                    scanf("%d",&val);
                    H = insert(H,val);
                }
                break;
            case 3:
                if(H->roots!=NULL)
                    findmin();
                else {
                    printf("\n\n ********* HEAP IS EMPTY ********\n");
                    break;
                }
                break;
            case 4:
                if (H->minptr) {
                    o=H->minptr->key;
                    extractMin(H);
                    if(H->roots) {
                        t=H->roots;
                        printf("\n\n");
                        display(t);
                        printf("\n\n************ EXTRACTED NODE IS  : %d ************",o);
                    }
                } else {
                    printf("\n\n************* CANNOT EXTRACT FROM EMPTY HEAP *************\n");
                    break;
                }
                printf("\n\n\t************ EXTRACT-MIN FINISHED *************\n");
                break;
            case 5:
                printf("\n SELECT THE NODE : ");
                scanf("%d",&val);
                x=searchnode(H->minptr,val);
                if(!x)
                    printf("\n\tTHE NODE IS NOT IN HEAP\n\n");
                else
                    printf("\n\t THE NODE IS IN THE HEAP\n\n");
                break;
            case 6:
                printf("\n AVAILABLE NODES IN THE HEAP ARE : ");
                t=H->roots;
                display(t);
                printf("\n SELECT THE NODE : ");
                scanf("%d",&val);
                x=searchnode(H->minptr,val);
                if(!x)
                    printf("\n\tTHE NODE IS NOT IN HEAP\n\n");
                else {
                    printf("\n\n\tTHE SELECTED NODE IS : %d\n",x->key);
                    printf("\n ENTER THE KEY TO BE SET  :  ");
                    scanf("%d",&val);
                    decKey(H,x,val);
                }
                break;
            case 7:
                printf("\n SELECT THE NODE : ");
                scanf("%d",&val);
                x=searchnode(H->minptr,val);
                if(!x)
                    printf("\n\tTHE NODE IS NOT IN HEAP\n\n");
                else {
                    printf("\n\n\tTHE SELECTED NODE IS : %d\n",x->key);
                    deleteit(H,x);
                    printf("\n\n************NODE SUCCESSFULLY DELETED ***************\n\n");
                }
                break;
            case 8:
                printf("\n******************************************");
                printf("\n          FIBONACCI HEAP DISPLAY           \n\n");
                t=H->roots;
                if(t==NULL)
                {
                    printf("\n********* FIBONACCI HEAP IS EMPTY ********\n");
                    break;
                }
                else display(t);
                printf("\n\n******************************************\n\n");
                break;
            case 9:
                break;
            }

        } while(option!=9);

        if(o!=1)
            H2=H;
        else H1=H;
    } while(o!=4);

    return 0;
}
//************************ END OF CODE *********************************