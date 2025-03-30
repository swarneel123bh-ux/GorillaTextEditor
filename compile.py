if __name__ == "__main__":
    import os

    files = os.listdir('.')
    gcc_text = ""

    for file in files:
        if file in ["padview.c", "test.c"]: continue

        split_name = file.split(".")
        
        if  split_name[-1] == "c":
            gcc_text += " " + file

    print(gcc_text)
    os.system(f"gcc {gcc_text} -o main -lncurses -g3")
