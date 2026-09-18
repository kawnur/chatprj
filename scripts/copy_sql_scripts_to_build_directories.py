import pathlib
import shutil
import sys


PROJECT_NAME = 'chatprj'
SCRIPTS_DIRECTORY_NAME = 'scripts'
SQL_SCRIPTS_DIRECTORY_NAME = 'sql_scripts'


def print_with_closing_newline(value):
    print(f'\n\n{value}\n')


def print_without_closing_newline(value):
    print(f'\n\n{value}')


def exit():
    print('Exit...')
    sys.exit()


def print_error(msg):
    print(f'ERROR: {msg}')


def check_cwd():
    current_path = pathlib.Path.cwd()
    print(f'current_path: {current_path}')

    if current_path.name != PROJECT_NAME:
        entry = (f"current working directory {current_path} looks not like "
                 f"'{PROJECT_NAME}' directory. Run this script from project "
                 f"directory")

        print_error(entry)

        return False

    return True


def get_current_path():
    return pathlib.Path.cwd()


def get_sql_scripts_path():
    return get_current_path() / SCRIPTS_DIRECTORY_NAME / SQL_SCRIPTS_DIRECTORY_NAME


def check_sql_scripts_path():
    path = get_sql_scripts_path()

    if not path.is_dir():
        entry = f"expected sql scripts containing path {path} not found"
        print_error(entry)

        return False

    return True


class Paths:
    def __init__(self):
        self.client_paths = []
        self.common_paths = []
        self.other_paths = []
        self.dest_paths = []
        self.copy_jobs = []

    def search_for_dest_paths_at_upper_fs_level(self):
        common_projects_path = get_current_path().parent

        for path in common_projects_path.iterdir():
            if 'build' in path.name and PROJECT_NAME in path.name:
                self.dest_paths.append(path)

    def search_for_dest_paths_at_this_fs_level(self):
        for path in get_current_path().iterdir():
            if 'build' in path.name:
                self.dest_paths.append(path)

    def init_paths(self):
        for path in get_sql_scripts_path().iterdir():
            if path.is_dir() and 'client' in path.name:
                self.client_paths.append(path)
            elif path.is_file():
                self.common_paths.append(path)
            else:
                self.other_paths.append(path)

        # self.search_for_dest_paths_at_upper_fs_level()
        self.search_for_dest_paths_at_this_fs_level()

    def init_copy_jobs(self):
        dests_for_common_paths = []

        for client_path in self.client_paths:
            for path in client_path.iterdir():
                dests = []

                for destPath in self.dest_paths:
                    dest_path_to_copy_in = (destPath / SQL_SCRIPTS_DIRECTORY_NAME
                                            / path.parent.name)

                    dests.append(dest_path_to_copy_in / path.name)
                    dests_for_common_paths.append(dest_path_to_copy_in)

                self.copy_jobs.append((path, dests))

        for common_path in self.common_paths:
            dests = [p / common_path.name for p in dests_for_common_paths]
            self.copy_jobs.append((common_path, dests))

    def init_fields(self):
        self.init_paths()
        self.init_copy_jobs()

    def print_state(self):
        print_with_closing_newline('Found following destination paths:')
        for path in sorted(self.dest_paths):
            print(path)

        print_with_closing_newline('Found following client source paths:')
        for path in sorted(self.client_paths):
            print(path)

        print_with_closing_newline('Found following common source paths:')
        for path in sorted(self.common_paths):
            print(path)

        print_without_closing_newline('Following files will be copied:')
        for pair in self.copy_jobs:
            print_with_closing_newline(
                f'source \n\n\t{pair[0]} \n\nwill be copied to')

            for dest in sorted(pair[1]):
                print(f'\t{dest}')

    def check_state(self):
        containers = {
            'client_paths': self.client_paths,
            'common_paths': self.common_paths,
            # 'other_paths': self.other_paths,
            'dest_paths': self.dest_paths,
            'copy_jobs': self.copy_jobs
        }

        result = True

        for k, v in containers.items():
            if not v:
                print_error(f"container {k} is empty")
                result = False

        return result


def run_copy_jobs(paths):
    for pair in paths.copy_jobs:
        for dest in pair[1]:
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy(pair[0], dest)


def ask_and_run(paths):
    paths.print_state()
    symbol = input('\n\nStart copy? [Y|n]:')

    if symbol != 'Y':
        print('Exit...')
        sys.exit()
    else:
        run_copy_jobs(paths)


def main():
    print('\n')

    if not check_cwd() or not check_sql_scripts_path():
        exit()

    paths = Paths()
    paths.init_fields()

    if not paths.check_state():
        exit()

    ask_and_run(paths)


if __name__ == '__main__':
    main()
