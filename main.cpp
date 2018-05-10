#include <iostream>
#include <fstream>
using namespace std;
ifstream fin("graf.in");
ofstream fout("graf.out");

template<class Type>class Graf;
template<class Type> class Node
{
    Type data;                          //informatia din nod
    Node *next;                         //adresa catre nodul urmator
public:
    Type Get_data() const {return data;}
    Node<Type>* Get_next() {return next;}
    friend class Graf<Type>;
};

template<class Type> class Graf
{
    int n;                            //numarul de noduri
    int m;                            //numarul de muchii
    Type *Label;                      //vectorul etichetelor nodurilor (1,2...) (a, c, r...)
    Node <Type> **List;               //liste de vecini
    void DFS_first(Type, int*, Type*, int&);   //DFS recursv, metoda privata
    void BFS_first(Type, int*, Type*, int&);   //metoda privata
public:
    Graf() {m=0; n=0;}                     //constructor fara parametri
    Graf(int n);                            //constructor cu parametri
    ~Graf();                               //destructor
    Graf (const Graf &other);             //constructor de copiere
    int Get_n() const {return n;}        //obtinerea numarului de noduri
    int Get_m() const {return m;}        //obtinerea numarului de muchii

    void Delete_List(Node<Type> *L);     //functie folosita indestructor
    Graf& operator= (const Graf &);      //supraincarcarea op =
    Type* operator[] (const Type) const;  //supraincarcarea op []
    int Connected_Components();          //determinarea componentelor conexe
    bool Tree();                         //verificare daca graful e arbore
    Type* DFS_second(Type);             //parcurgere DFS
    Type* BFS_second(Type);             //parcurgere BFS
    int Distance(Type, Type);           //distanta dintre 2 noduri

    void Add_edge (Type x, Type y)        //Adaugarea nodului y in lista de vecini a nodului x
    {
        Node <Type> *p=new Node <Type>;
        p->data=x;
        p->next=NULL;

        int pozy;
        for(int i=1; i<=n; i++)           //ne folosim de pozitia nodului x in vectorul de etichete
        if(Label[i]==y) {pozy=i;break;}   //pentru a tine o contorizare

        if(List[pozy]==NULL)              //daca lista e viza, primul element e p
            List[pozy]=p;
        else                             //altfel legam nodul p de restul listei
        {
            p->next=List[pozy];          //si il adaugam in fata
            List[pozy]=p;
        }
    }

    friend ifstream & operator >> (ifstream & in, Graf & G)   //supraincarcarea operatorului >>
    {
      Type x, y;
      in>>G.n>>G.m;                              //numarul de noduri si de muchii
      G.Label=new Type[G.n+1];                   //alocam memorie pt vectorul de etichete
      for(int i=1; i<=G.n; i++)                  //citim etichetele nodurilor
            in>>G.Label[i];
       G.List=new Node <Type> *[G.n+1];          //alocam memorie pt lista de vecini
       for(int i=1; i<=G.n; i++)
        {
            G.List[i]=NULL;                      //initial listele sunt vide
        }
        for(int i=1; i<=G.m; i++)                //citim cele m muchii
        {
            in>>x>>y;                            //cititm doua elemente de tip Type
            G.Add_edge(x, y);                    //il adaugam pe y in lista lui x
            G.Add_edge(y, x);                    //il adauxam pe x in lista lui y
        }
        return in;
    }

    friend ofstream & operator << (ofstream & out, const Graf & G)
{
        //supraincarcarea op << ce afiseaza toate datele unui graf
        out<<G.n<<" "<<G.m<<"\n";
        for(int i=1; i<=G.n; i++)
            out<<G.Label[i]<<" ";
        out<<"\n";
        for(int i=1; i<=G.n; i++)
        {
            out<<G.Label[i]<<": ";
            if(G.List[i]!=NULL)
            {
                Node <Type> *p=G.List[i];
                while(p!=NULL)
                {
                    out<<p->Get_data()<<" ";
                    p=p->Get_next();
                }
            }
            out<<"\n";
        }
        out<<"\n";
        return out;
    }

    friend bool operator < (const Graf & G1, const Graf & G2)         //supraincarcarea operatorului <
    {
       if(G1.n<G2.n) return true;
       if(G1.n==G2.n && G1.m<G2.m) return true;
       return false;
    }
    friend bool operator > (const Graf & G1, const Graf & G2)         //supraincarcarea operatorului >
    {
      if(G1.n>G2.n) return true;
      if(G1.n==G2.n && G1.m>G2.m) return true;
      return false;
    }

    friend ofstream & operator << (ofstream & out, int * v)
    {
        out<<"\n";
        for(int i=1; i<= sizeof(v)/sizeof(Type); i++)
            out<<v[i]<<" ";
        out<<"\n";
        return out;
    }

    friend ofstream & operator << (ofstream & out, int x)
    {
        out<<x;
        out<<"\n";
        return out;
    }

    friend ofstream & operator << (ofstream & out, char *s)
    {
        out<<s;
        return out;
    }
};


template <class Type>
Graf<Type>::Graf(int n)
{
    Label=new Type[n+1];          //aloc memorie pt vectorul de etichete
    List=new Node<Type>*[n+1];   //si pentru matricea de vecini
    for(int i=1; i<=n; i++)
        List[i]=new Node<Type>[n+1];
}

template<class Type>
void Graf<Type>:: Delete_List(Node<Type> *List)
{
    if(List!=NULL)                    //parcurgem lista L[i] (vector) de la final la inceput, stergand fiecare nod
        if(List->next!=NULL)
            Delete_List(List->next);
    if(List!=NULL)
        delete(List);
}

template <class Type>
Graf<Type>::~Graf()                   //destructor
{
    for(int i=1; i<=n; i++)          //dezalocam memoria folosita la listele de vecini
    {
        if(List[i]!=NULL)
             Delete_List(List[i]);
    }
    delete [] (List);

}


template<class Type>
Graf<Type>::Graf(const Graf &other)
{
    this->n=other.n;
    this->m=other.m;
    //aloc memorie pentru noul obiect
    this->Label=new Type[other.n+1];
    for(int i=1; i<=n; i++)
        this->Label[i]=other.Label[i];
    this->List=new Node<Type>*[other.n+1];
    for(int i=1; i<=n; i++)
        this->List[i]=NULL;

    for(int i=1; i<=n; i++)
    {
        //in fiecare List[i] am lista de pointeri a.i exista muchie intre i si p->data
        for(Node<Type>*p=other.List[i]; p!=NULL; p=p->next)
        {
            Node<Type> *u=new Node<Type>;     //creez un nod pt a copia informatia
            u->data=p->data;
            u->next=NULL;
            if(this->List[i]==NULL)           //caz lista vida
            {
                this->List[i]=u;
            }
            else                            //caz lista nevida
            {
                u->next=this->List[i];
                this->List[i]=u;
            }
        }
    }
}

template <class Type> Graf<Type>&
Graf <Type>:: operator = (const Graf &G)
{
    //la fel ca la constructorul de copiere, doar ca verificam ca cele 2 obiecte this si G sa fie diferite.
    if(this!=&G)
    {
        this->n=G.n;
        this->m=G.m;
        this->Label=new Type[G.n+1];
        for(int i=1; i<=n; i++)
            this->Label[i]=G.Label[i];
        this->List=new Node<Type>*[G.n+1];
        for(int i=1; i<=n; i++)
            this->List[i]=NULL;

        for(int i=1; i<=n; i++)
        {
            for(Node <Type> *p=G.List[i]; p!=NULL; p=p->Get_next())
            {
                Node<Type> *u=new Node <Type>;
                u->data=p->Get_data();
                u->next=NULL;
                if(this->List[i]==NULL)
                {
                    this->List[i]=u;
                }
                else
                {
                    u->next=this->List[i];
                    this->List[i]=u;
                }
            }
        }
    }
    return (*this);
}

template <class Type> Type*
Graf<Type>::operator[] (const Type x) const
{
    Type *v=new Type[n+1];   //indexare de la 1
    int pozx;
    for(int i=1; i<=n; i++)
        if(Label[i]==x)
        {
            pozx=i;    //vrem sa vedem a cata lista din **List este lista corespunzatoare nodului x
            break;
        }
    for(Node<Type> *p=List[pozx]; p!=NULL; p=p->Get_next())  //parcurgem lista
        v[++v[0]]=p->Get_data();      //extragem informatia
    return v;
}

template<class Type> void
Graf<Type>:: DFS_first(Type x, int *index, Type *path, int &step)
{
    int poz;
    for(int i=1; i<=n; i++)            //cautam pozitia lui x in vectorul de etichete
        if(Label[i]==x)
        {
            poz=i;    //ca sa stim care e lista lui de vecini si corespondentul in index
            break;
        }
    index[poz]=1;                     //marcam nodul ca vizitat
    path[++step]=x;                   //il adaugam in vectorul de parcurgere
    for(Node <Type> *p=List[poz]; p!=NULL; p=p->Get_next())        //parcurgem lista lui de vecini
    {
        int poz2;
        for(int i=1; i<=n; i++)
            if(Label[i]==p->Get_data())       //daca am gasit un nod vecin ii determinam pozitia din aceleasi motive
            {
                poz2=i;
                break;
            }
        if(index[poz2]==0)           //daca e nevizitat
        {
            DFS_first(p->Get_data(), index, path, step);  //aplicam functia de dfs pt acest nod, fiind recursiva
        }
    }
}


template <class Type> int
Graf<Type> :: Connected_Components(){
    int *index=new int[n+1];                         //index[i]=1-> nodul i a fost vizitat si index[0]=1 in caz contrar
    for(int i=0; i<=n; i++) index[i]=0;
    Type *path=new Type[n+1];
    int step=0;
    int numberComp=0;
    for(int i=1; i<=n; i++)                         //verificam pentru fiecare vecin
        if(index[i]==0)                             //daca am gasit un nod nevizitat
    {
        numberComp++;                               //inseamna ca am gasit o noua componenta conexa
        DFS_first(Label[i], index, path, step);     //pe care o exploram printr-un DFS
    }
    return numberComp;
}

template<class Type> bool
Graf<Type>::Tree()
{
    //Un arbore e un graf conex(are o singura componenta conexa) cu n-1 muchii
    if(Connected_Components()!=1) return false;
    if(m==n-1) return true;
    return false;
}

template <class Type> Type*
Graf<Type>::DFS_second(Type x)
{
                                         //functia returneaza vectorul de parcurgere dfs
    Type *path=new Type[n+1];            //vector de parcurgere
    int *index=new int[n+1];             //vector care retine daca un nod a fost vizitat sau nevizitat
    int step=0;                          //pozitia pe care am pus ultimul nod in path
    DFS_first(x, index, path, step);     //aplicam DFS din nodul de start
    for(int i=1;i<=n;i++)                //continuam parcurgerea in cazul in care avem mai multe componente conexe
        if(index[i]==0)
        DFS_first(Label[i], index, path, step);
    return path;
}

template <class Type> void
Graf<Type>::BFS_first(Type x, int *index, Type *path, int &step)
{
                                                   //folosim functia atat pt parcurgere cat si pt determinarea distantei dintre 2 noduri
    Type *Queue=new Type[n+1];                     //este imposibil sa existe mai mult de n noduri in coada la un moment dat
    for(int i=1;i<=n;i++) index[i]=0;              //initializam cu 0 vectorul care retine vizitarea nodurilor
    int poz, first=1, last=1;
    for(int i=1;i<=n;i++)
        if(Label[i]==x) {poz=i;break;}             //vrem sa vedem al catalea nod este x in vectorul de etichete
    index[poz]=1;                                  //ii marcam pozitia ca vizitat
                                                   //pt distanta, index[i] va retine distanta de la nodul de start la i, sau 0 daca nu a fost vizitat
    Queue[1]=x;                                    //introducem nodul de start in coada
    path[++step]=x;                                //introducem nodul x in vectorul de parcurgere pe pozitia step

    while(first<=last)                             //cat timp coada nu e vida
    {
        Type qfront=Queue[first];                  //extragem elementul din coada
        first++;                                   //il eliminam
        for(int i=1;i<=n;i++)                      //ii vedem pozitia in label ca sa stim cat este indexul corespunzator
        if(Label[i]==qfront) {poz=i; break;}
        for(Node <Type> *p=List[poz]; p!=NULL; p=p->next)
        {
            int poz2;
            for(int j=1; j<=n;j++)
                if(Label[j]==p->data) {poz2=j; break;}       //vedem pozitia lui p->data
            if(index[poz2]==0)                               //daca nu am vizitat nodul p->data
            {
            index[poz2]=index[poz]+1;                        //incrementam distanta cu 1
            Queue[++last]=p->data;                           //adaugam nodul in coada
            path[++step]=p->data;                            //adaugam nodul in vectorul de parcurgere
            }
        }
    }
}

template<class Type> Type*
Graf<Type>:: BFS_second(Type x)
{
                                       //functia returneaza vectorul de parcurgere bfs
    Type *path=new Type[n+1];         //vector de parcurgere
    int *index=new int[n+1];          //vector care retine daca un nod a fost vizitat
    int step=0;                       //pasul sau pozitia pe care am pus ultimul nod
    BFS_first(x, index, path, step);   //apelam functia de timp void BFS cu nodul de start x
    for(int i=1; i<=n; i++)            //continuam parcurgerea daca sunt mai multe componente conexe
        if(index[i]==0)
            BFS_first(Label[i], index, path, step);
    return path;                       //returnam vectorul de parcurgere
}

template<class Type> int
Graf<Type>::Distance(Type x, Type y)
{
    int *index=new int[n+1];                //vector de nod vizitat/nevizitat
    Type *path=new Type[n+1];               //vector de parcurgere(nu e necesar aici dar folosim BFS_first si pt parcurgerea bfs)
    int step=0,pozy;
    BFS_first(x, index, path, step);
    for(int i=1; i<=n; i++)                 //determinam pozitia lui y in label ca sa stim indexul lui
        if(Label[i]==y)
        {
            pozy=i;
            break;
        }
    return index[pozy]-1;                  //am inceput cu index[x]=1 deci fiecare distanta din index e mai mare cu 1 decat ar fi trebuit
}
int main()
{
    Graf<char> G;
    fin>>G;
    fout<<G;

    Graf<char> G1(G);
    fout<<G1;

    Graf<char> G2=G;
    fout<<G2;


    fout<<"\n";
    fin>>G2;
    fout<<G2;
    if(G1>G2) fout<<"\nG1 este mai mare ca G2";
    else if(G1<G2) fout<<"\nG2 este mai mare ca G1";
    else fout<<"\nGrafurile sunt egale\n";

    fout<<"\n";
    fout<<"Numarul de componente conexe este: ";
    fout<<G.Connected_Components();

    fout<<"\nGraful este arbore?: ";
    if(G.Tree()==true) fout<<"DA\n";
    else fout<<"NU\n";

    fout<<"Parcurgerea DFS: ";
    cout<<"Introduce nodul de start pt parcurgerea DFS: ";
    char c;
    cin>>c;
    char* v=G.DFS_second(c);
    for(int i=1;i<=G.Get_n();i++)
       fout<<v[i]<<" ";


    fout<<"\nParcurgerea BFS: ";
    cout<<"Introduce nodul de start pt parcurgerea BFS: ";
    char s;
    cin>>s;
    char *t=G.BFS_second(s);
    for(int i=1;i<=G.Get_n();i++)
       fout<<t[i]<<" ";

    cout<<"Introduceti cele 2 noduri: ";
    char x, y;
    cin>>x>>y;
    fout<<"\nDistanta dintre "<<x<<" si " <<y<<" este: ";
    fout<<G.Distance(x, y);

    return 0;
}
