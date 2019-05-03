#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

struct Line;

/**
 * @brief main: main function of the program.
 * @param argc number of input arguments
 * @param argv input arguments
 * @return 0 if everything went well (check is OK), 1 otherwise.
 *
 *Input parameters :
 * 1. File where the credentials are stored
 * 2. The operation to perform
 *    (ADD: Add a user-hash pair, UPDATE: update a hash, DELETE: delete a user-hash pair, CHECK: check a pair)
 * 3. User
 * 4. Hash
 */
int main(int argc, char *argv[])
{
    if(argc != 5){
        fprintf(stderr, "Expecting 4 arguments but %d were received.", argc-1);
        return 0;
    }
    string file = string(argv[1]);
    string operation = string(argv[2]);
    string user = string(argv[3]);
    string hash = string(argv[4]);

    ifstream fileStream;
    fileStream.open(file, ios_base::in);

    char line[512];
    vector<Line> v;
    while(fileStream.getline(line, 512)){
        //v.push_back
    }


    if(operation.compare("ADD")){

    }else if(operation.compare("UPDATE")){

    }else if(operation.compare("DELETE")){

    }else if(operation.compare("CHECK")){

    }



    return 0;
}

struct Line{
    enum Type {COMMENT, VALID, INVALID};
    Type type = COMMENT;
    string content;
    string user;
    string hash;

    void parse(string line);
};

void Line::parse(string line){
    if(line.at(0) == '#'){
        this->type = COMMENT;
        this->content = line;
    }else{
        string delimiter = "\t";
        unsigned long int pos = line.find(delimiter);
        if(pos == string::npos){
            this->type = INVALID;
            this->content = line;
        }else{
            string user = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
            this->user = user;
            this->hash = line;
        }
    }
}
