import argparse
import os
import platform

def main():
    parser = argparse.ArgumentParser(description="Bad C Compiler (BCC) Command Line Tool")
    parser.add_argument("-c", type=str, action="append", required=False, help="C file(s)")
    parser.add_argument("-o", type=str, required=False, help="Output Executeable")
    parser.add_argument("-a", type=str, action="append", required=False, help="C file(s) to generate assembly for")

    args = parser.parse_args()

    if os.system("gcc bcc_code.c error/error.c TACI_gen/intermediate_rep_gen.c codegen/codegen.c lexer/lexer.c parser/parser.c parser/int_vector/int_vector.c parser/parse_expr/parse_expr.c parser/scope_info_map/src/BST.c parser/scope_info_map/scope_info/scope_info.c -lm -w -o bcc -std=c17"):
        print("ERROR: failed to compile bcc")
        return -1
            
    for file in args.c:
        if os.system(f"./bcc {file} {file}.s > tmp"):
            print(f"ERROR: failed to compile {file}")
            return -1

    if platform.system() == "Linux":
        if os.system(f"gcc {" ".join([f"{_file}.s" for _file in args.c])} -o {args.o}"):
            print("Compiler generated invalid assembly")
            return -1
        for file in args.c:
            os.system(f"rm {file}.s")
    else:
        for file in args.c:
            if os.system(f"as -s {file}.asm -o {file}.o"):
                print("Compiler generated invalid assembly")
                return -1
            else:
                os.system(f"rm {file}.asm")

    if args.a:
        for file in args.a:
            if os.system(f"./bcc {file} {file}.s > tmp"):
                print(f"ERROR: failed to compile {file}")
                return -1

    os.system("rm bcc && rm tmp")

if __name__ == "__main__":
    main()