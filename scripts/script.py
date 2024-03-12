import sys
import os

available_os = ['win32', 'linux']

user_os = sys.platform

if(user_os not in available_os):
    print('Unsupported OS detected')
    exit()

available_commands = ['build', 'clean', 'run', 'help']

# get arguments and command from the user
args = sys.argv[1:]
command = args[0] if len(args) > 0 else None

directory = os.path.dirname(os.path.realpath(__file__))
root = directory + '/' + '../'

# check if the user has entered a command
if command is None:
    print('No command entered. Available commands are: ' + ', '.join(available_commands))
    exit()

# check if the user has entered a valid command
if command not in available_commands:
    print('Invalid command. Available commands are: ' + ', '.join(available_commands))
    exit()

# build command
if command == 'build':

    available_build_modes = ['vs2019', 'vs2022', 'gmake']
    argument = args[1] if len(args) > 1 else None

    # check if the user has entered a valid argument
    if argument is None or argument not in available_build_modes:
        print('Invalid build mode. Available build modes are: ' + ', '.join(available_build_modes))
        exit()

    if user_os == 'linux':
        argument = 'gmake'

    os.system(directory + '/' + user_os + '/' + 'premake5 ' + argument)
    exit()

# clean command
if command == 'clean':
    import shutil

    print('Cleaning project...')

    extensions = ['.sln', '.vcxproj', '.vcxproj.filters', '.vcxproj.user', 'Makefile']

    try:
        shutil.rmtree(root + 'build')
    except:
        print('Directory does not exist: ' + root + 'build')
    
    for root, dirs, files in os.walk(root):
        for file in files:
            if any(file.endswith(ext) for ext in extensions):
                try:
                    os.remove(os.path.join(root, file))
                except:
                    print('File does not exist: ' + file)
    
    exit()

# run command
if command == 'run':

    application = args[1] if len(args) > 1 else None

    # check if the user has entered an application
    if application is None:
        print('No application entered.')
        exit()

    # check if folder exists with the same name as the application
    if not os.path.isdir(root + application):
        print('Application does not exist.')
        exit()

    exe_path = root + application + '/' + application

    # check if the executable exists
    if not os.path.isfile(exe_path) and not os.path.isfile(exe_path + '.exe'):
        print('Executable does not exist.')
        exit()

    print('Running Application...')

    if(user_os == 'linux'):
        os.system("LIBGL_ALWAYS_SOFTWARE=1 " + root + application + '/' + application)
    else:
        os.system(root + application + '/' +  application + '.exe')