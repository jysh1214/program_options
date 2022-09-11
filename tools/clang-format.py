import pathlib
import subprocess

def reformat_all_files(path):
    for file in path.iterdir():
        if file.is_dir() and (not file.name in ["build"]):
            reformat_all_files(file)
        else:
            if file.suffix in ["c", ".cpp", ".h"]:
                command = ["clang-format", "-i", "-style=file", file.resolve()]
                subprocess.run(command)
            

if __name__ == "__main__":
    path = (pathlib.Path(__file__).parent / '..').resolve()
    reformat_all_files(pathlib.Path(path))
