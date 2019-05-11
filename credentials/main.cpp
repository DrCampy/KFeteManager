#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cmath>
#include <list>
#include <algorithm>

#include "sha256.h"
using namespace std;

class Line;
bool saveFile(const string &filename, const vector<Line> &entries);

/**
 * @brief The Line class contains a line from the text document.
 *
 * If the line contain a valid entry, stores it as VALID and stores the user and the hash in separate fields.
 * If the line contains a comment, store it as a COMMENT and stores the full line without '\n'.
 * If the line contains an invalid entry, stores it as INVALID and stores it's content without '\n'.
 *
 * Note that a line is considered valid if it is composed of any serie of chars with at least one '\t' in it.
 */
class Line{

public:
    enum Type {COMMENT, VALID, INVALID};
    Line(const string &line);
    Type getType(){return type;}
    string getContent();
    string getUser();
    string getHash();
    void update(string line);

private:
    void parse(const string &line);
    Type type = COMMENT;
    string content;
    string user;
    string hash;
};

/**
 * @brief main: main function of the program.
 * @param argc number of input arguments
 * @param argv input arguments
 * @return 0 if everything went well (check is OK), 1 otherwise.
 *
 *Input parameters :
 * 1. File where the credentials are stored
 * 2. The operation to perform
 *    (ADD: Add a user-hash pair, UPDATE: update a hash, DELETE: delete a pair, CHECK: check a pair, EXISTS: checks if an entry exists for that user)
 * 3. User
 * 4. Password
 */
int main(int argc, char *argv[])
{

    std::list<string> supportedOperations = {"ADD", "UPDATE", "DELETE", "CHECK", "EXISTS"};
    std::vector<string> test;
    if(argc < 2){
        cerr << "Error: must provide at least 1 argument. Type \"credentials HELP\" for help." << endl;
    }
    string operation = argv[1];

    if(find(supportedOperations.cbegin(), supportedOperations.cend(), operation) == supportedOperations.cend()){
        cerr << "Error: Unknwon operation: \"" << operation << "\"" << endl;
    }

    if(operation.compare("EXISTS") == 0){
        if(argc != 4){
            cerr << "Error: Operation EXISTS requires 3 arguments but " << argc-1 << " were received." << endl;
        }
    }else{
        if(argc != 5 ){
            cerr << "Error: Operation " << operation << " requires 4 arguments but " << argc-1 << " were received." << endl;
        }
    }
    if(argc < 4 || argc > 5){
        fprintf(stderr, "Expecting 3 or 4 arguments but %d were received.", argc-1);
        return 1;
    }

    unsigned int workFactor = 10;


    string file = string(argv[2]);
    string operation = string(argv[1]);
    string user = string(argv[3]);
    if(operation.compare("EXISTS") != 0 && argc != 5){

    }
    string password = string(argv[4]);

    //Computes the hash of the password.
    string hash = password + user;
    for(unsigned int i = 0; i < pow(2, workFactor); i++){
        hash = sha256(hash + user);
    }

    //Debugs
    cout << "User is " << user << endl;
    cout << "Hash is " << hash << endl;

    //Opens the given file
    ifstream fileStream;
    fileStream.open(file);

    //If file is not open, attempts to create it.
    if(!fileStream.is_open()){
        ofstream temp;
        temp.open(file, ios_base::out);
        temp.close();
        fileStream.open(file);
    }

    //If it fails again to open, file is not accessible.
    if(!fileStream.is_open()){
        fprintf(stderr, "The file %s is not accessible.", file.c_str());
        return 1;
    }
    //Loads the file.
    char line[512];
    vector<Line> v;
    while(fileStream.getline(line, 512)){
        Line l(line);
        if(l.getType() != Line::INVALID){
            v.push_back(Line(line));
        }
    }
    fileStream.close();

    if(operation.compare("ADD") == 0){
        //Looks for the existing lines to be sure that there is no existing line with that user.
        bool found = false;
        for(Line l : v){
            if(l.getUser() == user){
                found = true;
            }
        }
        if(found == false){
            //Creates a new line with adequate content.
            cout << "Adding user..." << endl;
            Line l(string(user + "\t" + hash));
            v.push_back(l);
            saveFile(file, v);
            return 0;
        }
    }else if(operation.compare("UPDATE") == 0){
        //Looks if the user we are looking for exists.
        for(auto it = v.begin(); it < v.end(); it++){
            if(it->getUser() == user){
                it->update(user + '\t' + hash);
                saveFile(file, v);
                return 0;
            }
        }
    }else if(operation.compare("DELETE") == 0){
        //Looks for the entry.
        for(auto it = v.begin(); it < v.end(); it++){
            if(it->getUser() == user){
                v.erase(it);
                saveFile(file, v);
                return 0;
            }
        }
    }else if(operation.compare("CHECK") == 0){
        //Looks for the user we are looking for.
        for(auto it = v.begin(); it < v.end(); it++){
            if(it->getUser() == user){
                if(it->getHash() == hash){
                    saveFile(file, v);
                    return 0;
                }
                return 1;
            }
        }
    }else if(operation.compare("EXISTS") == 0){
        //Looks for the user we are looking for.
        for(auto it = v.begin(); it < v.end(); it++){
            if(it->getUser() == user)
            return 0;
        }
    }else{
        fprintf(stderr, "Unknown operation : %s.\n", operation.c_str());
    }
    return 1;
}

/**
 * @brief saveFile saves the content of a vector of Line entities to a file.
 * @param filename The name of the file to save the entries to.
 * @param entries The vector of entries.
 * @return 0 if no error, 1 otherwise.
 */
bool saveFile(const string &filename, const vector<Line> &entries){
    ofstream fileStream;
    fileStream.open(filename, ios_base::out | ios_base::trunc);
    if(!fileStream.is_open()){
        return 1;
    }
    for(Line l : entries){
        if(l.getType() != Line::INVALID){
            fileStream << l.getContent() << '\n';
        }
    }
    fileStream.close();
    return 0;
}

/**
 * @brief Line::Line constructs Line structure from a line of text.
 *
 * @param line the line of text.
 */
Line::Line(const string &line){
    parse(line);
}

/**
 * @brief Line::parse loads data in the struct from a line of text. A comment or a pair or an invalid line.
 * @param line : a string containing the line of text.
 */
void Line::parse(const string &line){
    if(line.length() < 1){
            this->type = INVALID;
    }else if(line.at(0) == '#'){
        this->type = COMMENT;
        this->content = line;
    }else{
        string delimiter = "\t";
        unsigned long long int pos = line.find(delimiter);
        if(pos == string::npos || pos == 0){
            this->type = INVALID;
            this->content = line;
        }else{
            this->user = line.substr(0, pos);
            this->hash = line.substr(pos+delimiter.length());
            this->type = VALID;
        }
    }
}

/**
 * @brief Line::getContent returns the content of a line.
 *
 * @return the content of a line.
 */
string Line::getContent(){
    if(type == VALID){
        return user + "\t" + hash;
    }else{
        return content;
    }
}

/**
 * @brief Line::getUser returns the user of a valid line.
 *
 * @return the user if the line is valid. An empty string otherwise.
 */
string Line::getUser(){
    if(type == VALID){
        return user;
    }else{
        return string();
    }
}

/**
 * @brief Line::getHash returns the hash of a valid line.
 *
 * @return the hash if the line is valid. An empty string otherwise.
 */
string Line::getHash(){
    if(type == VALID){
        return hash;
    }else{
        return string();
    }
}

/**
 * @brief Line::update updates the content of the line with whatever is given in the string line.
 * @param line line of text to be used to update the line.
 */
void Line::update(string line){
    //Empties the fields
    content = hash = user = string();
    parse(line);
}
