import pathlib
import shutil
import sys

projectName = 'chatprj'
scriptsDirectoryName = 'scripts'
sqlScriptsDirectoryName = 'sql_scripts'


def print_header_with_closing_newline(value):
    print(f'\n\n{value}\n')


def print_header_without_closing_newline(value):
    print(f'\n\n{value}')


print('\n')

currentPath = pathlib.Path.cwd()
print(f'currentPath: {currentPath}')

if currentPath.name != projectName:
    print_header_with_closing_newline(f'current working directory {currentPath} looks not like \'{projectName}\'')

sqlScriptsPath = currentPath / scriptsDirectoryName / sqlScriptsDirectoryName

if not sqlScriptsPath.is_dir():
    print_header_with_closing_newline(
        f'current working directory {currentPath} does not contain \'{sqlScriptsDirectoryName}\' folder. Exit...')
    sys.exit()

clientPaths = []
commonPaths = []
otherPaths = []

for path in sqlScriptsPath.iterdir():
    if path.is_dir() and 'client' in path.name:
        clientPaths.append(path)
    elif path.is_file():
        commonPaths.append(path)
    else:
        otherPaths.append(path)

commonProjectsPath = currentPath.parent

destPaths = []

for path in commonProjectsPath.iterdir():
    if 'build' in path.name and projectName in path.name:
        destPaths.append(path)

copyJobs = []
destsForCommonPaths = []

for clientPath in clientPaths:
    for path in clientPath.iterdir():
        dests = []

        for destPath in destPaths:
            destPathToCopyIn = destPath / sqlScriptsDirectoryName / path.parent.name
            dests.append(destPathToCopyIn / path.name)
            destsForCommonPaths.append(destPathToCopyIn)

        copyJobs.append((path, dests))

for commonPath in commonPaths:
    dests = [p / commonPath.name for p in destsForCommonPaths]
    copyJobs.append((commonPath, dests))

print_header_with_closing_newline('Found following destination paths:')
for path in sorted(destPaths):
    print(path)

print_header_with_closing_newline('Found following client source paths:')
for path in sorted(clientPaths):
    print(path)

print_header_with_closing_newline('Found following common source paths:')
for path in sorted(commonPaths):
    print(path)

print_header_without_closing_newline('Following files will be copied:')
for pair in copyJobs:
    print_header_with_closing_newline(f'source \n\n\t{pair[0]} \n\nwill be copied to')

    for dest in sorted(pair[1]):
        print(f'\t{dest}')

symbol = input('\n\nStart copy? [Y|n]:')

if symbol != 'Y':
    print('Exit...')
    sys.exit()
else:
    for pair in copyJobs:
        for dest in pair[1]:
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy(pair[0], dest)

