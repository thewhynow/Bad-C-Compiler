#include "codegen/codegen.h"

char* openfile(const char* filename){
    FILE* file = fopen(filename, "r");
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* fileBuff = malloc(filesize + 1);
    fread(fileBuff, 1, filesize, file);
    fileBuff[filesize] = '\0';
    fclose(file);
    return fileBuff;
}

/*
    arg1: input file
    arg2: ouput file
*/

int main(int argc, char* argv[]){
    char* buff = openfile(argv[1]);
    Token* head = lex(buff);

    ProgramNode* AST = parse(head, buff);
    deltokens(head);

    Program* program = linearize(AST);

    codegen(program, argv[2]);
}