if __name__ == "__main__" :
    import os

    files = os.listdir('.')
    exclude_list = []
    compile_str = ""

    for file in files:
        split_name = file.split(".")

        if split_name[-1] == "c" and split_name[-1] not in exclude_list:
            compile_str += file + " "

    final_compile_str = "gcc " + compile_str + " -o main -lncurses -Wall -Werror -Wpedantic -g3"
    os.system(final_compile_str)
    print("Compile script finished execution")