import os
import sys
import subprocess
import shutil
import filecmp

def test(folder_path):
    split = os.path.split(folder_path)
    folder_name = split[1]
    if folder_name == "input":
        pass
    else:
        if os.path.isdir(folder_path):
            if folder_name in binaries:
                execute(folder_path)
            else:
                for subfolder_name in os.listdir(folder_path):
                    subfolder_path = os.path.join(folder_path, subfolder_name)
                    test(subfolder_path)

def execute(folder_path):
    split = os.path.split(folder_path)
    parent_path = split[0]
    folder_name = split[1]
    input_path = os.path.join(parent_path, "input")
    if os.path.isdir(input_path):
        output_path = os.path.join(parent_path, "output")
        shutil.rmtree(output_path, ignore_errors=True)
        shutil.copytree(input_path, output_path)
        binary_path = os.path.join(bin_folder, folder_name)
        source_path = os.path.join(output_path, "test.tex")
        log_path = os.path.join(output_path, "log.txt")
        with open(log_path, 'w') as log:
            subprocess.call(binary_path + " " + source_path, stdout=log, shell=True)
        for file_name in os.listdir(folder_path):
            in_file_path = os.path.join(folder_path, file_name)
            out_file_path = os.path.join(output_path, file_name)
            if not filecmp.cmp(in_file_path, out_file_path, shallow=True):
                print("  fail", out_file_path)

root_folder = os.getcwd()

bin_folder = os.path.join(root_folder, "bin")
binaries = []
for bin_name in os.listdir(bin_folder):
    bin_path = os.path.join(bin_folder, bin_name)
    if os.access(bin_path, os.X_OK):
        binaries.append(bin_name)
if binaries:
    test_folders = sys.argv[1:] or ["test"]
    for folder in test_folders:
        folder_path = os.path.abspath(folder)
        print("========= analyzing", folder_path)
        test(folder_path)
else:
    print("no binary in ", bin_folder)
