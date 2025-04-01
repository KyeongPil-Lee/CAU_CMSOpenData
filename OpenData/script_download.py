import os
import subprocess

# -- open all .txt file under dir_path
# -- read the file line by line
# -- if the line start with "root://" then add it to the list
# -- return the list
def GetROOTList_TextFile(dir_path):
    list_file = []
    for file in os.listdir(dir_path):
        if file.endswith('.txt'):
            with open(os.path.join(dir_path, file), 'r') as f:
                for line in f:
                    if line.startswith('root://'):
                        list_file.append(line.strip())
    return list_file

def check_proxy():
    try:
        # voms-proxy-info check
        result = subprocess.run(['voms-proxy-info', '--exists'], 
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE,
                              universal_newlines=True)
        if result.returncode == 0:
            print("Grid proxy is valid")
            return True
        else:
            print("Grid proxy is not valid or expired")
            return False
    except FileNotFoundError:
        print("voms-proxy-info command not found. Please check if VOMS client is installed.")
        return False

if __name__ == "__main__":
    inputDir_path = 'FileList'
    outputDir_path = '/pnfs/knu.ac.kr/data/cms/store/user/kplee/CMSOpenData/Run2016H/SingleMuon'

    # -- check CERN grid proxy
    if not check_proxy():
        print("Please initialize your grid proxy first using 'voms-proxy-init -voms cms'")
        exit(1)

    list_file = GetROOTList_TextFile(inputDir_path)
    # -- download the file via xrdcp
    for file in list_file:
        print(f"downloading {file}")
        # print(f'xrdcp {file} {outputDir_path}')

        # -- check if the file is already in the output directory
        if os.path.exists(os.path.join(outputDir_path, os.path.basename(file))):
            print(f"file {os.path.basename(file)} already exists in {outputDir_path}: skip it")
            continue

        os.system(f'xrdcp {file} {outputDir_path}')



import os
import subprocess

# -- open all .txt file under dir_path
# -- read the file line by line
# -- if the line start with "root://" then add it to the list
# -- return the list
def GetROOTList_TextFile(dir_path):
    list_file = []
    for file in os.listdir(dir_path):
        if file.endswith('.txt'):
            with open(os.path.join(dir_path, file), 'r') as f:
                for line in f:
                    if line.startswith('root://'):
                        list_file.append(line.strip())
    return list_file

def check_proxy():
    try:
        # voms-proxy-info check
        result = subprocess.run(['voms-proxy-info', '--exists'], 
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE,
                              universal_newlines=True)
        if result.returncode == 0:
            print("Grid proxy is valid")
            return True
        else:
            print("Grid proxy is not valid or expired")
            return False
    except FileNotFoundError:
        print("voms-proxy-info command not found. Please check if VOMS client is installed.")
        return False

if __name__ == "__main__":
    inputDir_path = 'FileList'
    outputDir_path = '/pnfs/knu.ac.kr/data/cms/store/user/kplee/CMSOpenData/Run2016H/SingleMuon'

    # -- check CERN grid proxy
    if not check_proxy():
        print("Please initialize your grid proxy first using 'voms-proxy-init -voms cms'")
        exit(1)

    list_file = GetROOTList_TextFile(inputDir_path)
    # -- download the file via xrdcp
    for file in list_file:
        print(f"downloading {file}")
        # print(f'xrdcp {file} {outputDir_path}')
        os.system(f'xrdcp {file} {outputDir_path}')



