import sys
import os
import subprocess

okon_cli_binary = sys.argv[1]
original_db_file_path = sys.argv[2]
working_dir = sys.argv[3]
result_file_path = os.path.join(working_dir, 'result.btree')

if not os.path.isdir(working_dir):
    os.makedirs(working_dir)

def prepare():
    okon_cli_prepare_command = [
        okon_cli_binary,
        '--prepare',
        original_db_file_path,
        '--wd',
        working_dir,
        '--output',
        result_file_path
    ]

    print('Preparing with command: ' + str(okon_cli_prepare_command))
    cp = subprocess.run(okon_cli_prepare_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return cp.returncode == 0

def check():
    okon_cli_check_command = [
        okon_cli_binary,
        '--path',
        result_file_path,
        '--hash'
    ]

    all_found = True

    counter = 0

    print('Checking with command: ' + str(okon_cli_check_command + ['SHA1']))
    with open(original_db_file_path, 'r') as f:
        while True:
            line = f.readline()
            if line == '':
                return all_found

            sha1 = line.split(':')[0]
            command = okon_cli_check_command + [sha1]

            counter += 1
            if counter % 10000 == 0:
                print(counter)

            cp = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            if cp.returncode != 1:
                print('Key not found: {}'.format(sha1))
                all_found = False


if prepare() == False:
    print('Preparing failed\n{}'.format(cp.stderr))
    exit(1)


if check():
    print('Success')
    exit(0)
else:
    print('Failed')
    exit(1)
