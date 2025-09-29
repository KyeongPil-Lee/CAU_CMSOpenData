import os
import subprocess
import urllib.parse

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

def convert_root_to_https(root_url):
    """
    Convert root:// URL to https:// URL for CERN Open Data
    Simply replace root:// with https://
    """
    if root_url.startswith('root://'):
        https_url = root_url.replace('root://', 'https://')
        return https_url
    else:
        print(f"Warning: URL does not start with root://: {root_url}")
        return None

def download_with_wget(url, output_dir):
    """
    Download file using wget
    """
    filename = os.path.basename(url)
    output_path = os.path.join(output_dir, filename)
    
    # Check if file already exists
    if os.path.exists(output_path):
        print(f"File {filename} already exists in {output_dir}: skip it")
        return True
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Use wget to download
    cmd = f'wget -O "{output_path}" "{url}" --no-check-certificate' # -- no-check-certificate is added to avoid the certificate error
    print(f"Downloading: {url}")
    print(f"Command: {cmd}")
    
    result = os.system(cmd)
    if result == 0:
        print(f"Successfully downloaded: {filename}")
        return True
    else:
        print(f"Failed to download: {filename}")
        return False

if __name__ == "__main__":
    inputDir_path = 'FileList'
    # outputDir_path = '/pnfs/knu.ac.kr/data/cms/store/user/kplee/CMSOpenData/MC2016/WJetsToLNu_aMCNLO_HTTPS/WJetsToLNu_aMCNLO_HTTPS' # -- KNU
    outputDir_path = '/data2/kplee/Lecture/CMSOpenData/MC2016/WJetsToLNu_aMCNLO_test' # -- CAUCluster
    
    print("=== CMS Open Data HTTPS Download Script ===")
    print(f"Input directory: {inputDir_path}")
    print(f"Output directory: {outputDir_path}")
    print()
    
    # Get list of ROOT files from text files
    list_file = GetROOTList_TextFile(inputDir_path)
    print(f"Found {len(list_file)} files to download")
    print()
    
    success_count = 0
    failed_count = 0
    
    # Download each file
    for i, root_url in enumerate(list_file, 1):
        print(f"[{i}/{len(list_file)}] Processing: {os.path.basename(root_url)}")
        
        # Convert ROOT URL to HTTPS URL
        https_url = convert_root_to_https(root_url)
        if https_url is None:
            print(f"Skipping invalid URL: {root_url}")
            failed_count += 1
            continue
        
        # Download with wget
        if download_with_wget(https_url, outputDir_path):
            success_count += 1
        else:
            failed_count += 1
        
        print("-" * 50)
    
    print(f"\n=== Download Summary ===")
    print(f"Total files: {len(list_file)}")
    print(f"Successfully downloaded: {success_count}")
    print(f"Failed downloads: {failed_count}")
    print(f"Files saved to: {outputDir_path}")
