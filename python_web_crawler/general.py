import os


# Create project directory
def create_project_dir(directory):
    if not os.path.exists(directory):
        print("Creating project : " + directory)
        os.makedirs(directory)


# Create Queue and crawled files
def create_crawl_files(project_name, base_url):
    queue = project_name + '/queue.txt'
    crawled = project_name + '/crawled.txt'

    if not os.path.isfile(queue):
        write_file(queue, base_url)

    if not os.path.isfile(crawled):
        write_file(crawled, '')


# Create a new text file
def write_file(path, data):
    f = open(path, 'w')
    f.write(data)
    f.close()


# Append to text file
def append_file(path, data):
    with open(path, 'a') as f:
        f.write(data + '\n')


# Delete content of text file
def delete_file_content(path):
    with open(path, 'w'):
        pass


# File to Set
def file_to_set(path):
    res = set()
    with open(path, 'rt') as f:
        for line in f:
            res.add(line.replace('\n', ''))
    return res


# Set to File
def set_to_file(link_set, path):
    delete_file_content(path)
    with open(path, 'a') as f:
        for link in sorted(link_set):
            f.write(link + '\n')

