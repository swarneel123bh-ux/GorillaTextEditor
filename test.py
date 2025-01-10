if __name__ == "__main__" : 
    import os
    extension = ".c"
    files = [f for f in os.listdir() if f.endswith(extension)]
    prompt = f'cc {" ".join(map(str, files))} -lncurses -g3'
    print(f"PROMPT :: {prompt}")
    input("Proceed?")
    os.system(prompt)
    os.system('./a.out')