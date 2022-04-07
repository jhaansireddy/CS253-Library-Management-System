#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string>

using namespace std;

//classes Definition
class User{
    string Name, Password;
    protected:
    int role; //1: Librarian; 2: Professor; 3: Student
    public:
    string Id;
    void setUser(string n, string i){
        Name = n;
        Id = i;
    }
    int KnowRole(){
        return role;
    }
};
class Book{
    string Title,Author,Publication,ISBN, Id;
    int avail;
    vector<int> due_day;
    void set_duedate();
    public:
    void setBook(string t, string a,string p, string isbn, string i, int av, int d[3]){
        Title= t;
        Author = a;
        Publication =p;
        ISBN = isbn;
        Id =i;
        avail=av;
        for(int j=0; j<3; j++) due_day.push_back(d[j]);
    }
    int Book_request(string id_of_user); 
    int Book_return(string id_of_user);
    int* Show_duedate(){
        cout << "Due date to return the book \""<<Title<<"\" is "<<due_day[0]<<"-"<<due_day[1]<<"-"<<due_day[2];
        return due_day;
    }
};
class UserDatabase{
    void Add();
    void Update();
    void Delete();
    void Search();
   // void Display();
    friend class Librarian;
};

class BooksDatabase{
    void Add();
    void Update();
    void Delete();
    
    friend class Librarian;
    friend class Book;
    public:
    void Search(int indicator); //indicator=1: search by issued person; indicator=2: search by isbn 
    void Display();
};

class Professor :  public User{
    float FineAmnt;
    int book_count;
  //  vector<Book> book_list;
    void Calculate_fine();
    public:
    void setProf(string n, string i, float f, int bc, vector<Book> list){
        setUser(n,i);
        role = 2;
        FineAmnt=f;
        book_count=bc;
       // for(int j=0; j<bc; j++) { book_list[j] = list[j]; }
    }
    void Clear_fine_amount(){
        FineAmnt = 0;
    }
};

class Librarian : private User{
    public:
    void Act_on_user_database(UserDatabase U, int choice){ //1: add, 2: update, 3: delete, 4: view details of any user
        switch(choice){
            case 1: U.Add(); break;
            case 2: U.Update(); break;
            case 3: U.Delete(); break;
            case 4: U.Search(); break;
            default: cout<<"\nReconsider Choice\n";
        }
    }
    void Act_on_book_database(BooksDatabase B, int choice){ //5:view booksissued by a user, 6: add, 7: update, 8: delete, 9: view details of particular book by isbn, 10: view list of books
        switch (choice)
        {
            case 5: B.Search(1); break;
            case 6: B.Add(); break;
            case 7: B.Update(); break;
            case 8: B.Delete(); break;
            case 9: B.Search(2); break;
            case 10: B.Display(); break;
            default: cout<<"\nReconsider Choice\n";
        }
    }
};

class Student :  private User{
    float FineAmnt;
    int book_count;
   // vector<Book> book_list;
    void Calculate_fine();
    public:
    void setStud(string n, string i, float f, int bc, vector<Book> list){
        setUser(n,i);
        role =3;
        FineAmnt=f;
        book_count=bc;
     //   for(int j=0; j<bc; j++) { book_list[j] = list[j]; }
    }
    void Clear_fine_amount(){
        FineAmnt = 0;
    }
};

//class functions definition
int Book :: Book_request(string id_of_user){
    int role, books;
    UserDatabase U;
    BooksDatabase B;
        if(avail == 0){
        cout << "Book not available.";
        }
        else if(avail == 1){
            
            ifstream fin;
            ofstream fout;
            string line, word;
            vector<string> row;
            int flag =0;
    
            fin.open("UserDatabase.csv");
            fout.open("UserTemp.csv");
            
            getline(fin,line); //titles of attributes
            while(!fin.eof()){
                int issued = 0;
                row.clear();
        
                getline(fin,line);
                stringstream s(line);
                while(getline(s,word,", ")){
                     row.push_back(word);
                }
                if(!id_of_user.compare(row[0])){
                    flag =1;
                    role = stoi(row[3]);
                    if(role==2) {
                        Professor P;
                        P.setProf(row[1],row[0],stof(row[4]),stoi(row[5]));
                        avail = 0; issued = 1;
                    }
                    else if(role==3){
                        Student S;
                        S.setStud(row[1],row[0],stof(row[4]),stoi(row[5]));
                        if(S.book_count < 5){
                            avail = 0; issued = 1;
                        }
                    }
                    if(issued){
                        Id = id_of_user;
                        set_duedate(role);
                        int rsize = row.size();
                        for(int j=0;j<rsize-1;j++){
                             fout<< row[j]<<", ";
                        }
                        fout<<to_string(stoi(row[rsize-1]) + 1)<<"\n";
                    }
                }
                else if(issued==0){
                    int rsize = row.size();
                    for(int j=0;j<rsize-1;j++) fout<< row[j]<<", ";
                    fout<<row[rsize-1]<<"\n";
                }
        }
         if(flag==0){
                 cout<< "\nUser NOT found\n";
            }
        fin.close();
        fout.close();
        remove("UserDatabase.csv");
        rename("UserTemp.csv","UserDatabase.csv");
        }
    return avail;
}
int Book ::Book_return(string id_of_user){
            int role;
            ifstream fin;
            ofstream fout;
            string line, word;
            vector<string> row;
            int flag =0;
    
            fin.open("UserDatabase.csv");
            fout.open("UserTemp.csv");
            
            getline(fin,line); //titles of attributes
            while(!fin.eof()){
                //int issued = 0;
                row.clear();
        
                getline(fin,line);
                stringstream s(line);
                while(getline(s,word,", ")){
                     row.push_back(word);
                }
                if(!id_of_user.compare(row[0])){
                        if(Id.compare(id_of_user)){
                            cout << "\nYou CANNOT return this book\n";
                            return avail;
                        }
                        avail = 1;
                        int rsize = row.size();
                        for(int j=0;j<rsize-1;j++){
                             fout<< row[j]<<", ";
                        }
                        fout<<to_string(stoi(row[rsize-1]) - 1)<<"\n";
                }
                else {
                    int rsize = row.size();
                    for(int j=0;j<rsize-1;j++) fout<< row[j]<<", ";
                    fout<<row[rsize-1]<<"\n";
                }
        }
         if(flag==0){
                 cout<< "\nUser NOT found\n";
            }
        fin.close();
        fout.close();
        remove("UserDatabase.csv");
        rename("UserTemp.csv","UserDatabase.csv");

        fin.open("BooksDatabase.csv");
        fout.open("BooksTemp.csv");
            
            getline(fin,line); //titles of attributes
            while(!fin.eof()){
                //int issued = 0;
                row.clear();
        
                getline(fin,line);
                stringstream s(line);
                while(getline(s,word,", ")){
                     row.push_back(word);
                }
                if(!ISBN.compare(row[0])){
                       // int rsize = row.size();
                        for(int j=0;j< 4;j++){
                             fout<< row[j]<<", ";
                        }
                        fout<<avail<<", NULL, NULL\n";
                }
                else {
                    int rsize = row.size();
                    for(int j=0;j<rsize-1;j++) fout<< row[j]<<", ";
                    fout<<row[rsize-1]<<"\n";
                }
        }
         if(flag==0){
                 cout<< "\nUser NOT found\n";
            }
        fin.close();
        fout.close();
        remove("BooksDatabase.csv");
        rename("BooksTemp.csv","BooksDatabase.csv");
        return avail;
}

void Book :: set_duedate(){
    time_t now = time(0);
    tm *d = 
}

void UserDatabase :: Add(){
    ofstream fout;
    string name, id, password=" ", cpassword=".";
    int role;
    char c= 'y';
    fout.open("UserDatabase.csv", ios::app);
    do{
    cout <<"\nENTER DETAILS OF USER TO BE ADDED: ";
    cout << "\nEnter User Name : ";
    getline(cin,name);
    cout << "\nEnter user id : ";
    cin >> id;
    while(password.compare(cpassword)){
        cout << "\nSet password for user : (no spaces)";
        cin >> password;
        cout << "\nRenenter password : ";
        cin >> cpassword;
    }
    cout << "\nPASSWORD SET\n";
    cout << "What is the role of the user? Type \n 1: Librarian\n 2: Professor\n 3: Student\n";
    cin >> role;

    fout << id<<", "<<name<<", "<<password<<", "<<role<<", "<<0<<", "<<0<<"\n";

    cout << "\nContinue with adding users? (y/n): ";
    cin >> c;
    }while(c == 'y');
    fout.close();
}
void BooksDatabase :: Add(){
    ofstream fout;
    string title, author,isbn, publication;
    int availability;
    char c= 'y';
    fout.open("BooksDatabase.csv", ios::app);
    do{
    cout <<"\nENTER DETAILS OF BOOK TO BE ADDED: ";
    cout << "\nEnter Title of the book : ";
    getline(cin,title);
    cout << "\nEnter the author name : ";
    getline(cin,author);
    cout << "\nEnter the publication : ";
    getline(cin,publication);
    cout <<"\nEnter the ISBN number : ";
    cin >> isbn;

    fout << isbn<<", "<<title<<", "<<author<<", "<<publication<<", "<<0<<", NULL, NULL\n";

    cout << "\nContinue with adding books? (y/n): ";
    cin >> c;
    }while(c == 'y');
    fout.close();
}

void UserDatabase :: Search(){
    ifstream fin;
    string line, word, id;
    vector<string> row;
    int flag =0;
    
    fin.open("UserDatabase.csv");
    cout<<"\nEnter the Id of the user : ";
    cin>> id;
    getline(fin,line); //titles of attributes
    while(!fin.eof()){
        row.clear();
        
        getline(fin,line);
        stringstream s(line);
        while(getline(s,word,", ")){
            row.push_back(word);
        }
        if(!id.compare(row[0])){
            flag =1;
            cout<< "\nFOUND. Here are the DETAILS:  ";
            cout << "\nID : "<<row[0];
            cout << "\nName : "<<row[1];
            cout << "\nRole : ";
            switch(stoi(row[3])){
                case 1: cout <<"Librarian"; break;
                case 2: cout<< "Professor"; break;
                case 3: cout<< "Student"; break;
                default: cout<< "Found to be NULL";
            }
            cout<< "\nFine Amount Due : "<<stof(row[4]);
            cout<< "\nNumber of books issued : "<<stoi(row[5]);
            break;
        }
    }
    if(flag==0){
        cout<< "\nUser NOT found\n";
    }
    fin.close();
}
void BooksDatabase :: Search(int indicator){
    ifstream fin;
    string line, word, id, isbn;
    vector<string> row;
    int flag =0;
    
    fin.open("BooksDatabase.csv");
    if(indicator==1){
        cout<<"\nEnter the Id of the user : ";
        cin>> id;
    }
    if(indicator==2){
        cout<<"\nEnter the ISBN of the book : ";
        cin>> isbn;
    }
    getline(fin,line); //titles of attributes
    while(!fin.eof()){
        row.clear();
        
        getline(fin,line);
        stringstream s(line);
        while(getline(s,word,", ")){
            row.push_back(word);
        }
        if(indicator==1 && id.compare(row[5])==0){
            flag =1;
            cout<< "\nFOUND. Here are the DETAILS:  ";
            cout << "\nISBN : "<<row[0];
            cout << "\nTitle : "<<row[1];
            cout << "\nAuthor : "<<row[2];
            cout << "\nPublication : "<<row[3];
            cout<< "\nAvailability : "<<(stoi(row[4])==0)?"yes":"no";
            cout<< "\nIssued by id : "<<row[5];
            cout<< "\nDue date : "<<row[6];
        }
        if(indicator==2 && isbn.compare(row[0])==0){
            flag =1;
            cout<< "\nFOUND. Here are the DETAILS:  ";
            cout << "\nISBN : "<<row[0];
            cout << "\nTitle : "<<row[1];
            cout << "\nAuthor : "<<row[2];
            cout << "\nPublication : "<<row[3];
            cout<< "\nAvailability : "<<(stoi(row[4])==0)?"yes":"no";
            cout<< "\nIssued by id : "<<row[5];
            cout<< "\nDue date : "<<row[6];
            break;
        }
    }
    if(flag==0) cout<<"\nBook NOT found by the given details\n";
    fin.close();
}

void UserDatabase :: Update(){
    int col;
    string id;
    cout << "\nUPDATE USER DATABASE\n";
    cout << "\nEnter ID of user whose details you want to update: ";
    cin >> id;
    cout << "\nWhich field do you want to update?\n 1:Role  2:FineAmnt  3:Books count\n";
    cin >> col;

            ifstream fin;
            ofstream fout;
            string line, word;
            vector<string> row;
            int flag =0;
    
            fin.open("UserDatabase.csv");
            fout.open("UserTemp.csv");
            
            getline(fin,line); //titles of attributes
            while(!fin.eof()){
                int issued = 0;
                row.clear();
        
                getline(fin,line);
                stringstream s(line);
                while(getline(s,word,", ")){
                     row.push_back(word);
                }
                if(!id_of_user.compare(row[0])){
                        cout<< "\nFOUND. Here are the DETAILS:  ";
                        cout << "\nID : "<<row[0];
                        cout << "\nName : "<<row[1];
                        cout << "\nRole : ";
                        switch(stoi(row[3])){
                             case 1: cout <<"Librarian"; break;
                             case 2: cout<< "Professor"; break;
                             case 3: cout<< "Student"; break;
                             default: cout<< "Found to be NULL";
                         }
                        cout<< "\nFine Amount Due : "<<stof(row[4]);
                        cout<< "\nNumber of books issued : "<<stoi(row[5]);
                        switch(col){
                            case 1: int nr;
                                    cout <<"\nEnter new role (1:Librarian 2:Professor 3:Student)\n";
                                    cin >> nr; 
                                    row[3] = to_string(nr);
                                    break;
                            case 2: float nf;
                                    cout <<"\nEnter new Fine amount: ";
                                    cin >> nf; 
                                    row[4] = to_string(nf);
                                    break;
                            case 3: int nb;
                                    cout <<"\nEnter new book count: ";
                                    cin >> nb; 
                                    row[5] = to_string(nb);
                                    break;
                            default: cout <<"\nEnter valid choice!!";
                        }
                    
                }
                    int rsize = row.size();
                    for(int j=0;j<rsize-1;j++) fout<< row[j]<<", ";
                    fout<<row[rsize-1]<<"\n";
                
        }
         if(flag==0){
                 cout<< "\nUser NOT found\n";
            }
        fin.close();
        fout.close();
        remove("UserDatabase.csv");
        rename("UserTemp.csv","UserDatabase.csv");
        }
}
void BooksDatabase :: Update(){
    int col;
    string isbn;
    cout << "\nUPDATE BOOKS DATABASE\n";
    cout << "\nEnter ISBN of book whose details you want to update: ";
    cin >> isbn;
    cout << "\nWhich field do you want to update?\n 1:Title  2:Author  3:Publication  4:Availability  5:Issued by user  6:Due date of return\n";
    cin >> col;
            ifstream fin;
            ofstream fout;
            string line, word;
            vector<string> row;
            int flag =0;
    
            fin.open("UserDatabase.csv");
            fout.open("UserTemp.csv");
            
            getline(fin,line); //titles of attributes
            while(!fin.eof()){
                int issued = 0;
                row.clear();
        
                getline(fin,line);
                stringstream s(line);
                while(getline(s,word,", ")){
                     row.push_back(word);
                }
                if(!id_of_user.compare(row[0])){
                    flag =1;
                    role = stoi(row[3]);
                    if(role==2) {
                        Professor P;
                        P.setProf(row[1],row[0],stof(row[4]),stoi(row[5]));
                        avail = 0; issued = 1;
                    }
                    else if(role==3){
                        Student S;
                        S.setStud(row[1],row[0],stof(row[4]),stoi(row[5]));
                        if(S.book_count < 5){
                            avail = 0; issued = 1;
                        }
                    }
                    if(issued){
                        Id = id_of_user;
                        set_duedate(role);
                        int rsize = row.size();
                        for(int j=0;j<rsize-1;j++){
                             fout<< row[j]<<", ";
                        }
                        fout<<to_string(stoi(row[rsize-1]) + 1)<<"\n";
                    }
                }
                else if(issued==0){
                    int rsize = row.size();
                    for(int j=0;j<rsize-1;j++) fout<< row[j]<<", ";
                    fout<<row[rsize-1]<<"\n";
                }
        }
         if(flag==0){
                 cout<< "\nUser NOT found\n";
            }
        fin.close();
        fout.close();
        remove("UserDatabase.csv");
        rename("UserTemp.csv","UserDatabase.csv");
        }
}

void UserDatabase :: Delete(){
    ifstream fin;
    ofstream fout;
    string line, word, id;
    vector<string> row;
    int flag =0;
    char c='n';
    fin.open("UserDatabase.csv");
    fout.open("UserTemp.csv");
    cout<<"\nEnter the Id of the user to be deleted: ";
    cin>> id;
    getline(fin,line); //titles of attributes
    while(!fin.eof()){
        row.clear();
        c ='n';
        getline(fin,line);
        stringstream s(line);
        while(getline(s,word,", ")){
            row.push_back(word);
        }
        if(!id.compare(row[0])){
            flag =1;
            cout<< "\nFOUND. Here are the DETAILS:  ";
            cout << "\nID : "<<row[0];
            cout << "\nName : "<<row[1];
            cout << "\nRole : ";
            switch(stoi(row[3])){
                case 1: cout <<"Librarian"; break;
                case 2: cout<< "Professor"; break;
                case 3: cout<< "Student"; break;
                default: cout<< "Found to be NULL";
            }
            cout<< "\nFine Amount Due : "<<stof(row[4]);
            cout<< "\nNumber of books issued : "<<stoi(row[5]);
            cout<<"\nCONFIRM DELETE? (y/n)";
            cin >> c;
            break;
        }
        else if(c=='n'){
            int rsize = row.size();
            for(int j=0;j<rsize-1;j++) fout<< row[j]<<", ";
            fout<<row[rsize-1]<<"\n";
        }
    }
    if(flag==0){
        cout<< "\nUser NOT found\n";
    }
    fin.close();
    fout.close();

    remove("UserDatabase.csv");
    rename("UserTemp.csv","UserDatabase.csv");
}
void BooksDatabase :: Delete(){
    ifstream fin;
    ofstream fout;
    string line, word, isbn;
    vector<string> row;
    int flag =0;
    char c='n';
    fin.open("BooksDatabase.csv");
    fout.open("BooksTemp.csv");
    cout<<"\nEnter the ISBN of the Book to be deleted: ";
    cin>> isbn;
    getline(fin,line); //titles of attributes
    while(!fin.eof()){
        row.clear();
        c ='n';
        getline(fin,line);
        stringstream s(line);
        while(getline(s,word,", ")){
            row.push_back(word);
        }
        if(!id.compare(row[0])){
            flag =1;
            cout<< "\nFOUND. Here are the DETAILS:  ";
            cout << "\nISBN : "<<row[0];
            cout << "\nTitle : "<<row[1];
            cout << "\nAuthor : "<<row[2];
            cout << "\nPublication : "<<row[3];
            cout<< "\nAvailability : "<<(stoi(row[4])==0)?"yes":"no";
            cout<< "\nIssued by id : "<<row[5];
            cout<< "\nDue date : "<<row[6];
            cout<<"\nCONFIRM DELETE? (y/n)";
            cin >> c;
            break;
        }
        else if(c=='n'){
            int rsize = row.size();
            for(int j=0;j<rsize-1;j++) fout<< row[j]<<", ";
            fout<<row[rsize-1]<<"\n";
        }
    }
    if(flag==0){
        cout<< "\nBook NOT found\n";
    }
    fin.close();
    fout.close();

    remove("BooksDatabase.csv");
    rename("BooksTemp.csv","BooksDatabase.csv");
}


void BooksDatabase :: Display(){
    ifstream fin;
    string line, word, id;
    vector<string> row;
    
    fin.open("BooksDatabase.csv");
    getline(fin,line); //titles of attributes
    cout << left <<setw(17)<<"ISBN"
        << left << setw(40) <<"TITLE"
        << left << setw(15) <<"AUTHOR"
        << left << setw(20) <<"PUBLICATION"
        << left << setw(3) <<"AVAILABILITY";
    while(!fin.eof()){
        row.clear();
        
        getline(fin,line);
        stringstream s(line);
        while(getline(s,word,", ")){
            row.push_back(word);
        }
        cout << left <<setw(17)<<row[0]
        << left << setw(40) <<row[1]
        << left << setw(15) <<row[2]
        << left << setw(20) <<row[3]
        << left << setw(3) <<(stoi(row[4])==0)?"yes":"no";
    }
    fin.close();
}


//general functions
Book GiveObject(string isbn){
    ifstream fin;
    string line, word;
    vector<string> row;
    int flag =0;
    Book B;
    fin.open("BooksDatabase.csv");
    
    getline(fin,line); //titles of attributes
    while(!fin.eof()){
        row.clear();
        
        getline(fin,line);
        stringstream s(line);
        while(getline(s,word,", ")){
            row.push_back(word);
        }
        
        if(isbn.compare(row[0])==0){
            flag =1;

            int d[3],j=0;
            string dig;
            stringstream s1(row[6]);
            while(getline(s1,dig,"-")){
                d[j++]= stoi(dig);
            }
            B.setBook(row[1],row[2],row[3],row[0],row[5],stoi(row[4]),d);
            return B;
        }
    }
    if(flag==0) cout<<"\nBook NOT found by the given details\n";
    fin.close();
}

int main(){
    UserDatabase U;
    BooksDatabase B;
    int attempt = 5;
    ifstream fin;
    
    vector<string> row;
    
    int userRole =0;
    while(attempt){
        fin.open("UserDatabase.csv");
        string input_id, input_passwd,line ,word ;
        cout<< "\nLOGIN\n";
        cout<< "\nEnter ID :";
        cin >> input_id;
        cout<< "\nEnter Password :";
        cin >> input_passwd;
        getline(fin, line); //titles of attributes
        while(!fin.eof()){
            row.clear();
            
            getline(fin, line);
            stringstream s(line);
    
            while (getline(s, word, ', ')) {
                row.push_back(word);
            }
            if(input_id.compare(row[0])==0){
                if(input_passwd.compare(row[2])== 0){
                    userRole = stoi(row[3]);
                    break;}
            }
        }
        if(userRole) {
            cout << "\nLOGIN SUCCESSFUL\n";
            break;
        }
        else{
            cout << "\nINVALID ID OR PASSWORD\n TRY AGAIN\n";
            attempt--;
        }
        fin.close();
    }
    
    if(userRole==1){
        Librarian L;
       // char c='y';
        int choice=0;
        do{
            cout << "\nMENU\nENTER YOUR CHOICE\n";
            cout << "1: Add User\n";
            cout << "2: Update User details\n";
            cout << "3: Delete User\n";
            cout << "4: View details of a User\n";
            cout << "5: View books issued to a particular User\n";
            cout << "6: Add Book\n";
            cout << "7: Update Book details\n";
            cout << "8: Delete Book from database\n";
            cout << "9: View details of Book (and who it was issued to)\n";
            cout << "10: View List of all Books\n";
            cout << "0: Exit\n";
            cin >> choice;
            if(choice && choice <= 4) L.Act_on_user_database(U,choice);
            else if(choice <= 10) L.Act_on_book_database(B,choice);
            else cout <<"\nEnter Valid Choice!\n";
        }while(choice!= 0);
    }
    else if(userRole==2){
        Professor P;
        P.setProf(row[1],row[0],stof(row[4]),stoi(row[5]));
        int choice=0;
        do{
            cout << "\nMENU\nENTER YOUR CHOICE\n";
            cout << "1: View list of ALL books\n";
            cout << "2: Request for a book\n";
            cout << "3: View details of a book\n";
            cout << "4: Return a book\n";
            cout << "5: Check Dues\n";
            cout << "0: Exit\n";
            cin >> choice;
            switch (choice)
            {
            case 1:
                    break;
            
            default: cout <<"\nEnter Valid choice\n";
                break;
            }
        }while(choice!= 0);
    }
    else if(userRole==3){
        Student S;
        S.setStud(row[1],row[0],stof(row[4]),stoi(row[5]));
        string isbn;
        int choice=0;
        do{
            cout << "\nMENU\nENTER YOUR CHOICE\n";
            cout << "1: View list of ALL books\n";
            cout << "2: Request for a book\n";
            cout << "3: View details of a book\n";
            cout << "4: Return a book\n";
            cout << "5: Check Dues\n";
            cout << "0: Exit\n";
            cin >> choice;
            switch (choice)
            {
            case 1: B.Display();
                    break;
            case 2: cout << "\nEnter isbn of the book you want to request: ";
                    cin >> isbn;
                    Book b = GiveObject(isbn);
                    b.Book_request(S.Id);
                    break;
            case 3: B.Search(2); break;
            case 4: cout << "\nEnter isbn of the book you want to return: ";
                    cin >> isbn;
                    Book b = GiveObject(isbn);
                    b.Book_return(S.Id);
                    break;
            case 5: 
                    break;
            default: cout <<"\nEnter Valid choice\n";
                break;
            }
        }while(choice!= 0);
    }
    else{
        cout<< "\nYOU HAVE REACHED THE UPPER LIMIT OF LOGIN ATTEMPTS. \n   SESSION ABORTING\n";
    }
    return 0;
}